#!/usr/bin/env python3
"""Génère 200 états Othello reproductibles pour le fuzz différentiel."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

DR = (-1, -1, -1, 0, 0, 1, 1, 1)
DC = (-1, 0, 1, -1, 1, -1, 0, 1)


def legal(board, p):
    out = []
    op = 1 - p
    for r in range(8):
        for c in range(8):
            if board[r][c] != -1:
                continue
            for dx, dy in zip(DR, DC):
                nr, nc = r + dx, c + dy
                cnt = 0
                while 0 <= nr < 8 and 0 <= nc < 8 and board[nr][nc] == op:
                    cnt += 1
                    nr += dx
                    nc += dy
                if cnt > 0 and 0 <= nr < 8 and 0 <= nc < 8 and board[nr][nc] == p:
                    out.append((r, c))
                    break
    return out


def apply(board, p, r, c):
    board[r][c] = p
    op = 1 - p
    for dx, dy in zip(DR, DC):
        nr, nc = r + dx, c + dy
        line = []
        while 0 <= nr < 8 and 0 <= nc < 8 and board[nr][nc] == op:
            line.append((nr, nc))
            nr += dx
            nc += dy
        if line and 0 <= nr < 8 and 0 <= nc < 8 and board[nr][nc] == p:
            for yy, xx in line:
                board[yy][xx] = p


for seed in range(200):
    rng = random.Random(77000 + seed)
    board = [[-1] * 8 for _ in range(8)]
    board[3][3] = 1
    board[4][4] = 1
    board[3][4] = 0
    board[4][3] = 0
    p = 0
    passes = 0
    for _ in range(rng.randint(0, 40)):
        moves = legal(board, p)
        if not moves:
            passes += 1
            if passes >= 2:
                break
            p = 1 - p
            continue
        passes = 0
        r, c = rng.choice(moves)
        apply(board, p, r, c)
        p = 1 - p
    moves = legal(board, p)
    if not moves:
        p = 1 - p
        moves = legal(board, p)
    if not moves:
        continue
    lines = [str(p), "8"]
    for row in board:
        lines.append("".join("." if v == -1 else str(v) for v in row))
    lines.append(str(len(moves)))
    for r, c in moves:
        lines.append(f"{chr(97 + c)}{r + 1}")
    (FUZZ / f"case_{seed:03d}.in").write_text("\n".join(lines) + "\n")

print(f"generated {len(list(FUZZ.glob('case_*.in')))} cases")
