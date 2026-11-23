#!/usr/bin/env python3
"""Ataxx fuzz: streams of 1..5 snapshot turns from seeded random playouts.

Boards: random wall layout (mirrored like the referee configurations),
starting pieces in the corners, then random legal moves with passes.
The legal list is the reference generator's, shuffled, in action text.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_PLIES = 50

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
CORNERS_P0 = (0, 48)
CORNERS_P1 = (6, 42)


def random_walls(rng):
    walls = 0
    for _ in range(rng.randint(0, 4)):
        i = rng.randrange(bot.CELLS)
        mirror = bot.CELLS - 1 - i
        if i not in CORNERS_P0 + CORNERS_P1 and i != 24:
            walls |= (1 << i) | (1 << mirror)
    return walls


def snapshot(rng, me):
    """Position where `me` is to move with at least one legal move."""
    while True:
        p0 = sum(1 << c for c in CORNERS_P0)
        p1 = sum(1 << c for c in CORNERS_P1)
        s = bot.State(p0, p1, random_walls(rng))   # player 0 to move
        side = 0
        for _ in range(rng.randint(0, MAX_PLIES)):
            mv = bot.gen_moves(s)
            if not mv:
                s = bot.State(s.theirs, s.mine, s.walls)
            else:
                s = bot.apply_move(s, rng.choice(mv))
            side = 1 - side
            if not s.mine or not s.theirs:
                break
        if side == me and s.mine and s.theirs:
            mv = bot.gen_moves(s)
            if mv:
                return s, mv


def render(s, me):
    rows = []
    for y in range(bot.SIZE):
        row = ""
        for x in range(bot.SIZE):
            bit = 1 << (y * bot.SIZE + x)
            if s.walls & bit:
                row += "#"
            elif s.mine & bit:
                row += str(me)
            elif s.theirs & bit:
                row += str(1 - me)
            else:
                row += "."
        rows.append(row)
    return rows


def make_case(rng):
    me = rng.randrange(2)
    lines = [str(me)]
    for _ in range(rng.randint(1, 5)):
        s, mv = snapshot(rng, me)
        lines += render(s, me)
        lines.append(rng.choice(["null", "pass", "a1"]))
        names = [bot.action_text(m) for m in mv]
        rng.shuffle(names)
        lines.append(str(len(names)))
        lines += names
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
