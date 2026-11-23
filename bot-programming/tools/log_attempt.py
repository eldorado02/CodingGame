#!/usr/bin/env python3
"""Append an attempt to IMPROVEMENTS.md (+ challenge critique.md) and update
the game's row in PROGRESS.md.

Usage:
  python3 bot-programming/tools/log_attempt.py <game> --title "v1 soumise" \
      --constat ... --hyp ... --sources ... --action ... --result ... --lesson ... \
      [--league "Wood (unique)"] [--state "Soumis `id` (...)"] [--target rang]
       [--challenge-dir hackathon-2026-12-03/challenges/<dir>]
Dates use today's date. Format: constat / hypothèse / sources / action /
résultat / leçon (PLAYBOOK).
"""
import argparse
import datetime
import os
import re

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))


def entry(a, game_title):
    return (f"\n### {datetime.date.today().isoformat()} — {game_title} — {a.title}\n"
            f"- Constat : {a.constat}\n- Hypothèse : {a.hyp}\n- Sources : {a.sources}\n"
            f"- Action : {a.action}\n- Résultat : {a.result}\n- Leçon : {a.lesson}\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("game")
    for k in ("title", "constat", "hyp", "sources", "action", "result", "lesson"):
        ap.add_argument("--" + k, required=True)
    ap.add_argument("--league")
    ap.add_argument("--state")
    ap.add_argument("--target", default=None)
    ap.add_argument("--challenge-dir")
    a = ap.parse_args()

    prog = os.path.join(ROOT, "bot-programming", "PROGRESS.md")
    s = open(prog, encoding="utf-8").read()
    pat = re.compile(r"^\| (\d+) \| `" + re.escape(a.game) + r"` \| ([^|]*) \|([^|]*)\|([^|]*)\|([^|]*)\|([^|]*)\|$", re.M)
    m = pat.search(s)
    title = m.group(2).strip() if m else a.game
    if m and (a.league or a.state):
        league = a.league or m.group(3).strip()
        rank = "en cours" if a.state else m.group(4).strip()
        state = a.state or m.group(5).strip()
        target = a.target or m.group(6).strip()
        row = f"| {m.group(1)} | `{a.game}` | {title} | {league} | {rank} | {state} | {target} |"
        s = s[:m.start()] + row + s[m.end():]
        open(prog, "w", encoding="utf-8").write(s)
    e = entry(a, title)
    with open(os.path.join(ROOT, "bot-programming", "IMPROVEMENTS.md"), "a", encoding="utf-8") as f:
        f.write(e)
    if a.challenge_dir:
        with open(os.path.join(ROOT, a.challenge_dir, "critique.md"), "a", encoding="utf-8") as f:
            f.write(e)
    print("logged:", title, "-", a.title, "(PROGRESS row updated)" if m else "(no PROGRESS row)")


if __name__ == "__main__":
    main()
