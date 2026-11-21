#!/usr/bin/env python3
"""Génère des parties Tron Battle LÉGALES (10 cas + fuzz 200).

Deux joueurs, grille 30x20. On simule de vraies parties : chaque coup va sur
une case libre, les tours sont séquentiels (P0 puis P1), et un joueur sans
coup légal est déclaré mort (-1 -1 -1 -1) et sa bande disparaît.
"""
import random
from pathlib import Path

HERE = Path(__file__).parent
W, H = 30, 20
DIRS = ((0, -1), (0, 1), (-1, 0), (1, 0))


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    # positions initiales distinctes
    p0 = (rng.randrange(W), rng.randrange(H))
    p1 = (rng.randrange(W), rng.randrange(H))
    while p1 == p0:
        p1 = (rng.randrange(W), rng.randrange(H))

    head = [p0, p1]
    tail = [p0, p1]
    occupied = [{p0}, {p1}]  # bande de chaque joueur (cases occupées)
    alive = [True, True]
    turns = rng.randrange(15, 80)
    lines = []

    for _ in range(turns):
        lines.append(f"2 0")
        # P0 bouge d'abord, puis P1 (tours séquentiels)
        for p in (0, 1):
            if not alive[p]:
                lines.append("-1 -1 -1 -1")
                continue
            lines.append(f"{tail[p][0]} {tail[p][1]} {head[p][0]} {head[p][1]}")
        for p in (0, 1):
            if not alive[p]:
                continue
            hx, hy = head[p]
            opts = []
            for dx, dy in DIRS:
                nx, ny = hx + dx, hy + dy
                if not (0 <= nx < W and 0 <= ny < H):
                    continue
                if any((nx, ny) in occ for occ in occupied):
                    continue
                opts.append((nx, ny))
            if not opts:
                alive[p] = False
                occupied[p].clear()
                continue
            tail[p] = head[p]
            head[p] = rng.choice(opts)
            occupied[p].add(head[p])

    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
