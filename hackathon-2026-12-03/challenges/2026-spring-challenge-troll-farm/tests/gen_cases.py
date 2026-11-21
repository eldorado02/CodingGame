#!/usr/bin/env python3
"""Génère des scénarios Troll Farm (Wood) déterministes (10 cas + fuzz)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
W, H = 16, 8
TYPES = ("PLUM", "LEMON", "APPLE", "BANANA")


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    sx = rng.randrange(1, W // 2 - 1)
    sy = rng.randrange(1, H - 1)
    grid = [["." for _ in range(W)] for _ in range(H)]
    grid[sy][sx] = "0"
    grid[H - 1 - sy][W - 1 - sx] = "1"

    def free_cell():
        while True:
            x, y = rng.randrange(W), rng.randrange(H)
            if grid[y][x] == ".":
                return x, y

    trees = []
    for _ in range(rng.randrange(1, 13)):
        x, y = free_cell()
        trees.append((rng.choice(TYPES), x, y, rng.randrange(1, 5),
                      rng.randrange(1, 21), rng.randrange(0, 4),
                      rng.randrange(0, 10)))

    px, py = free_cell()
    ox, oy = free_cell()
    lines = [f"{W} {H}"] + ["".join(row) for row in grid]

    for _ in range(2):
        lines.append(" ".join(str(rng.randrange(0, 12)) for _ in range(6)))
        lines.append(" ".join(str(rng.randrange(0, 12)) for _ in range(6)))
        lines.append(str(len(trees)))
        for kind, x, y, size, health, fruits, cooldown in trees:
            lines.append(f"{kind} {x} {y} {size} {health} {fruits} {cooldown}")
        lines.append("2")
        carry = rng.randrange(0, 2)
        lines.append(f"0 0 {px} {py} 1 1 1 0 {carry} 0 0 0 0 0")
        lines.append(f"1 1 {ox} {oy} 1 1 1 0 0 0 0 0 0 0")
        px = max(0, min(W - 1, px + rng.choice((-1, 0, 1))))
        py = max(0, min(H - 1, py + rng.choice((-1, 0, 1))))
        ox = max(0, min(W - 1, ox + rng.choice((-1, 0, 1))))
        oy = max(0, min(H - 1, oy + rng.choice((-1, 0, 1))))
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
