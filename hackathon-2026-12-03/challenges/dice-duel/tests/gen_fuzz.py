#!/usr/bin/env python3
"""Dice Duel fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is 1..3 independent snapshots (the bot keeps no memory): 2..16
dice on distinct cells, both owners, orientations reached by random rolls of
the referee's start die, from crowded to sparse boards.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


SIZE = 8
START = (4, 1, 5, 6, 2, 3)
ROLL = ((1, 2, 3, 0, 4, 5), (3, 0, 1, 2, 4, 5), (4, 1, 5, 3, 2, 0), (5, 1, 4, 3, 0, 2))
MAX_TURNS = 3


def faces(rng):
    state = START
    for _ in range(rng.randint(0, 12)):
        roll = rng.choice(ROLL)
        state = tuple(state[i] for i in roll)
    return state


def snapshot(rng):
    count = rng.randint(2, 16)
    cells = rng.sample([(x, y) for x in range(SIZE) for y in range(SIZE)], count)
    owners = [0, 1] + [rng.randint(0, 1) for _ in range(count - 2)]
    lines = [str(count)]
    for (x, y), owner in zip(cells, owners):
        lines.append("%d %s%d %s" % (owner, chr(65 + x), y + 1, " ".join(map(str, faces(rng)))))
    return lines


def make_case(rng):
    out = []
    for _ in range(rng.randint(1, MAX_TURNS)):
        out += snapshot(rng)
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
