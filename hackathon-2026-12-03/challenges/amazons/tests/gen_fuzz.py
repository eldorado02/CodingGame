#!/usr/bin/env python3
"""Amazons fuzz: streams of 1..4 snapshot turns from seeded random playouts.

Random start (4 amazons each on distinct cells), random legal actions
(move + arrow) for both sides, our colour to play with at least one move.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
N = 8
MAX_PLIES = 30

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
BOARD = bot.Board(N)


def snapshot(rng, me):
    while True:
        cells = [bot.EMPTY] * (N * N)
        spots = rng.sample(range(N * N), 8)
        for k, i in enumerate(spots):
            cells[i] = "w" if k < 4 else "b"
        side = "w"
        for _ in range(rng.randint(0, MAX_PLIES)):
            acts = bot.gen_actions(BOARD, cells, side)
            if not acts:
                break
            src, dst, arrow = rng.choice(acts)
            cells[src], cells[dst], cells[arrow] = bot.EMPTY, side, bot.WALL
            side = "b" if side == "w" else "w"
        if side == me and bot.gen_actions(BOARD, cells, me):
            return cells


def make_case(rng):
    lines = [str(N)]
    for _ in range(rng.randint(1, 4)):
        me = rng.choice("wb")
        cells = snapshot(rng, me)
        lines.append(me)
        lines += ["".join(cells[y * N:(y + 1) * N]) for y in range(N)]
        lines += ["null", str(len(bot.gen_actions(BOARD, cells, me)))]
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
