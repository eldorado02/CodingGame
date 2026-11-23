#!/usr/bin/env python3
"""Run N CodinGame test games and summarise wins.

Usage:
  python3 bot-programming/tools/bench.py <game> <src.cpp> <opp1[,opp2]> [--n 5]
         [--seeds 111111111,222222222] [--language C++] [--tag name]

Each log is saved in bot-programming/games/<game>/logs/<tag>-*.log.
Prints one line per game: our place, scores, summary; then "WINS x/n".
Handles HTTP 429 by sleeping Retry-After / retry_after_seconds.
Our agent is the one labelled "(me" in the log header.
"""
import argparse
import json
import os
import re
import sys
import time
import urllib.error
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cg  # noqa: E402


def call(method, path, body=None):
    url = cg.API + path
    while True:
        data = json.dumps(body).encode() if body is not None else None
        headers = {"Authorization": "Bearer " + cg.token()}
        if data:
            headers["Content-Type"] = "application/json"
        req = urllib.request.Request(url, data=data, headers=headers, method=method)
        try:
            with urllib.request.urlopen(req) as r:
                return json.loads(r.read().decode())
        except urllib.error.HTTPError as e:
            txt = e.read().decode()
            if e.code == 429:
                wait = e.headers.get("Retry-After")
                try:
                    wait = wait or json.loads(txt).get("retry_after_seconds")
                except ValueError:
                    pass
                wait = int(float(wait or 30)) + 1
                sys.stderr.write("429, sleeping %ss\n" % wait)
                time.sleep(wait)
                continue
            raise SystemExit("HTTP %s %s" % (e.code, txt[:500]))
        except (urllib.error.URLError, OSError) as e:  # DNS / network blips
            sys.stderr.write("network error %s, retry in 15s\n" % e)
            time.sleep(15)


def run_one(game, code, lang, opps, seed, tag):
    body = {"language": lang, "code": code, "opponents": opps}
    if seed:
        body["game_options"] = "seed=%s" % seed
    resp = call("POST", "/puzzles/%s/test-games?wait=30" % game, body)
    while resp.get("status") not in ("done", "failed", None):
        tid = resp.get("test_game_id")
        if not tid:
            break
        time.sleep(2)
        resp = call("GET", "/test-games/%s?wait=30" % tid)
    log = resp.get("log") or ""
    d = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "games", game, "logs")
    os.makedirs(d, exist_ok=True)
    path = os.path.join(d, "%s-%s.log" % (tag, time.strftime("%Y%m%d-%H%M%S")))
    with open(path, "w") as f:
        f.write(log if log else json.dumps(resp, indent=1))
    return log, path


def parse(log):
    me = None
    for line in log.splitlines()[:12]:
        m = re.match(r"\s+(\d+) .*\(me", line)
        if m:
            me = int(m.group(1))
    rk = re.findall(r"ranking \(agent indexes, winner first\): ([\d ]+)", log)
    sc = re.findall(r"scores: (.*)", log)
    summ = re.findall(r"summary: (.*)", log)
    order = [int(x) for x in rk[-1].split()] if rk else []
    place = order.index(me) + 1 if (me is not None and me in order) else None
    return me, place, sc[-1] if sc else "?", summ[-1][:100] if summ else ""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("game")
    ap.add_argument("src")
    ap.add_argument("opps")
    ap.add_argument("--n", type=int, default=5)
    ap.add_argument("--seeds", default="")
    ap.add_argument("--language", default="C++")
    ap.add_argument("--tag", default="bench")
    a = ap.parse_args()
    code = open(a.src, encoding="utf-8").read()
    opps = [int(x) if x.lstrip("-").isdigit() else x for x in a.opps.split(",")]
    seeds = [s for s in a.seeds.split(",") if s] or [None] * a.n
    wins = 0
    for i, seed in enumerate(seeds):
        log, path = run_one(a.game, code, a.language, opps, seed, a.tag)
        me, place, sc, summ = parse(log)
        win = place == 1
        tie = False
        m = re.findall(r"(\d+)=(-?[\d.]+)", sc)
        if len(m) >= 2 and len({v for _, v in m}) == 1:
            tie = True
        wins += 1 if win and not tie else 0
        print("game %d seed=%s me=%s place=%s %s scores[%s] %s -> %s" % (
            i + 1, seed, me, place, "TIE" if tie else ("WIN" if win else "LOSS"),
            sc, summ, os.path.basename(path)), flush=True)
    print("WINS %d/%d" % (wins, len(seeds)))


if __name__ == "__main__":
    main()
