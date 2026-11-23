#!/usr/bin/env python3
"""Bandas fuzz: the bot is stateless (full grid every turn), so a case is the
header plus 1..3 snapshots of one random game. Pawns are placed like the
referee does (8 pawns of player 0 in the centre 4x4, rest random), then both
sides play seeded random directions; holes appear through border trimming.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
SIDE = 8
CENTRE = range(2, 6)
CENTRE_PAWNS = 8
MAX_GAP = 12

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
CODE_CHAR = {bot.EMPTY: "-", bot.PAWN0: "0", bot.PAWN1: "1", bot.HOLE: "x"}


def initial_grid(rng):
    cells = [None] * (SIDE * SIDE)
    centre = [y * SIDE + x for y in CENTRE for x in CENTRE]
    for i in rng.sample(centre, CENTRE_PAWNS):
        cells[i] = bot.PAWN0
    for i in centre:
        cells[i] = cells[i] or bot.PAWN1
    free = [i for i in range(SIDE * SIDE) if cells[i] is None]
    for i in rng.sample(free, SIDE * SIDE // 2 - CENTRE_PAWNS):
        cells[i] = bot.PAWN0
    return bot.Grid(SIDE, SIDE, [c or bot.PAWN1 for c in cells])


def snapshot(grid):
    return [" ".join(CODE_CHAR[grid.at(y, x)] for x in range(SIDE)) for y in range(SIDE)]


def make_case(rng):
    me = rng.choice((0, 1))
    grid = initial_grid(rng)
    lines = [str(me), str(SIDE), str(SIDE)]
    player = rng.choice((0, 1))
    for _ in range(rng.randint(1, 3)):
        for _ in range(rng.randint(0, MAX_GAP)):
            trial = grid.copy()
            trial.play(player, rng.randrange(4))
            if trial.winner() >= 0:
                break
            grid = trial
            player = 1 - player
        lines += snapshot(grid)
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
