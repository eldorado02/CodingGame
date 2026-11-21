#!/usr/bin/env python3
"""Génère 200 cas de fuzz reproductibles pour CodeBusters (4 langages)."""
from __future__ import annotations

import random
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
W, H = 16000, 9000


def entity(eid: int, x: int, y: int, kind: int, state: int = 0, value: int = -1) -> str:
    return f"{eid} {x} {y} {kind} {state} {value}"


FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(20_000 + seed)
    team = rng.randint(0, 1)
    n_busters = rng.randint(1, 4)
    n_ghosts = rng.randint(0, 8)
    entities = []
    for i in range(n_busters):
        entities.append(entity(i, rng.randint(0, W), rng.randint(0, H), team,
                               state=rng.choice([0, 0, 0, 1]),
                               value=rng.choice([-1, 10, 11])))
    for i in range(n_busters, 2 * n_busters):
        entities.append(entity(i, rng.randint(0, W), rng.randint(0, H), 1 - team,
                               state=rng.choice([0, 1]), value=rng.choice([-1, 10, 11])))
    for i in range(10, 10 + n_ghosts):
        entities.append(entity(i, rng.randint(0, W), rng.randint(0, H), -1,
                               state=0, value=rng.choice([0, 0, 1, 2])))
    lines = [str(n_busters), str(max(8, n_ghosts)), str(team), str(len(entities))]
    lines += entities
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated 200 fuzz cases in {FUZZ}")
