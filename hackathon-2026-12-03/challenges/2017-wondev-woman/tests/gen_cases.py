#!/usr/bin/env python3
"""Génère 10 cas déterministes + 200 cas de fuzz pour Wondev Woman.

Format réel du stub (vérifié sur des test games CodinGame) :
    size unitsPerPlayer
    size lignes de grille ('.' = trou, '0'..'3')
    unitsPerPlayer lignes "x y" (nos unités)
    unitsPerPlayer lignes "x y" (unités adverses, -1 -1 si invisible)
    legalActions
    legalActions lignes "atype index dir1 dir2"

Les cas aléatoires sont reproductibles (seed fixe) afin que les 4 langages
soient comparés sur exactement les mêmes données.
"""
from __future__ import annotations

import random
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
DIRS = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"]
DELTA = {
    "N": (0, -1), "NE": (1, -1), "E": (1, 0), "SE": (1, 1),
    "S": (0, 1), "SW": (-1, 1), "W": (-1, 0), "NW": (-1, -1),
}


def write(name: str, grid: list[str], ours: list[tuple[int, int]],
          foes: list[tuple[int, int]],
          actions: list[tuple[str, str, str]]) -> None:
    lines = [str(len(grid)), str(len(ours))]
    lines += grid
    lines += [f"{x} {y}" for x, y in ours]
    lines += [f"{x} {y}" for x, y in foes]
    lines += [str(len(actions))]
    lines += [f"{atype} {idx} {d1} {d2}" for atype, idx, d1, d2 in actions]
    (HERE / name).write_text("\n".join(lines) + "\n")


FLAT = ["00000", "00000", "00000", "00000", "00000"]

# 01 — une action mène sur le niveau 3 : point immédiat.
write("01_win.in", ["00000", "00200", "00300", "00000", "00000"],
      [(2, 1)], [(0, 0)], [("MOVE&BUILD", "0", "S", "N"), ("MOVE&BUILD", "0", "N", "S")])
# 02 — deux montées : choisir le tremplin de hauteur 2.
write("02_climb.in", ["00000", "00000", "00120", "00000", "00000"],
      [(2, 2)], [(4, 4)], [("MOVE&BUILD", "0", "E", "W"), ("MOVE&BUILD", "0", "W", "E")])
# 03 — déni : détruire une case de niveau 3 voisine de l'adversaire.
write("03_deny.in", ["00000", "00000", "00320", "00000", "00000"],
      [(0, 2)], [(3, 2)], [("MOVE&BUILD", "0", "E", "E"), ("MOVE&BUILD", "0", "NE", "E")])
# 04 — même arrivée, deux constructions : élever la case 2.
write("04_build.in", ["00000", "00002", "00100", "00000", "00000"],
      [(2, 2)], [(0, 0)], [("MOVE&BUILD", "0", "E", "NE"), ("MOVE&BUILD", "0", "E", "NW")])
# 05 — adversaire invisible : pas de déni, simple montée.
write("05_hidden.in", ["00000", "00000", "00100", "00000", "00000"],
      [(1, 2)], [(-1, -1)], [("MOVE&BUILD", "0", "E", "W"), ("MOVE&BUILD", "0", "W", "E")])
# 06 — grille avec trous : éviter le '.'.
write("06_hole.in", [".....", ".0.0.", ".010.", ".000.", "....."],
      [(2, 2)], [(0, 0)], [("MOVE&BUILD", "0", "N", "S"), ("MOVE&BUILD", "0", "E", "W")])
# 07 — 2 unités par joueur (protocole réel).
write("07_two_units.in", ["00000", "00100", "00000", "00000", "00000"],
      [(1, 1), (3, 3)], [(4, 0), (0, 4)],
      [("MOVE&BUILD", "0", "E", "E"), ("MOVE&BUILD", "1", "W", "W"),
       ("PUSH&BUILD", "0", "S", "N")])
# 08 — aucune action légale : ACCEPT-DEFEAT.
write("08_no_action.in", FLAT, [(0, 0)], [(4, 4)], [])
# 09 — extension d'une case 2 vers une case 3 lointaine.
write("09_remote_build.in", ["00000", "00000", "00100", "00000", "00030"],
      [(1, 2)], [(3, 1)], [("MOVE&BUILD", "0", "E", "E"), ("MOVE&BUILD", "0", "S", "W")])
# 10 — toutes les directions, départage sur la première action.
write("10_tie.in", ["00000", "00000", "00000", "00000", "00000"],
      [(2, 2)], [(2, 0)],
      [("MOVE&BUILD", "0", d, "N") for d in DIRS])


def legal_actions(grid: list[str], ours: list[tuple[int, int]], rng: random.Random) -> list[tuple[str, str, str, str]]:
    """Actions plausibles : déplacement vers une case jouable, puis construction."""
    size = len(grid)
    out: list[tuple[str, str, str, str]] = []
    for idx, (ux, uy) in enumerate(ours):
        for d1 in DIRS:
            dx, dy = DELTA[d1]
            mx, my = ux + dx, uy + dy
            if not (0 <= mx < size and 0 <= my < size) or grid[my][mx] == ".":
                continue
            current = int(grid[uy][ux]) if grid[uy][ux] != "." else 0
            target = int(grid[my][mx])
            if target > current + 1 or target >= 4:
                continue
            for d2 in DIRS:
                bx, by = DELTA[d2]
                cx, cy = mx + bx, my + by
                if 0 <= cx < size and 0 <= cy < size and grid[cy][cx] != ".":
                    out.append(("MOVE&BUILD" if rng.random() > 0.15 else "PUSH&BUILD",
                                str(idx), d1, d2))
    rng.shuffle(out)
    return out[: rng.randint(1, 30)]


FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

for seed in range(200):
    rng = random.Random(10_000 + seed)
    size = rng.choice([4, 5, 6, 7])
    grid = []
    for _ in range(size):
        grid.append("".join(
            "." if rng.random() < 0.08 else str(rng.randint(0, 3))
            for _ in range(size)
        ))
    playable = [(x, y) for y in range(size) for x in range(size) if grid[y][x] != "."]
    if len(playable) < 2:
        grid[0] = "0" * size
        playable = [(x, 0) for x in range(size)]
    upp = rng.choice([1, 2])
    ours = [rng.choice(playable) for _ in range(upp)]
    foes = [(-1, -1) if rng.random() < 0.2 else rng.choice(playable) for _ in range(upp)]
    acts = legal_actions(grid, ours, rng)
    write(f"fuzz/case_{seed:03d}.in", grid, ours, foes, acts)

print(f"generated 10 deterministic + 200 fuzz cases in {HERE}")
