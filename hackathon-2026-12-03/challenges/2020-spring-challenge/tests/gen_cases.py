#!/usr/bin/env python3
"""Génère des entrées Spring Challenge 2020 déterministes (10 cas, 10 tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20200507)
W, H = 37, 17


def make_grid() -> list[str]:
    g = [[' ' for _ in range(W)] for _ in range(H)]
    for _ in range(rng.randrange(20, 45)):
        cx, cy = rng.randrange(W), rng.randrange(H)
        for dy in range(rng.randrange(1, 4)):
            for dx in range(rng.randrange(1, 5)):
                x, y = cx + dx, cy + dy
                if 0 <= x < W and 0 <= y < H and rng.random() < 0.7:
                    g[y][x] = '#'
    # Symétrie verticale.
    for y in range(H):
        for x in range(W // 2):
            g[y][W - 1 - x] = g[y][x]
    # Bords gauche/droite ouverts pour la majorité des cartes.
    for y in range(H):
        g[y][0] = ' '
        g[y][W - 1] = ' '
    return [''.join(row) for row in g]


for k in range(10):
    grid = make_grid()
    floors = [(x, y) for y in range(H) for x in range(W) if grid[y][x] == ' ']
    px, py = rng.choice(floors)
    ox, oy = rng.choice(floors)
    lines = [f"{W} {H}", *grid]
    for t in range(10):
        lines.append(f"{rng.randrange(0, 60)} {rng.randrange(0, 60)}")
        lines.append("2")
        lines.append(f"0 1 {px} {py} NONE 0 0")
        lines.append(f"0 0 {ox} {oy} NONE 0 0")
        pellets = rng.sample(floors, k=min(12, len(floors)))
        lines.append(str(len(pellets)))
        for x, y in pellets:
            lines.append(f"{x} {y} {10 if rng.random() < 0.1 else 1}")
        # Le pac se déplace d'une case (ou reste) pour simuler la partie.
        dx, dy = rng.choice([(0, 0), (0, 1), (0, -1), (1, 0), (-1, 0)])
        nx = (px + dx) % W
        ny = py + dy
        if 0 <= ny < H and grid[ny][nx] == ' ':
            px, py = nx, ny
        ox = (ox + rng.choice([-1, 0, 1])) % W
    (HERE / f"{k + 1:02d}.in").write_text("\n".join(lines) + "\n")
