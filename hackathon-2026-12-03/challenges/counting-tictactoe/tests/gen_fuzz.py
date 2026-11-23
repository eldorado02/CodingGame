#!/usr/bin/env python3
"""Counting TicTacToe fuzz: self-play streams across the match reset.

Our replies come from the Python reference (same CG_NODES as run.sh); the
opponent plays seeded random empty cells. Valid lists are shuffled like the
referee's. Some cases jump to a second match (cleared board) to exercise the
reset detection.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_MY_TURNS = 8

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def turn_lines(rng, cell, last):
    empty = [i for i in range(bot.CELLS) if cell[i] == 0]
    rng.shuffle(empty)
    return ["%d %d" % last, str(len(empty))] + ["%d %d" % (i // bot.N, i % bot.N) for i in empty]


def play_match(rng, lines, me_first, turns, last):
    cell = [0] * bot.CELLS
    if not me_first:
        o = rng.randrange(bot.CELLS)
        cell[o] = bot.THEIRS
        last = (o // bot.N, o % bot.N)
    for _ in range(turns):
        lines += turn_lines(rng, cell, last)
        s = bot.State(cell[:], bot.MINE)
        m = bot.Search().run(s)
        cell[m] = bot.MINE
        empty = [i for i in range(bot.CELLS) if cell[i] == 0]
        o = rng.choice(empty)
        cell[o] = bot.THEIRS
        last = (o // bot.N, o % bot.N)
    return last


def make_case(rng):
    lines = []
    last = play_match(rng, lines, rng.random() < 0.5, rng.randint(1, MAX_MY_TURNS), (-1, -1))
    if rng.random() < 0.4:
        play_match(rng, lines, rng.random() < 0.5, rng.randint(1, 3), last)
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
