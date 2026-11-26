#!/usr/bin/env python3
"""Code Keeper fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is a stream of 1..40 turn snapshots. The bot only reads its input
(no opponent), so snapshots need not follow from the previous answer; they
keep a fixed random maze so the memory sees consistent walls and items.
Covers: low health, zero charges, monsters on every side, entities out of
the field of view or out of the board, several entities on one cell.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


WIDTH, HEIGHT = 16, 12
FOV = 3
MAX_TURNS = 40
WALL_RATE = 0.25
ITEM_TYPES = (0, 2, 3, 4, 5, 6)
MONSTERS = {7: 1, 8: 6, 9: 14, 10: 8, 11: 10}   # type -> initial health
ITEM_VALUE = {0: 10000, 2: 100, 3: 10, 4: 1, 5: 2, 6: 3}


def random_maze(rng):
    """Cell -> (etype, value) for walls and items, fixed for the whole case."""
    maze = {}
    for y in range(HEIGHT):
        for x in range(WIDTH):
            r = rng.random()
            if r < WALL_RATE:
                maze[(x, y)] = (1, -1)
            elif r < WALL_RATE + 0.06:
                t = rng.choice(ITEM_TYPES)
                maze[(x, y)] = (t, ITEM_VALUE[t])
    return maze


def snapshot(rng, maze):
    hx, hy = rng.randrange(WIDTH), rng.randrange(HEIGHT)
    maze.pop((hx, hy), None)
    head = [hx, hy, rng.randint(1, 20), rng.randint(0, 10500)] + [rng.randint(0, 5) for _ in range(3)]
    ents = [(x, y, t, v) for (x, y), (t, v) in sorted(maze.items())
            if max(abs(x - hx), abs(y - hy)) <= FOV]
    for _ in range(rng.randint(0, 6)):
        t = rng.choice(list(MONSTERS))
        x = hx + rng.randint(-FOV, FOV)
        y = hy + rng.randint(-FOV, FOV)
        if (x, y) != (hx, hy):
            ents.append((x, y, t, rng.randint(1, MONSTERS[t])))
    rng.shuffle(ents)
    lines = [" ".join(map(str, head)), str(len(ents))]
    lines += ["%d %d %d %d" % e for e in ents]
    return lines


def make_case(rng):
    maze = random_maze(rng)
    out = []
    for _ in range(rng.randint(1, MAX_TURNS)):
        out += snapshot(rng, maze)
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
