#!/usr/bin/env python3
"""Oware fuzz: self-play input streams.

Our replies come from the Python reference (same CG_NODES as run.sh); the
opponent plays a seeded random legal move. Inputs are the 12 houses rotated
so that the receiving player's houses come first, like the referee.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_MY_TURNS = 25

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def make_case(rng):
    me_first = rng.random() < 0.5
    s = bot.State([4] * bot.HOUSES, 0, 0, 0)   # point of view of the side to move
    lines = []
    if not me_first:
        mv = bot.legal_moves(s)
        s = bot.apply_move(s, rng.choice(mv))
    for _ in range(rng.randint(1, MAX_MY_TURNS)):
        mv = bot.legal_moves(s)
        if not mv or s.mine > bot.WIN_SCORE or s.theirs > bot.WIN_SCORE:
            break
        lines.append(" ".join(map(str, s.h)))
        s = bot.apply_move(s, bot.Search().run(s))
        opp = bot.legal_moves(s)
        if not opp or s.mine > bot.WIN_SCORE or s.theirs > bot.WIN_SCORE:
            break
        s = bot.apply_move(s, rng.choice(opp))
    if not lines:
        lines = ["4 4 4 4 4 4 4 4 4 4 4 4"]
    return "\n".join(lines) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(CASES):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{CASES} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
