#!/usr/bin/env python3
"""Génère des entrées A Code of Ice and Fire déterministes (plusieurs tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(1717)
W = H = 12
DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def connected_grid() -> list[str]:
    grid = [["#" for _ in range(W)] for _ in range(H)]
    # Une ligne de base garantit la connexité des deux QG.
    for x in range(W):
        grid[0][x] = "."
    for y in range(H):
        grid[y][W - 1] = "."
    # Quelques voids aléatoires, puis bruit contrôlé.
    for y in range(H):
        for x in range(W):
            if grid[y][x] == "#" and rng.random() < 0.55:
                grid[y][x] = "."
    grid[0][0] = "O"
    grid[H - 1][W - 1] = "X"
    # Territoires actifs autour des QG.
    for y in range(3):
        for x in range(3):
            if grid[y][x] != "#" and rng.random() < 0.6:
                grid[y][x] = "O"
    for y in range(H - 3, H):
        for x in range(W - 3, W):
            if grid[y][x] != "#" and rng.random() < 0.6:
                grid[y][x] = "X"
    return ["".join(row) for row in grid]


def turn(grid: list[str], uid: int) -> tuple[str, int]:
    lines = [
        str(rng.randrange(0, 70)),
        str(rng.randrange(-5, 20)),
        str(rng.randrange(0, 70)),
        str(rng.randrange(-5, 20)),
        *grid,
        "2",
        "0 0 0 0",
        "1 0 11 11",
    ]
    free = [(x, y) for y in range(H) for x in range(W) if grid[y][x] != "#"]
    nu = rng.randrange(1, 7)
    units = []
    for _ in range(nu):
        x, y = rng.choice(free)
        owner = rng.randrange(2)
        units.append(f"{owner} {uid} 1 {x} {y}")
        uid += 1
    lines.append(str(len(units)))
    lines.extend(units)
    return "\n".join(lines) + "\n", uid


for k in range(10):
    grid = connected_grid()
    mines = rng.randrange(0, 3)
    header = str(mines) + "\n"
    for _ in range(mines):
        header += f"{rng.randrange(W)} {rng.randrange(H)}\n"
    uid = 1
    body = ""
    for _ in range(3):
        text, uid = turn(grid, uid)
        body += text
    (HERE / f"{k + 1:02d}.in").write_text(header + body)
