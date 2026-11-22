#!/usr/bin/env python3
"""Génère 200 cas de fuzz reproductibles pour Elemental Wars (Wood 2+)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(70_000 + seed)
    n = rng.choice((11, 13))
    grid = [["#" if y == 0 or y == n - 1 else "." for x in range(n)]
            for y in range(n)]
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
    per_tribe = rng.choice((2, 3, 4))
    count = 3 * per_tribe
    lines = [rng.choice("WFP"), str(n)]
    lines += ["".join(row) for row in grid]
    for turn in range(rng.randint(1, 4)):
        lines.append("%d %d %d" % (rng.randint(0, 20), rng.randint(0, 20),
                                   rng.randint(0, 20)))
        lines.append(str(count))
        for tribe_idx, tribe in enumerate("WFP"):
            for i in range(per_tribe):
                x, y = rng.choice(walkable)
                prisoner = 1 if (turn > 1 and rng.random() < 0.15) else 0
                lines.append("%d %s %d %d %d" % (i, tribe, x, y, prisoner))
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated 200 fuzz cases in {FUZZ}")
