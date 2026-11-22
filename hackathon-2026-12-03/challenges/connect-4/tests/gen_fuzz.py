#!/usr/bin/env python3
"""Génère 200 états de jeu Connect 4 reproductibles pour le fuzz différentiel."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

DR = (0, 1, 1, 1)
DC = (1, 0, 1, -1)


def wins(cell, r, c, p):
    for d in range(4):
        cnt = 1
        for s in range(1, 4):
            nr, nc = r + DR[d] * s, c + DC[d] * s
            if not (0 <= nr < 7 and 0 <= nc < 9) or cell[nr * 9 + nc] != p:
                break
            cnt += 1
        for s in range(1, 4):
            nr, nc = r - DR[d] * s, c - DC[d] * s
            if not (0 <= nr < 7 and 0 <= nc < 9) or cell[nr * 9 + nc] != p:
                break
            cnt += 1
        if cnt >= 4:
            return True
    return False


for seed in range(200):
    rng = random.Random(90_000 + seed)
    cell = [0] * 63
    h = [0] * 9
    moves = []
    target = rng.randint(0, 22)
    while len(moves) < target:
        col = rng.randrange(9)
        if h[col] >= 7:
            continue
        r = 7 - 1 - h[col]
        p = (len(moves) % 2) + 1
        cell[r * 9 + col] = p
        if wins(cell, r, col, p):
            cell[r * 9 + col] = 0
            continue
        h[col] += 1
        moves.append(col)
    me = len(moves) % 2
    lines = [f"{me} {1 - me}", str(len(moves))]
    for r in range(7):
        lines.append("".join("." if cell[r * 9 + c] == 0 else str(cell[r * 9 + c] - 1)
                             for c in range(9)))
    valid = [c for c in range(9) if h[c] < 7]
    lines.append(str(len(valid)))
    lines.extend(str(c) for c in valid)
    lines.append(str(moves[-1]) if moves else "-1")
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")
