#!/usr/bin/env python3
"""Génère 200 cas de fuzz reproductibles pour Elemental Wars (Wood)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(70_000 + seed)
    n = 11
    grid = [["#" if y == 0 or y == n - 1 else "." for x in range(n)]
            for y in range(n)]
    for x in range(n):
        grid[0][x] = grid[n - 1][x] = "#"
    walkable = [(x, y) for y in range(1, n - 1) for x in range(1, n - 1)]
    for x, y in list(walkable):
        if rng.random() < 0.15:
            grid[y][x] = "#"
    walkable = [(x, y) for y in range(1, n - 1) for x in range(1, n - 1)
                if grid[y][x] != "#"]
    rng.shuffle(walkable)
    camps = walkable[:3]
    for ch, (x, y) in zip("WFP", camps):
        grid[y][x] = ch
    places = [p for p in walkable if p not in camps]
    rng.shuffle(places)
    tribes = ["W", "F", "P"]
    rng.shuffle(tribes)
    lines = [rng.choice("WFP"), str(n)]
    lines += ["".join(row) for row in grid]
    for turn in range(rng.randint(1, 3)):
        lines.append("%d %d %d" % (rng.randint(0, 20), rng.randint(0, 20),
                                   rng.randint(0, 20)))
        lines.append("3")
        for i, t in enumerate(tribes):
            x, y = places[i]
            prisoner = 1 if rng.random() < 0.1 else 0
            lines.append("%d %s %d %d %d" % (i, t, x, y, prisoner))
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated 200 fuzz cases in {FUZZ}")
