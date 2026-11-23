#!/usr/bin/env python3
"""Brandubh fuzz: streams of 1..6 board snapshots.

Each snapshot comes from a seeded random playout (captures applied with the
reference rules) from the opening position, cut at a random ply, with the
side to move as role. Consecutive snapshots of one case are independent
positions (the bots only keep a repetition history, which stays
deterministic). Repeated snapshots are inserted on purpose to exercise the
3-fold repetition filter.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
INIT = "...A......A......D...AADKDAA...D......A......A..."


def snapshot(rng):
    while True:
        s = bot.State([bot.CHAR[c] for c in INIT], bot.SIDE_A)
        plies = rng.randint(0, 40)
        ok = True
        for _ in range(plies):
            mv = bot.gen_moves(s)
            if not mv:
                ok = False
                break
            s = bot.apply_move(s, rng.choice(mv))
            if bot.winner(s) >= 0:
                ok = False
                break
        if ok and bot.gen_moves(s):
            return s


def render(s, last):
    rows = ["".join(".ADK"[v] for v in s.cell[y * 7:y * 7 + 7]) for y in range(7)]
    return [str(s.side), last] + rows


def make_case(rng):
    lines = []
    prev = None
    for t in range(rng.randint(1, 6)):
        s = prev if (prev is not None and rng.random() < 0.3) else snapshot(rng)
        prev = s
        lines += render(s, "NONE" if t == 0 else "a1a2")
    return "\n".join(lines) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
