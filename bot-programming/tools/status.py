#!/usr/bin/env python3
"""Campaign status scanner for the 100 bot-programming games.

For each game of games.json it reads:
  - our progress (league, score, rank, boss, in_progress) via /progress;
  - the league of leaderboard rank 1 (= highest league that currently exists).

A game is DONE when our league equals the top existing league (Legend for
official contests, often Wood 1 / Bronze for community games).

Usage: python3 bot-programming/tools/status.py [--only g1,g2] [--out FILE]
Writes bot-programming/status.json and prints one line per game.
The API token is read by cg.py and is never printed.
"""
import argparse
import json
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cg  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))

LEAGUE_ORDER = ["Wood 3", "Wood 2", "Wood 1", "Wood", "Bronze", "Silver",
                "Gold", "Legend"]


def league_index(name):
    return LEAGUE_ORDER.index(name) if name in LEAGUE_ORDER else -1


def safe(method, path):
    try:
        return cg.request(method, path)
    except SystemExit:
        return None


def scan(game):
    row = {"game": game}
    prog = safe("GET", "/puzzles/%s/progress" % game)
    if prog:
        prog.pop("limits", None)
        boss = prog.get("league_boss") or {}
        row.update({
            "league": prog.get("league"),
            "score": prog.get("score"),
            "rank": prog.get("league_rank"),
            "league_total": prog.get("league_total"),
            "in_progress": prog.get("in_progress"),
            "percentage": prog.get("percentage"),
            "submission_id": prog.get("submission_id"),
            "boss_id": boss.get("agent_id") if isinstance(boss, dict) else None,
            "boss_score": boss.get("score") if isinstance(boss, dict) else None,
        })
    lb = safe("GET", "/puzzles/%s/leaderboard?limit=1" % game)
    rows = (lb or {}).get("rows") or []
    row["top_league"] = rows[0].get("league", "NONE") if rows else None
    row["top_score"] = rows[0].get("score") if rows else None
    lg, top = row.get("league"), row["top_league"]
    # Single-league games ("Wood" or no league field): no promotion exists,
    # DONE = we have a scored submission (rank is then the only target).
    if top in ("Wood", "NONE"):
        row["done"] = row.get("score") is not None
    else:
        row["done"] = bool(lg and top and league_index(lg) >= league_index(top))
    if row.get("boss_score") is not None and row.get("score") is not None:
        row["gap"] = round(row["boss_score"] - row["score"], 2)
    return row


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only")
    ap.add_argument("--out", default=os.path.join(ROOT, "status.json"))
    args = ap.parse_args()
    games = [g["pretty_id"] for g in json.load(open(os.path.join(ROOT, "games.json")))]
    if args.only:
        games = args.only.split(",")
    old = {}
    if os.path.exists(args.out):
        old = {r["game"]: r for r in json.load(open(args.out))}
    for g in games:
        r = scan(g)
        old[g] = r
        print("%-45s %-7s top=%-7s done=%-5s score=%s boss=%s gap=%s rank=%s/%s" % (
            g, r.get("league"), r.get("top_league"), r["done"], r.get("score"),
            r.get("boss_score"), r.get("gap"), r.get("rank"), r.get("league_total")))
        time.sleep(1.1)  # 2 requests/game, stay under 120 req/60 s
    with open(args.out, "w") as f:
        json.dump(sorted(old.values(), key=lambda r: r["game"]), f, indent=1)


if __name__ == "__main__":
    main()
