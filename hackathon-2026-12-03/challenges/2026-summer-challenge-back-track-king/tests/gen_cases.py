#!/usr/bin/env python3
"""Génère des scénarios Back Track King (Wood) déterministes et de fuzz."""
from pathlib import Path
import random

HERE = Path(__file__).parent


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    width = rng.randrange(21, 31)
    height = rng.randrange(14, 21)
    region = [[rng.randrange(0, 20) for _ in range(width)] for _ in range(height)]
    terrain = [[rng.randrange(0, 3) for _ in range(width)] for _ in range(height)]

    town_count = rng.randrange(4, 13)
    spots = rng.sample(range(width * height), town_count)
    towns = []
    for town_id, spot in enumerate(spots):
        towns.append((town_id, spot % width, spot // width))

    desired = [[] for _ in towns]
    for u in range(town_count):
        for v in range(town_count):
            if u != v and rng.random() < 0.18:
                desired[u].append(v)
    if not any(desired):
        desired[0].append(1)

    lines = [f"{rng.randrange(0, 2)} {width} {height}"]
    for y in range(height):
        for x in range(width):
            lines.append(f"{region[y][x]} {terrain[y][x]}")
    lines.append(str(town_count))
    for town_id, x, y in towns:
        wanted = "x" if not desired[town_id] else ",".join(map(str, desired[town_id]))
        lines.append(f"{town_id} {x} {y} {wanted}")

    pairs = [(u, v) for u in range(town_count) for v in range(u + 1, town_count)
             if v in desired[u]]
    for _ in range(2):
        lines.append(f"{rng.randrange(0, 40)} {rng.randrange(0, 40)}")
        for _ in range(width * height):
            owner = rng.choice((-1, -1, 0, 0, 1, 1, 2))
            instability = rng.randrange(0, 12)
            inked = rng.choice(("0", "0", "0", "1", "false", "true"))
            if pairs and rng.random() < 0.25:
                chosen = rng.sample(pairs, min(len(pairs), rng.randrange(1, 3)))
                active = ",".join(f"{a}-{b}" for a, b in chosen)
            else:
                active = "x"
            lines.append(f"{owner} {instability} {inked} {active}")
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
