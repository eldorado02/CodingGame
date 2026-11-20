#!/usr/bin/env python3
"""Génère des entrées Xmas Rush déterministes (plusieurs tours par fichier)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(71)
CH = "URDL"


def turn(tt: int) -> str:
    lines = [str(tt)]
    for _ in range(7):
        lines.append(" ".join("".join(rng.choice("01") for _ in range(4)) for _ in range(7)))
    x, y = rng.randrange(7), rng.randrange(7)
    tx, ty = rng.randrange(7), rng.randrange(7)
    pat = lambda: "".join(rng.choice("01") for _ in range(4))
    lines.append(f"1 {x} {y} {pat()}")
    lines.append(f"1 {tx} {ty} {pat()}")
    ix, iy = rng.randrange(7), rng.randrange(7)
    lines.append("2")
    lines.append(f"A {ix} {iy} 0")
    lines.append(f"B {iy} {ix} 1")
    lines.append("1")
    lines.append("A 0")
    return "\n".join(lines) + "\n"


for k in range(10):
    (HERE / f"{k + 1:02d}.in").write_text("".join(turn(t) for t in range(4)))
