#!/usr/bin/env python3
"""Génère des entrées Ocean of Code déterministes (placement + plusieurs tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20200320)
N = 15
DIRS = ["N", "E", "S", "W"]
ORDERS = [
    "NA",
    "MOVE N",
    "MOVE E|TORPEDO 3 5",
    "SURFACE 4",
    "TORPEDO 7 7",
    "MOVE S",
    "SURFACE 9|MSG coucou",
    "MOVE W|TORPEDO 12 2",
]


def make_grid() -> list[str]:
    g = [["." for _ in range(N)] for _ in range(N)]
    for _ in range(rng.randrange(8, 18)):
        cx, cy = rng.randrange(N), rng.randrange(N)
        for dy in range(3):
            for dx in range(3):
                x, y = cx + dx, cy + dy
                if 0 <= x < N and 0 <= y < N and rng.random() < 0.75:
                    g[y][x] = "x"
    # Garantir quelques cases eau.
    g[7][7] = "."
    g[0][0] = "."
    g[N - 1][N - 1] = "."
    return ["".join(row) for row in g]


for k in range(10):
    grid = make_grid()
    water = [(x, y) for y in range(N) for x in range(N) if grid[y][x] == "."]
    myid = rng.randrange(2)
    lines = [f"15 15 {myid}", *grid]

    my_life = 6
    opp_life = 6
    for t in range(10):
        px, py = rng.choice(water)
        my_life = max(1, my_life - rng.choice([0, 0, 0, 1]))
        delta = rng.choice([0, 0, 1, 1, 2])
        opp_life = max(0, opp_life - delta)
        cooldown = [3, 2, 1, 0, 0, 3, 2, 1, 0, 0][t]
        sonar = rng.choice(["NA", "Y", "N"])
        orders = ORDERS[(t + k) % len(ORDERS)]
        lines.append(f"{px} {py} {my_life} {opp_life} {cooldown} 2 -1 -1")
        lines.append(sonar)
        lines.append(orders)
    (HERE / f"{k + 1:02d}.in").write_text("\n".join(lines) + "\n")
