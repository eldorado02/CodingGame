#!/usr/bin/env python3
"""Génère les cas déterministes de Wondev Woman."""
from pathlib import Path

HERE = Path(__file__).parent


def write(name: str, grid: list[str], me: tuple[int, int], opp: tuple[int, int],
          actions: list[tuple[str, str, str]]) -> None:
    lines = ["5", "1"] + grid + [f"{me[0]} {me[1]}", f"{opp[0]} {opp[1]}", str(len(actions))]
    lines += [f"MOVE&BUILD 0 {d1} {d2}" for _, d1, d2 in actions]
    (HERE / name).write_text("\n".join(lines) + "\n")


FLAT = ["00000", "00000", "00000", "00000", "00000"]

# 01 — une action mène sur le niveau 3 : victoire immédiate.
write("01_win.in",
      ["00000", "00200", "00300", "00000", "00000"],
      (2, 1), (0, 0),
      [("S", "S", "N"), ("N", "N", "S")])

# 02 — deux montées : choisir la case de hauteur 2.
write("02_climb.in",
      ["00000", "00000", "00120", "00000", "00000"],
      (2, 2), (4, 4),
      [("E", "E", "W"), ("W", "W", "E")])

# 03 — l'adversaire (3,2) peut gagner sur sa case 3 en (2,2) : déni.
write("03_deny.in",
      ["00000", "00000", "00320", "00000", "00000"],
      (0, 2), (3, 2),
      [("E", "E", "E"), ("NE", "NE", "E")])

# 04 — même arrivée, deux constructions : élever la case 2.
write("04_build.in",
      ["00000", "00002", "00100", "00000", "00000"],
      (2, 2), (0, 0),
      [("E", "E", "NE"), ("E", "E", "NW")])

# 05 — adversaire invisible : pas de déni, simple montée.
write("05_hidden.in",
      ["00000", "00000", "00100", "00000", "00000"],
      (1, 2), (-1, -1),
      [("E", "E", "W"), ("W", "W", "E")])

# 06 — grille avec trous : fuir la case '.'.
write("06_hole.in",
      [".....", ".0.0.", ".010.", ".000.", "....."],
      (2, 2), (0, 0),
      [("N", "N", "S"), ("E", "E", "W")])
