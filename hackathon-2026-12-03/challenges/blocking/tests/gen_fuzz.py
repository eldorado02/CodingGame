#!/usr/bin/env python3
"""Blocking fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: 10/13/18 random shapes, 2..4 players, board 13..20, then 1..8
snapshots with random rows, played moves and 0..60 valid-looking moves
(the bot reads the list and scores it; it keeps no memory).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


LETTERS = "ABCDEFGHIJKLMNOPQRSTU"
MAX_TURNS = 8


def shape_line(rng, letter):
    cols, rows = rng.randint(1, 3), rng.randint(1, 3)
    cells = "".join(rng.choice("#.") for _ in range(cols * rows))
    return "%s %d %d %s" % (letter, cols, rows, cells.replace(".", "#", 1) if "#" not in cells else cells)


def move(rng, size):
    return "%d %d %s%d%d%d" % (rng.randrange(size), rng.randrange(size), rng.choice(LETTERS),
                               rng.randint(0, 1), rng.randint(0, 3), rng.randint(1, 5))


def make_case(rng):
    count = rng.choice((10, 13, 18))
    letters = sorted(rng.sample(LETTERS, count))
    players, size = rng.randint(2, 4), rng.randint(13, 20)
    out = [str(count)] + [shape_line(rng, l) for l in letters]
    out += [str(players), str(rng.randrange(players)), str(size), "".join(letters)]
    for _ in range(rng.randint(1, MAX_TURNS)):
        out += ["".join(rng.choice("..x0123") for _ in range(size)) for _ in range(size)]
        played = rng.randint(0, 3)
        out.append(str(played))
        out += ["%d %s" % (rng.randrange(players), move(rng, size)) for _ in range(played)]
        moves = rng.randint(0, 60)
        out.append(str(moves))
        out += [move(rng, size) for _ in range(moves)]
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
