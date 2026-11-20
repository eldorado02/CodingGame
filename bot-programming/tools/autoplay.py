#!/usr/bin/env python3
"""Autonomous CodinGame league driver with self-healing rate-limit handling.

Designed to run detached for days. It survives:
  * CodinGame rate limits (reads `limits[].resets_in_seconds`, sleeps, retries
    the same action instead of dropping it);
  * network errors (retry with backoff);
  * a killed/restarted process (every phase is checkpointed in state.json);
  * the agent session ending (run it under `nohup`, it does not depend on me).

It stops cleanly when a new league is entered only if `stop_on_promotion` is
true in autoplay.json. Otherwise it picks the configured bot for the new
league, saves the new statement, and keeps going.

Token is read from ~/.config/codingame/token and never printed or persisted.
"""
import argparse
import json
import os
import sys
import time
import traceback
import urllib.error
import urllib.request
from datetime import datetime, timezone

API = "https://www.codingame.com/api/v1"
TOKEN_PATH = os.path.expanduser("~/.config/codingame/token")
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
GAME = "mad-pod-racing"
GAME_DIR = os.path.join(ROOT, "games", GAME)
STATE_PATH = os.path.join(GAME_DIR, "autoplay_state.json")
CONFIG_PATH = os.path.join(GAME_DIR, "autoplay.json")
LOG_DIR = os.path.join(GAME_DIR, "logs")
STATEMENT_DIR = os.path.join(LOG_DIR, "statements")
STOP_PATH = os.path.join(GAME_DIR, "autoplay.stop")

DEFAULT_CONFIG = {
    "test_games": 4,
    "min_wins": 3,
    "poll_seconds": 30,
    "retry_margin_seconds": 3,
    "restart_delay_seconds": 30,
    "stop_on_promotion": False,
    "league_bots": {
        "Wood 3": ["bot.py", "Python3"],
        "Wood 2": ["bot.py", "Python3"],
        "Wood 1": ["bot.py", "Python3"],
        "Bronze": ["bot.py", "Python3"],
        "Silver": ["bot.py", "Python3"],
        "Gold": ["bot_gold.cpp", "C++"],
        "Legend": ["bot_gold.cpp", "C++"],
    },
}

_next_allowed = 0.0


def now():
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def log(message):
    line = f"{now()} {message}"
    print(line, flush=True)
    try:
        os.makedirs(LOG_DIR, exist_ok=True)
        with open(os.path.join(LOG_DIR, "autoplay.log"), "a") as f:
            f.write(line + "\n")
    except OSError:
        pass


def load_config():
    cfg = dict(DEFAULT_CONFIG)
    try:
        with open(CONFIG_PATH) as f:
            cfg.update(json.load(f))
    except FileNotFoundError:
        pass
    return cfg


def load_state():
    try:
        with open(STATE_PATH) as f:
            return json.load(f)
    except (FileNotFoundError, ValueError):
        return {}


def save_state(state):
    tmp = STATE_PATH + ".tmp"
    with open(tmp, "w") as f:
        json.dump(state, f, indent=2)
    os.replace(tmp, STATE_PATH)


def token():
    with open(TOKEN_PATH) as f:
        return f.read().strip()


def limit_wait(payload, margin):
    """Seconds to wait based on an API response's `limits` block."""
    best = 0.0
    if not isinstance(payload, dict):
        return best
    limits = payload.get("limits") or {}
    for info in limits.values():
        if not isinstance(info, dict):
            continue
        if info.get("remaining", 1) == 0:
            best = max(best, float(info.get("resets_in_seconds", 0)) + margin)
    return best


def api(method, path, body=None, margin=3):
    """Call the API, sleeping through rate limits and transient failures."""
    global _next_allowed
    while True:
        if os.path.exists(STOP_PATH):
            raise SystemExit("stop file present")
        delay = _next_allowed - time.time()
        if delay > 0:
            log(f"rate limit: sleeping {delay:.0f}s before {method} {path}")
            time.sleep(delay)

        data = None
        if body is not None:
            data = json.dumps(body).encode()
        headers = {"Authorization": "Bearer " + token()}
        if data is not None:
            headers["Content-Type"] = "application/json"
        req = urllib.request.Request(
            path if path.startswith("http") else API + path,
            data=data, headers=headers, method=method,
        )
        try:
            with urllib.request.urlopen(req, timeout=60) as resp:
                payload = resp.read().decode()
        except urllib.error.HTTPError as e:
            raw = e.read().decode()
            retry_after = e.headers.get("Retry-After")
            if retry_after and retry_after.isdigit():
                wait = int(retry_after) + margin
            else:
                try:
                    wait = limit_wait(json.loads(raw), margin) or 30
                except ValueError:
                    wait = 30
            _next_allowed = time.time() + wait
            log(f"HTTP {e.code} on {path}: {raw[:160]} -- retry in {wait:.0f}s")
            continue
        except (urllib.error.URLError, TimeoutError, OSError) as e:
            _next_allowed = time.time() + 10
            log(f"network error on {path}: {e} -- retry in 10s")
            continue

        try:
            parsed = json.loads(payload)
        except ValueError:
            return payload
        wait = limit_wait(parsed, margin)
        if wait > 0:
            _next_allowed = max(_next_allowed, time.time() + wait)
        return parsed


def save_statement(league):
    text = api("GET", f"/puzzles/{GAME}")
    statement = text.get("statement", "") if isinstance(text, dict) else str(text)
    os.makedirs(STATEMENT_DIR, exist_ok=True)
    slug = league.lower().replace(" ", "_")
    path = os.path.join(STATEMENT_DIR, f"{slug}.md")
    with open(path, "w") as f:
        f.write(statement)
    log(f"saved statement for {league} to {path}")
    return path


def save_test_log(log_text, tag):
    os.makedirs(LOG_DIR, exist_ok=True)
    path = os.path.join(LOG_DIR, f"autoplay-{tag}-{int(time.time())}.log")
    with open(path, "w") as f:
        f.write(log_text or "")
    return path


def wait_test(resp, margin):
    """Follow a 202 test game until it finishes."""
    while isinstance(resp, dict) and resp.get("status") not in ("done", "failed", None):
        tid = resp.get("test_game_id")
        if not tid:
            break
        time.sleep(2)
        resp = api("GET", f"/test-games/{tid}?wait=30", margin=margin)
    return resp


def is_win(log_text):
    return bool(log_text) and "scores: 0=2" in log_text


def run_tests(state, cfg):
    bot, language = state["bot"], state["language"]
    code = open(os.path.join(GAME_DIR, bot)).read()
    wins = 0
    total = int(cfg["test_games"])
    log(f"testing {bot} ({language}) {total} games vs {state.get('league')} boss")
    for i in range(total):
        body = {"language": language, "code": code, "opponents": ["default"]}
        resp = api("POST", f"/puzzles/{GAME}/test-games?wait=30", body,
                   margin=cfg["retry_margin_seconds"])
        resp = wait_test(resp, cfg["retry_margin_seconds"])
        text = resp.get("log", "") if isinstance(resp, dict) else str(resp)
        path = save_test_log(text, f"{state['league'].replace(' ', '_')}-{i + 1}")
        won = is_win(text)
        wins += int(won)
        state["wins"] = wins
        state["games_done"] = i + 1
        save_state(state)
        log(f"test {i + 1}/{total}: {'WIN' if won else 'LOSS'} -> {path}")
    state["phase"] = "submit" if wins >= int(cfg["min_wins"]) else "blocked"
    log(f"tests: {wins}/{total} wins -> phase={state['phase']}")
    save_state(state)


def do_submit(state, cfg):
    bot, language = state["bot"], state["language"]
    code = open(os.path.join(GAME_DIR, bot)).read()
    resp = api("POST", f"/puzzles/{GAME}/submissions",
               {"language": language, "code": code},
               margin=cfg["retry_margin_seconds"])
    state["submission_id"] = resp.get("submission_id")
    state["phase"] = "rank"
    state["wins"] = 0
    state["games_done"] = 0
    save_state(state)
    log(f"submitted {bot} as {state['submission_id']}")


def handle_new_league(progress, state, cfg):
    league = progress["league"]
    save_statement(league)
    state["league"] = league
    state["history"].append({
        "at": now(), "league": league,
        "submission_id": progress.get("submission_id"),
    })
    if cfg.get("stop_on_promotion"):
        state["phase"] = "await_human"
        save_state(state)
        log(f"promotion to {league}: stopping for statement review")
        raise SystemExit(0)
    bot, language = cfg["league_bots"].get(
        league, cfg["league_bots"].get("Bronze", ["bot.py", "Python3"]))
    state["bot"], state["language"] = bot, language
    state["phase"] = "test"
    state["wins"] = 0
    state["games_done"] = 0
    save_state(state)
    log(f"promotion to {league}: using {bot} ({language})")


def step(state, cfg):
    progress = api("GET", f"/puzzles/{GAME}/progress",
                   margin=cfg["retry_margin_seconds"])
    league = progress["league"]

    if state.get("league") is None:
        state["league"] = league
        state.setdefault("history", [])
        state.setdefault("phase", "rank")
        if state["phase"] == "rank" and state.get("bot") is None:
            bot, language = cfg["league_bots"].get(league, ["bot.py", "Python3"])
            state["bot"], state["language"] = bot, language
        save_state(state)
        log(f"initialised in {league}, phase={state['phase']}, bot={state['bot']}")

    if league != state["league"]:
        handle_new_league(progress, state, cfg)
        return

    state["last_progress"] = {
        "percentage": progress.get("percentage"),
        "score": progress.get("score"),
        "rank": progress.get("rank"),
        "eligible_for_promotion": progress.get("eligible_for_promotion"),
    }
    save_state(state)

    phase = state["phase"]
    if phase == "await_human":
        log("awaiting human review (remove autoplay.stop or clear phase)")
        raise SystemExit(0)

    if phase == "blocked":
        log(f"BLOCKED in {league}: {state.get('wins')}/{state.get('games_done')} "
            f"wins, need {cfg['min_wins']}/{cfg['test_games']}. "
            f"Improve the bot, then clear phase in {STATE_PATH}")
        raise SystemExit(2)

    if phase == "rank":
        if not progress.get("in_progress"):
            if progress.get("eligible_for_promotion"):
                log(f"ranked {progress.get('score')} (boss "
                    f"{progress.get('league_boss', {}).get('score')}), "
                    f"waiting for promotion")
            else:
                log("ranking finished without promotion; retesting this bot")
                state["phase"] = "test"
                save_state(state)
        time.sleep(int(cfg["poll_seconds"]))
        return

    if phase == "test":
        run_tests(state, cfg)
        return

    if phase == "submit":
        do_submit(state, cfg)
        return

    state["phase"] = "rank"
    save_state(state)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--supervise", action="store_true",
                    help="restart automatically after a crash")
    args = ap.parse_args()
    cfg = load_config()
    while True:
        try:
            state = load_state()
            step(state, cfg)
            if not args.supervise:
                return 0
        except SystemExit as e:
            # Code 2 means "blocked, needs a better bot": never auto-retry it.
            if isinstance(e.code, int) and e.code != 0:
                if not args.supervise or e.code == 2:
                    return e.code
                log(f"step exited {e.code}; retrying in "
                    f"{cfg['restart_delay_seconds']}s")
            else:
                return 0
        except Exception:
            log("crash:\n" + traceback.format_exc())
            if not args.supervise:
                return 1
        if args.supervise and os.path.exists(STOP_PATH):
            log("stop file present, exiting")
            return 0
        if args.supervise:
            time.sleep(int(cfg["restart_delay_seconds"]))


if __name__ == "__main__":
    sys.exit(main())
