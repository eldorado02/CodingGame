#!/usr/bin/env python3
"""Tulips and Daisies fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: random field size (8..16), the referee costs or random ones, our
flower name, then 1..6 random snapshots (the bot keeps no memory) from empty
to crowded fields with runs of 3 ready to harvest, low or zero gold.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


MAX_TURNS = 6
NAMES = ("TULIPS", "DAISIES")


def snapshot(rng, width, height):
    weights = rng.choice(((10, 1, 1, 2, 2), (2, 2, 1, 5, 5), (6, 3, 1, 1, 1)))
    lines = [str(rng.randint(1, 256)), "%d %d" % (rng.choice((0, 3, rng.randint(0, 200))), rng.randint(0, 200))]
    for _ in range(height):
        lines.append("".join(rng.choices("SGRTD", weights)[0] for _ in range(width)))
    return lines


def make_case(rng):
    width, height = rng.randint(8, 16), rng.randint(8, 16)
    costs = (0, 1, 5, 10) if rng.random() < 0.7 else tuple(rng.randint(0, 12) for _ in range(4))
    mine = rng.randint(0, 1)
    out = ["%d %d" % (width, height), "%d %d %d %d" % costs, "%s %s" % (NAMES[mine], NAMES[1 - mine])]
    for _ in range(rng.randint(1, MAX_TURNS)):
        out += snapshot(rng, width, height)
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
