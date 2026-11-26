#!/usr/bin/env python3
"""Volcanoes fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: an 80-tile graph (N1..N40, S1..S40, 3 neighbours each, random but
symmetric enough to chain), then 1..15 random positions (levels -4..4,
crowded or sparse, eruption-ready level 3 volcanoes) with valid-looking
moves (empty tiles and our volcanoes below level 4, plus unknown names).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


HALF = 40
MAX_TURNS = 15


def graph(rng):
    tiles = 2 * HALF
    neighbours = [[(i + 1) % tiles, (i - 1) % tiles, rng.randrange(tiles)] for i in range(tiles)]
    return ["N%d" % (i + 1) for i in range(HALF)] + ["S%d" % (i + 1) for i in range(HALF)], neighbours


def make_case(rng):
    names, neighbours = graph(rng)
    out = [str(len(names))] + ["%s %d %d %d" % ((n,) + tuple(nb)) for n, nb in zip(names, neighbours)]
    for _ in range(rng.randint(1, MAX_TURNS)):
        weights = rng.choice(((8, 1, 1), (2, 1, 1), (1, 2, 2)))
        levels = []
        for _ in names:
            kind = rng.choices((0, 1, -1), weights)[0]
            levels.append(kind * rng.randint(1, 4))
        moves = [n for n, v in zip(names, levels) if 0 <= v < 4]
        moves = rng.sample(moves, min(len(moves), rng.randint(1, 30))) + (["X99"] if rng.random() < 0.1 else [])
        out.append(" ".join(map(str, levels)))
        out.append(" ".join(moves))
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
