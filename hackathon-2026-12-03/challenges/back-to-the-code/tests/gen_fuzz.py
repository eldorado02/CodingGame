#!/usr/bin/env python3
"""Back to the Code fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case plays 1..30 rounds of 1..3 random walkers on the 35x20 grid: every
player claims the cell it stands on, some cases start from a half-owned grid,
some opponents drop out (-1 -1 0). The bot's answers are not fed back (it
keeps only its rectangle plan), so any walk is a valid stream.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


WIDTH, HEIGHT = 35, 20
MAX_ROUNDS = 30
STEPS = ((0, 1), (0, -1), (1, 0), (-1, 0), (0, 0))


def walk(rng, x, y):
    dx, dy = rng.choice(STEPS)
    return min(WIDTH - 1, max(0, x + dx)), min(HEIGHT - 1, max(0, y + dy))


def make_case(rng):
    opponents = rng.randint(1, 3)
    grid = [["."] * WIDTH for _ in range(HEIGHT)]
    owned_rate = rng.choice((0.0, 0.1, 0.5))
    for y in range(HEIGHT):
        for x in range(WIDTH):
            if rng.random() < owned_rate:
                grid[y][x] = str(rng.randint(0, opponents))
    players = [(rng.randrange(WIDTH), rng.randrange(HEIGHT)) for _ in range(opponents + 1)]
    alive = [True] * (opponents + 1)
    out = [str(opponents)]
    for rnd in range(1, rng.randint(1, MAX_ROUNDS) + 1):
        for k, (x, y) in enumerate(players):
            if alive[k]:
                grid[y][x] = str(k)
        out.append(str(rnd))
        for k, (x, y) in enumerate(players):
            out.append("%d %d %d" % (x, y, 1) if alive[k] else "-1 -1 0")
        out += ["".join(row) for row in grid]
        players = [walk(rng, x, y) for x, y in players]
        if opponents > 1 and rng.random() < 0.05:
            alive[rng.randint(1, opponents)] = False
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
