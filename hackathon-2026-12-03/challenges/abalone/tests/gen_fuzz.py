#!/usr/bin/env python3
"""Abalone fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: our id (1 or 2), then 1..6 random snapshots (the bot keeps no
memory): marbles scattered or packed near the edge, 1..80 moves made of 1..3
aligned cells of the board in any direction (pushes, falls off the board,
side-steps, blocked moves the bot must still simulate the same way).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


RADIUS = 4
WIDTHS = (5, 6, 7, 8, 9, 8, 7, 6, 5)
AXES = ((1, 0), (1, -1), (0, -1), (-1, 0), (-1, 1), (0, 1))
MAX_TURNS = 6


def q_min(r):
    return max(-RADIUS, -r - RADIUS)


def to_xy(q, r):
    return q - q_min(r), r + RADIUS


def on_board(q, r):
    return -RADIUS <= r <= RADIUS and q_min(r) <= q <= min(RADIUS, -r + RADIUS)


def move(rng):
    while True:
        r = rng.randint(-RADIUS, RADIUS)
        q = rng.randint(q_min(r), min(RADIUS, -r + RADIUS))
        dq, dr = rng.choice(AXES)
        k = rng.randint(0, 2)
        lq, lr = q + k * dq, r + k * dr
        if on_board(lq, lr):
            return "%d %d %d %d %d" % (to_xy(q, r) + to_xy(lq, lr) + (rng.randrange(6),))


def make_case(rng):
    out = [str(rng.randint(1, 2))]
    for _ in range(rng.randint(1, MAX_TURNS)):
        out.append("%d %d" % (rng.randint(0, 5), rng.randint(0, 5)))
        weights = rng.choice(((6, 1, 1), (2, 1, 1), (1, 2, 2)))
        out += ["".join(rng.choices("012", weights)[0] for _ in range(w)) for w in WIDTHS]
        out.append("-1 -1 -1 -1 -1")
        moves = [move(rng) for _ in range(rng.randint(1, 80))]
        out.append(str(len(moves)))
        out += moves
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
