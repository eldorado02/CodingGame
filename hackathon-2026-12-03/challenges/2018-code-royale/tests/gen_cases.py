#!/usr/bin/env python3
"""Génère les cas déterministes de Code Royale."""
from pathlib import Path

HERE = Path(__file__).parent


def write(name: str, sites_init: list[tuple[int, int, int, int]], gold: int,
          touched: int, sites_state: list[tuple[int, int, int, int, int, int, int]],
          units: list[tuple[int, int, int, int, int]]) -> None:
    lines = [str(len(sites_init))]
    lines += [f"{sid} {x} {y} {r}" for sid, x, y, r in sites_init]
    lines += [f"{gold} {touched}"]
    lines += [f"{sid} 0 0 {st} {ow} {p1} {p2}" for sid, st, ow, p1, p2 in sites_state]
    lines += [str(len(units))]
    lines += [f"{x} {y} {ow} {ut} {hp}" for x, y, ow, ut, hp in units]
    (HERE / name).write_text("\n".join(lines) + "\n")


# Reines : nous (200,200), ennemi (1720,800).
Q = [(200, 200, 0, -1, 100), (1720, 800, 1, -1, 100)]

# 01 — site libre : construire un barracks KNIGHT.
write("01_build.in", [(0, 500, 500, 80)], 100, -1,
      [(0, -1, -1, -1, -1)], Q)

# 02 — barracks KNIGHT prêt : TRAIN, rien à construire.
write("02_train.in", [(0, 500, 500, 80)], 100, 0,
      [(0, 2, 0, 0, 0)], Q)

# 03 — barracks en cours d'entraînement (param1=3) : pas de TRAIN.
write("03_notrain.in", [(0, 500, 500, 80)], 100, 0,
      [(0, 2, 0, 3, 0)], Q)

# 04 — barracks ARCHER prêt : le remplacer par KNIGHT, pas de TRAIN.
write("04_archer.in", [(0, 500, 500, 80)], 100, 0,
      [(0, 2, 0, 0, 1)], Q)

# 05 — barracks ennemi proche : MOVE pour le détruire.
write("05_destroy.in", [(0, 350, 350, 80)], 100, -1,
      [(0, 2, 1, -1, -1)], Q)

# 06 — pas d'or : WAIT + TRAIN vide.
write("06_nosite.in", [(0, 500, 500, 80)], 50, 0,
      [(0, 2, 0, 0, 0)], Q)

# 07 — deux barracks prêts, 160 or : TRAIN sur les deux.
write("07_multi.in", [(0, 500, 500, 80), (1, 700, 700, 80)], 160, -1,
      [(0, 2, 0, 0, 0), (1, 2, 0, 0, 0)], Q)
