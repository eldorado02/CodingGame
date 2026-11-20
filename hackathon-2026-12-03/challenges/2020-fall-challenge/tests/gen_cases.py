#!/usr/bin/env python3
"""Génère des entrées Fall Challenge 2020 déterministes (10 cas, 6 tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20201112)

for k in range(10):
    lines = []
    next_id = 1 + k * 100
    for _turn in range(6):
        n = rng.randrange(1, 11)
        lines.append(str(n))
        for _ in range(n):
            price = rng.randrange(6, 24)
            deltas = [-rng.randrange(0, 6) for _ in range(4)]
            lines.append(f"{next_id} BREW {deltas[0]} {deltas[1]} {deltas[2]} {deltas[3]} "
                         f"{price} 0 0 0 0")
            next_id += 1
        inv = [rng.randrange(0, 11) for _ in range(4)]
        score = rng.randrange(0, 60)
        oinv = [rng.randrange(0, 11) for _ in range(4)]
        oscore = rng.randrange(0, 60)
        lines.append(" ".join(map(str, inv + [score])))
        lines.append(" ".join(map(str, oinv + [oscore])))
    (HERE / f"{k + 1:02d}.in").write_text("\n".join(lines) + "\n")
