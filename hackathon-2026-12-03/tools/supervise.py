#!/usr/bin/env python3
"""Superviseur resilient pour la preparation du hackathon CodinGame du 3 decembre.

Proprietes clefs :
  * Chaque etape est executee par `opencode run` dans une SESSION DEDIEE avec un
    modele explicite (pas de sous-agent imbrique, pour garder des contextes courts).
  * Tout est checkpoint atomiquement dans supervisor_state.json apres chaque evenement.
  * Si le processus est tue, si les tokens s'epuisent ou si le contexte deborde,
    le redemarrage reprend EXACTEMENT l'etape en cours dans la meme session.
  * En cas de quota / erreur de modele, on descend dans la chaine de repli du
    fichier MODELS-RANKING.md, puis on attend et on recommence.
  * Les passations entre etapes se font par des fichiers `handoff.md` courts :
    jamais l'historique complet d'une session n'est recharge.

Le token CodinGame n'est jamais lu ni ecrit ici (le superviseur ne parle pas a
CodinGame, il ne fait qu'orchestrer des sessions OpenCode).
"""
import argparse
import fcntl
import json
import os
import re
import subprocess
import sys
import time
import traceback
from datetime import datetime, timezone

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
STATE_PATH = os.path.join(ROOT, "supervisor_state.json")
CONFIG_PATH = os.path.join(ROOT, "supervisor.json")
STEPS_PATH = os.path.join(ROOT, "steps.json")
LOG_PATH = os.path.join(ROOT, "logs", "supervisor.log")
STATUS_PATH = os.path.join(ROOT, "STATUS.md")
STOP_PATH = os.path.join(ROOT, "supervisor.stop")
LOCK_PATH = os.path.join(ROOT, "supervisor.lock")
PROMPTS_DIR = os.path.join(ROOT, "prompts")

FAILURE_RE = re.compile(
    r"quota|usage limit|free usage|rate.?limit|too many requests|429|503|overloaded|"
    r"context length|context window|token limit|maximum context|budget exceeded|"
    r"insufficient credit|model not found|unauthorized|payment required|capacity|"
    r"temporarily unavailable",
    re.I,
)


def now():
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def log(message):
    line = f"{now()} {message}"
    print(line, flush=True)
    try:
        os.makedirs(os.path.dirname(LOG_PATH), exist_ok=True)
        with open(LOG_PATH, "a", encoding="utf-8") as f:
            f.write(line + "\n")
    except OSError:
        pass


def load_json(path, default):
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except (FileNotFoundError, ValueError):
        return default


def save_json(path, obj):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    tmp = path + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(obj, f, indent=2, ensure_ascii=False)
    os.replace(tmp, path)


def load_state():
    s = load_json(STATE_PATH, {})
    s.setdefault("current", "phase1.research")
    s.setdefault("status", "pending")
    s.setdefault("attempt", 0)
    s.setdefault("round", 0)
    s.setdefault("model_index", 0)
    s.setdefault("session_id", None)
    s.setdefault("session_used", False)
    s.setdefault("completed", [])
    s.setdefault("challenges", [])
    s.setdefault("phase2_steps", {})
    s.setdefault("last_error", "")
    s.setdefault("last_model", "")
    s.setdefault("started_at", now())
    return s


def save_state(s):
    save_json(STATE_PATH, s)
    write_status(s)


def write_status(s):
    lines = [
        "# Etat du superviseur",
        "",
        f"- Mise a jour : `{now()}`",
        f"- Etape courante : **{s['current']}** ({s['status']})",
        f"- Modele : `{s.get('last_model') or '-'}`",
        f"- Tentative : {s['attempt']} (cycle {s['round']})",
        f"- Etapes terminees : {len(s['completed'])}",
        f"- Challenges prévus : {len(s.get('challenges', []))}",
        "",
    ]
    if s.get("last_error"):
        lines += [f"Derniere erreur : `{s['last_error'][:300]}`", ""]
    lines += ["## Etapes terminees", ""]
    for item in s["completed"][-20:]:
        lines.append(f"- {item.get('at','')} `{item.get('step')}` -> {item.get('artifacts', [])}")
    if not s["completed"]:
        lines.append("_(aucune pour l'instant)_")
    lines.append("")
    lines.append("Pour arreter : `touch hackathon-2026-12-03/supervisor.stop`")
    with open(STATUS_PATH, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


def slug(text):
    return re.sub(r"[^a-zA-Z0-9]+", "-", text).strip("-").lower()[:60]


def get_step(state, steps):
    sid = state["current"]
    if sid in steps:
        return steps[sid]
    if sid in state.get("phase2_steps", {}):
        return state["phase2_steps"][sid]
    return None


def read_handoff(state):
    """Compact passation : uniquement le dernier handoff et la liste des etapes."""
    parts = []
    last = state["completed"][-1] if state["completed"] else None
    if last and last.get("handoff"):
        path = os.path.join(ROOT, last["handoff"])
        try:
            text = open(path, encoding="utf-8").read()
            parts.append("=== PASSATION DE L'ETAPE PRECEDENTE ===\n" + text[:8000])
        except OSError:
            pass
    parts.append("=== ETAPES DEJA TERMINEES ===\n" +
                 "\n".join(f"- {i.get('step')}: {i.get('summary', '')[:200]}"
                           for i in state["completed"]))
    if state.get("challenges"):
        parts.append("=== LISTE DES CHALLENGES ===\n" +
                     json.dumps(state["challenges"], ensure_ascii=False)[:4000])
    return "\n\n".join(parts)


def build_prompt(state, step):
    template = os.path.join(PROMPTS_DIR, step["prompt"])
    try:
        text = open(template, encoding="utf-8").read()
    except OSError:
        text = "Fais le travail de l'etape " + state["current"] + "."
    replacements = dict(step.get("vars", {}))
    replacements["STEP_ID"] = state["current"]
    for k, v in replacements.items():
        text = text.replace("{" + k + "}", str(v))
    handoff = read_handoff(state)
    text += (
        "\n\n=== REGLES DE REPRISE ET DE PASSATION (IMPERATIF) ===\n"
        "1. Lis toutes les Instructions ci-dessus et les fichiers deja presents. "
        "Si un artefact existe deja, continues a partir de lui, ne recommence pas depuis zero.\n"
        "2. Ecris TOUS les fichiers demandes avec des chemins ABSOLUS sous "
        f"{ROOT}/.\n"
        "3. Avant de terminer, ecris le fichier de passation "
        f"{ROOT}/" + step.get("handoff", "HANDOFF.md") + " en markdown, "
        "SUR 3000 CARACTERES MAXIMUM : ce qui a ete fait, les fichiers ecrits, "
        "les decisions prises, les points ouverts, et la consigne pour l'etape suivante.\n"
        "4. Ne lance PAS d'autre sous-agent. Ne parle PAS a l'API CodinGame. "
        "Ne lis jamais ~/.config/codingame/token.\n"
        "5. Si tu n'as plus de tokens ou plus de contexte, ecris quand meme le "
        "fichier de passation avec l'etat exact avant de t'arreter.\n\n"
        "=== PASSATION / CONTEXTE REDUIT ===\n" + handoff + "\n"
    )
    return text


def pick_model(state, cfg, step):
    chain = cfg["models"].get(step["role"], cfg["models"]["code"])
    idx = state["model_index"] % len(chain)
    return chain[idx], chain


def run_once(state, cfg, step, model, prompt):
    sid = state.get("session_id") or ("ses-hack-" + slug(state["current"]))
    state["session_id"] = sid
    cmd = ["opencode", "run", "--model", model, "--auto", "--archived",
           "--session", sid]
    if state.get("session_used"):
        cmd.append("--continue")
    cmd += ["--title", state["current"], prompt]
    os.makedirs(os.path.join(ROOT, "logs"), exist_ok=True)
    out_path = os.path.join(ROOT, "logs",
                            f"{slug(state['current'])}-{state['round']}-"
                            f"{state['model_index']}-{state['attempt']}.out")
    log(f"run: etape={state['current']} modele={model} session={sid}")
    with open(out_path, "wb") as out:
        proc = subprocess.Popen(cmd, cwd=ROOT, stdout=out,
                                stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL)
        rc = proc.wait()
    try:
        tail = open(out_path, encoding="utf-8", errors="replace").read()[-2000:]
    except OSError:
        tail = ""
    return rc, tail, out_path


def artifacts_ok(step):
    for rel in step.get("artifacts", []):
        path = os.path.join(ROOT, rel)
        if not os.path.isfile(path) or os.path.getsize(path) < 40:
            return False, rel
    return True, None


CONTEXT_RE = re.compile(
    r"context length|context window|token limit|maximum context|"
    r"conversation too long|prompt too long|context overflow",
    re.I,
)


def failure_kind(rc, tail):
    if rc != 0 and CONTEXT_RE.search(tail):
        return "context"
    if rc != 0 and FAILURE_RE.search(tail):
        return "quota"
    if rc != 0:
        return "error"
    return None


def advance(state, steps):
    """Passe a l'etape suivante, en developpant la Phase 2 si besoin."""
    done = state["current"]
    done_def = steps.get(done) or state.get("phase2_steps", {}).get(done, {})
    nxt = done_def.get("next")
    if done == "phase2.index":
        challenges = load_json(os.path.join(ROOT, "challenges.json"), [])
        if not challenges:
            log("phase2.index n'a pas produit challenges.json -- arret propre")
            return False
        state["challenges"] = challenges
        for c in challenges:
            base = f"phase2.{c['year']}-{slug(c['id'])}"
            for i, kind in enumerate(["research", "plan", "critique", "implement"]):
                sid = f"{base}.{kind}"
                state["phase2_steps"][sid] = {
                    "role": "deep_search" if kind == "research" else "code",
                    "prompt": f"phase2_{kind}.md",
                    "handoff": f"challenges/{c['year']}-{slug(c['id'])}/HANDOFF.md",
                    "artifacts": ARTIFACTS[kind](c, slug(c['id'])),
                    "vars": {
                        "YEAR": c["year"], "CHALLENGE_ID": c["id"],
                        "CHALLENGE_NAME": c.get("name", c["id"]),
                        "CHALLENGE_DIR": f"challenges/{c['year']}-{slug(c['id'])}",
                    },
                    "next": (f"{base}.plan" if kind == "research"
                             else f"{base}.critique" if kind == "plan"
                             else f"{base}.implement" if kind == "critique"
                             else next_challenge_start(challenges, c)),
                }
        log(f"Phase 2 developpee : {len(challenges)} challenges")
        first = challenges[0]
        nxt = f"phase2.{first['year']}-{slug(first['id'])}.research"
    state["current"] = nxt if nxt else "__DONE__"
    state["status"] = "pending"
    state["attempt"] = 0
    state["round"] = 0
    state["model_index"] = 0
    state["session_id"] = None
    state["session_used"] = False
    state["last_error"] = ""
    return True


def next_challenge_start(challenges, current):
    for i, c in enumerate(challenges):
        if c is current or c.get("id") == current.get("id"):
            if i + 1 < len(challenges):
                n = challenges[i + 1]
                return f"phase2.{n['year']}-{slug(n['id'])}.research"
    return "__DONE__"


def ARTIFACTS(kind, folder):
    if kind == "research":
        return lambda c, f: [f"challenges/{f}/statement.md", f"challenges/{f}/research.md"]
    if kind == "plan":
        return lambda c, f: [f"challenges/{f}/plan.md"]
    if kind == "critique":
        return lambda c, f: [f"challenges/{f}/critique.md"]
    return lambda c, f: [f"challenges/{f}/solutions/main.c",
                         f"challenges/{f}/solutions/main.cpp",
                         f"challenges/{f}/solutions/main.rs",
                         f"challenges/{f}/solutions/main.py"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--supervise", action="store_true",
                    help="boucle et redemarre apres chaque etape")
    ap.add_argument("--once", action="store_true",
                    help="execute une seule etape puis s'arrete")
    args = ap.parse_args()

    cfg = load_json(CONFIG_PATH, {})
    steps = load_json(STEPS_PATH, {})
    state = load_state()

    lock = open(LOCK_PATH, "w")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except OSError:
        log("un autre superviseur tourne deja -- sortie")
        return 0

    while True:
        if os.path.exists(STOP_PATH):
            log("fichier supervisor.stop present -- arret propre")
            return 0
        state = load_state()
        if state["current"] == "__DONE__":
            log("toutes les etapes sont terminees")
            return 0
        step = get_step(state, steps)
        if step is None:
            log(f"etape inconnue: {state['current']} -- arret")
            return 2

        if state["status"] != "running":
            state["status"] = "running"
            state["attempt"] = 0
            state["round"] = 0
            state["model_index"] = 0
            state["session_id"] = None
            state["session_used"] = False
            save_state(state)

        model, chain = pick_model(state, cfg, step)
        prompt = build_prompt(state, step)
        state["last_model"] = model
        save_state(state)
        rc, tail, out_path = run_once(state, cfg, step, model, prompt)
        state["session_used"] = True
        state["last_error"] = tail[-300:]
        save_state(state)

        ok, missing = artifacts_ok(step)
        if rc == 0 and ok:
            state["completed"].append({
                "at": now(), "step": state["current"], "model": model,
                "artifacts": step.get("artifacts", []),
                "handoff": step.get("handoff", "HANDOFF.md"),
                "summary": tail[-400:].replace("\n", " "),
            })
            save_state(state)
            log(f"OK {state['current']} -> {out_path}")
            if not advance(state, steps):
                return 3
            save_state(state)
            if args.once:
                return 0
            continue

        kind = failure_kind(rc, tail)
        reason = (f"artefact manquant {missing}" if rc == 0 and not ok
                  else f"{kind or 'echec'} rc={rc}")
        log(f"ECHEC {state['current']}: {reason} ({out_path})")

        if kind == "context":
            # Contexte plein : on repart dans une session neuve avec la passation
            # compacte, au lieu de rejouer un historique qui deborde deja.
            log(f"contexte sature sur {state['current']} : nouvelle session propre")
            state["session_id"] = None
            state["session_used"] = False

        state["attempt"] += 1
        max_attempts = int(cfg.get("max_attempts_per_model", 1))
        if state["attempt"] >= max_attempts:
            state["attempt"] = 0
            state["model_index"] += 1
            if state["model_index"] >= len(chain):
                state["model_index"] = 0
                state["round"] += 1
                wait = int(cfg.get("cycle_wait_seconds", 900))
                log(f"tous les modeles ont echoue, cycle {state['round']}: "
                    f"attente {wait}s")
                if os.path.exists(STOP_PATH):
                    return 0
                time.sleep(wait)
        state["last_error"] = reason
        save_state(state)
        if args.once:
            return 1
        time.sleep(int(cfg.get("retry_delay_seconds", 20)))


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception:
        traceback.print_exc()
        sys.exit(99)
