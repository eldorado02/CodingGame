#!/usr/bin/env python3
"""Chain Reaction fuzz: streams of 1..5 snapshot turns from random playouts.

First line = colour, then per turn: last move ("null" or a cell) + 6 rows.
Positions are non-terminal and the side to move is our colour.
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


def snapshot(rng, me):
    while True:
        s = bot.State([0] * 36, [0] * 36, 1)
        plies = rng.randint(0, 70)
        ok = True
        for _ in range(plies):
            mv = bot.gen_moves(s)
            s = bot.apply_move(s, rng.choice(mv))
            if bot.winner(s):
                ok = False
                break
        if ok and s.side == me and bot.gen_moves(s):
            return s


def render(s):
    rows = []
    for y in range(6):
        row = ""
        for x in range(6):
            i = y * 6 + x
            row += ".." if s.own[i] == 0 else ("r" if s.own[i] == 1 else "b") + str(s.orb[i])
        rows.append(row)
    return rows


def make_case(rng):
    me = 1 if rng.random() < 0.5 else 2
    lines = ["r" if me == 1 else "b"]
    for t in range(rng.randint(1, 5)):
        s = snapshot(rng, me)
        lines.append("null" if sum(s.orb) == 0 else bot.name_of(rng.randrange(36)))
        lines += render(s)
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
