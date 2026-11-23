#!/usr/bin/env python3
"""Tower Dereference fuzz: the bot only keeps the map, so a case is the map
plus 1..3 independent turns. Maps: a random canyon walk from the left to
the right border (with optional side branches). Turns: random money, random
towers of both players (all four types, random upgrade levels, sometimes
printed with a decimal comma), random attacker lines (ignored by the bot).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
SIZE = 17
TYPES = ("GUNTOWER", "GLUETOWER", "FIRETOWER", "HEALTOWER")
TABLES = {
    "GUNTOWER": ((5, 8, 15, 30), (3, 4, 5, 6), (5, 4, 3, 2)),
    "GLUETOWER": ((8, 15, 25, 40), (3, 4, 5, 6), (4, 3, 2, 1)),
    "FIRETOWER": ((2, 3, 5, 7), (1.5, 2, 2.3, 2.5), (8, 7, 6, 5)),
    "HEALTOWER": ((5, 8, 15, 30), (3, 4, 5, 6), (5, 4, 3, 2)),
}


def random_map(rng):
    grid = [["#"] * SIZE for _ in range(SIZE)]
    x, y = 0, rng.randrange(3, SIZE - 3)
    while x < SIZE:
        grid[y][x] = "."
        if rng.random() < 0.6 or x == 0:
            x += 1
        else:
            y = max(1, min(SIZE - 2, y + rng.choice((-1, 1))))
    for _ in range(rng.randint(0, 3)):  # dead-end branches
        bx, by = rng.randrange(SIZE), rng.randrange(1, SIZE - 1)
        for _ in range(rng.randint(1, 4)):
            grid[by][bx] = "."
            by = max(1, min(SIZE - 2, by + rng.choice((-1, 1))))
    return ["".join(row) for row in grid]


def number(rng, value):
    text = "%g" % value
    return text.replace(".", ",") if rng.random() < 0.2 else text


def random_turn(rng, me, grid):
    plateau = [(x, y) for y in range(SIZE) for x in range(SIZE) if grid[y][x] == "#"]
    lines = ["%d %d" % (rng.randint(0, 600), rng.randint(1, 20)),
             "%d %d" % (rng.randint(0, 600), rng.randint(1, 20))]
    cells = rng.sample(plateau, rng.randint(0, 14))
    lines.append(str(len(cells)))
    for tower_id, (x, y) in enumerate(cells):
        kind = rng.choice(TYPES)
        owner = me if rng.random() < 0.6 else 1 - me
        dmg, rng_, rel = (rng.choice(t) for t in TABLES[kind])
        lines.append("%s %d %d %d %d %s %s %s %d" % (kind, tower_id, owner, x, y, number(rng, dmg),
                                                    number(rng, rng_), number(rng, rel), rng.randint(0, 4)))
    attackers = rng.randint(0, 5)
    lines.append(str(attackers))
    for i in range(attackers):
        lines.append("%d %d %.1f %.1f 5 8 10 10 0 25" % (i, rng.randint(0, 1), rng.uniform(0, 16), rng.uniform(0, 16)))
    return lines


def make_case(rng):
    me = rng.choice((0, 1))
    grid = random_map(rng)
    lines = [str(me), "%d %d" % (SIZE, SIZE)] + grid
    for _ in range(rng.randint(1, 3)):
        lines += random_turn(rng, me, grid)
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
