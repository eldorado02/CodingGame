#!/usr/bin/env python3
"""Génère des entrées Code a la Mode déterministes (plusieurs tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(31)
KITCHEN = [
    "###########",
    "#D#......W#",
    "###.#####.#",
    "#B#.....#.#",
    "###.#####.#",
    "#I#.....#.#",
    "###########",
]
ITEMS = ["NONE", "BLUEBERRIES", "ICE_CREAM", "DISH",
         "DISH-BLUEBERRIES", "DISH-ICE_CREAM",
         "DISH-BLUEBERRIES-ICE_CREAM"]
TABLES = [(x, y) for y, row in enumerate(KITCHEN) for x, c in enumerate(row) if c == '#']


def turn() -> str:
    lines = [str(rng.randrange(200))]
    lines.append(f"{rng.randrange(11)} {rng.randrange(7)} {rng.choice(ITEMS)}")
    lines.append(f"{rng.randrange(11)} {rng.randrange(7)} {rng.choice(ITEMS)}")
    n = rng.randrange(5)
    lines.append(str(n))
    for tx, ty in rng.sample(TABLES, n):
        lines.append(f"{tx} {ty} {rng.choice(ITEMS)}")
    lines.append("NONE 0")
    nc = rng.randrange(4)
    lines.append(str(nc))
    for _ in range(nc):
        lines.append(f"DISH-ICE_CREAM-BLUEBERRIES {rng.randrange(200, 500)}")
    return "\n".join(lines) + "\n"


for k in range(10):
    header = "2\nDISH-ICE_CREAM-BLUEBERRIES 450\nDISH-BLUEBERRIES-ICE_CREAM 400\n"
    header += "\n".join(KITCHEN) + "\n"
    (HERE / f"{k + 1:02d}.in").write_text(header + "".join(turn() for _ in range(6)))
