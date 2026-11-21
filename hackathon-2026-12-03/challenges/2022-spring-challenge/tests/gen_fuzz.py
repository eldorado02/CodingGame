#!/usr/bin/env python3
"""Génère 200 cas de fuzz reproductibles pour Spring Challenge 2022 (Wood 1)."""
from __future__ import annotations

import random
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
W, H = 17630, 9000

FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(30_000 + seed)
    bx, by = rng.choice([(17630, 9000), (0, 0)])
    lines = [f"{bx} {by}", "3"]
    turns = rng.randint(1, 4)
    for _ in range(turns):
        lines.append(f"{rng.randint(1, 3)} 0")
        lines.append("0 0")
        n = rng.randint(3, 12)
        lines.append(str(n))
        for eid in range(n):
            # Toujours 3 héros à nous, comme dans une vraie partie.
            etype = 1 if eid < 3 else rng.choice([0, 0, 0, 2])
            x, y = rng.randint(0, W), rng.randint(0, H)
            hp = rng.randint(1, 60) if etype == 0 else 0
            vx, vy = rng.randint(-600, 600), rng.randint(-600, 600)
            near = rng.randint(0, 1)
            threat = rng.choice([0, 1, 2])
            owner = 0 if etype == 1 else (1 if etype == 2 else -1)
            lines.append(f"{eid} {etype} {x} {y} 0 {owner} {hp} {vx} {vy} {near} {threat}")
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated 200 fuzz cases in {FUZZ}")
