#!/usr/bin/env python3
"""Vindinium fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: a random map (size 10..24, walls, taverns, mines, 4 spawn digits),
our id, then 1..30 snapshots with 4 heroes inside the board (life 1..100,
gold 0..300) and every mine with a random owner (-1..3). Heroes start on
their spawn so the x/y axis detection is exercised in both orders.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


MAX_TURNS = 30


def make_case(rng):
    size = rng.choice(range(10, 25, 2))
    grid = [[rng.choices(".#TM", (14, 4, 1, 2))[0] for _ in range(size)] for _ in range(size)]
    spawns = rng.sample([(r, c) for r in range(size) for c in range(size)], 4)
    for k, (r, c) in enumerate(spawns):
        grid[r][c] = str(k)
    me = rng.randint(0, 3)
    x_is_column = rng.random() < 0.5
    mines = [(r, c) for r in range(size) for c in range(size) if grid[r][c] == "M"]
    out = [str(size)] + ["".join(row) for row in grid] + [str(me)]
    for turn in range(rng.randint(1, MAX_TURNS)):
        lines = []
        for k in range(4):
            r, c = spawns[k] if turn == 0 else (rng.randrange(size), rng.randrange(size))
            x, y = (c, r) if x_is_column else (r, c)
            lines.append("HERO %d %d %d %d %d" % (k, x, y, rng.randint(1, 100), rng.randint(0, 300)))
        for r, c in mines:
            x, y = (c, r) if x_is_column else (r, c)
            lines.append("MINE %d %d %d -1 -1" % (rng.randint(-1, 3), x, y))
        out.append(str(len(lines)))
        out += lines
    return "\n".join(out) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        rng = random.Random(i)
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(rng))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
