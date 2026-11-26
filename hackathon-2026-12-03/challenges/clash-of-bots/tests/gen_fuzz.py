#!/usr/bin/env python3
"""Clash of Bots fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is 1..30 turns of 0..10 random minimaps (the bot keeps no memory,
so turns need not follow each other). Density varies per case to cover empty
views, crowded explosions and ties between equally weak enemies.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


SIZE = 5
MAX_HEALTH = 10
MAX_ROBOTS = 10
MAX_TURNS = 30


def minimap(rng, density):
    grid = [[0] * SIZE for _ in range(SIZE)]
    for r in range(SIZE):
        for c in range(SIZE):
            if rng.random() < density:
                hp = rng.randint(1, MAX_HEALTH)
                grid[r][c] = hp if rng.random() < 0.4 else -hp
    grid[2][2] = rng.randint(1, MAX_HEALTH)
    return [" ".join(map(str, row)) for row in grid]


def make_case(rng):
    density = rng.choice((0.0, 0.05, 0.15, 0.3, 0.6))
    out = []
    for _ in range(rng.randint(1, MAX_TURNS)):
        k = rng.randint(0, MAX_ROBOTS)
        out.append(str(k))
        for _ in range(k):
            out += minimap(rng, density)
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
