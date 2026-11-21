#!/usr/bin/env python3
"""Génère 200 cas de fuzz reproductibles pour Ghost in the Cell."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"

FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(40_000 + seed)
    n = rng.randint(4, 12)
    links = []
    for i in range(1, n):
        links.append((rng.randrange(i), i, rng.randint(1, 6)))
    for _ in range(rng.randrange(n // 2)):
        a, b = rng.sample(range(n), 2)
        w = rng.randint(1, 6)
        if not any({x, y} == {a, b} for x, y, _ in links):
            links.append((a, b, w))

    lines = [str(n), str(len(links))]
    lines += [f"{a} {b} {d}" for a, b, d in links]

    # Première usine toujours à nous pour éviter les parties trivialement vides.
    owners = [1] + [rng.choice([0, 0, -1, -1, 1]) for _ in range(n - 1)]
    for turn in range(rng.randint(1, 3)):
        entities = []
        for i in range(n):
            cyborgs = rng.randint(0, 45)
            prod = rng.choice([0, 0, 1, 2, 3, 3, 4])
            block = rng.choice([0, 0, 0, 1, 2, 5])
            entities.append(f"{i} FACTORY {owners[i]} {cyborgs} {prod} {block} 0")
        eid = n
        for _ in range(rng.randint(0, 6)):
            src, dst = rng.randrange(n), rng.randrange(n)
            if src == dst:
                continue
            entities.append(f"{eid} TROOP {rng.choice([1, -1])} {src} {dst} "
                            f"{rng.randint(1, 30)} {rng.randint(1, 8)}")
            eid += 1
        for _ in range(rng.randint(0, 2)):
            entities.append(f"{eid} BOMB {rng.choice([1, -1])} "
                            f"{rng.randrange(n)} {rng.randrange(n)} "
                            f"{rng.randint(1, 6)} 0")
            eid += 1
        lines += [str(len(entities))] + entities
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated 200 fuzz cases in {FUZZ}")
