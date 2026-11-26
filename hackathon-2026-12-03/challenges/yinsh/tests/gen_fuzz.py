#!/usr/bin/env python3
"""Yinsh fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: our id, then 1..12 random snapshots (85-cell board, 11 column
rows) with 1..60 actions mixing placements, aligned and misaligned moves,
removals before / after a move, STEAL, upper-case cells and garbage. The bot
keeps no memory, so snapshots need not follow each other.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


COLUMN_START = (2, 1, 1, 1, 1, 2, 2, 3, 4, 5, 7)
COLUMN_SIZE = (4, 7, 8, 9, 10, 9, 10, 9, 8, 7, 4)
LETTERS = "abcdefghijk"
AXES = ((0, 1), (1, 0), (1, 1))
MAX_TURNS = 12


def cell(rng):
    col = rng.randrange(11)
    return col, COLUMN_START[col] + rng.randrange(COLUMN_SIZE[col])


def name(c):
    """Cell name; an off-board column becomes 'z' (still a parsable token)."""
    letter = LETTERS[c[0]] if 0 <= c[0] < len(LETTERS) else "z"
    return "%s%d" % (letter, abs(c[1]))


def aligned(rng, start):
    dc, dn = rng.choice(AXES)
    k = rng.choice((-5, -4, -3, -2, -1, 1, 2, 3, 4, 5))
    return start[0] + k * dc, start[1] + k * dn


def board(rng):
    weights = rng.choice(((8, 1, 1, 1, 1), (3, 1, 3, 1, 3), (20, 1, 0, 1, 0)))
    rows = []
    for col in range(11):
        rows.append("".join(rng.choices(".RSrs", weights)[0] for _ in range(COLUMN_SIZE[col])))
    return rows


def action(rng):
    kind = rng.randrange(7)
    a = cell(rng)
    if kind == 0:
        return name(a)
    if kind == 1:
        return "%s-%s" % (name(a), name(aligned(rng, a)))
    if kind == 2:
        return "%s-%s" % (name(a), name(cell(rng)))
    removal = "x%s-%sx%s" % (name(a), name(aligned(rng, a)), name(cell(rng)))
    if kind == 3:
        return "%s-%s;%s" % (name(a), name(aligned(rng, a)), removal)
    if kind == 4:
        return "%s;%s-%s" % (removal, name(a), name(aligned(rng, a)))
    if kind == 5:
        return rng.choice(("STEAL", "steal", name(a).upper(), "z9", "a", "x-x", "a1-", "-b2", "xa2-a5", "a20"))
    return "%s;%s" % (removal, removal)


def make_case(rng):
    out = [str(rng.randint(0, 1))]
    for _ in range(rng.randint(1, MAX_TURNS)):
        out.append("11")
        out += board(rng)
        actions = [action(rng) for _ in range(rng.randint(1, 60))]
        out.append(str(len(actions)))
        out += actions
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
