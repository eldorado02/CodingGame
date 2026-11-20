#!/usr/bin/env python3
"""Génère les cas déterministes d'A*Craft (grille 19x10 + robots)."""
from pathlib import Path

HERE = Path(__file__).parent
W, H = 19, 10


def write(name: str, rows: list[str], robots: list[tuple[int, int, str]]) -> None:
    lines = [r.ljust(W)[:W] for r in rows]
    assert len(lines) == H
    lines += [str(len(robots))]
    lines += [f"{x} {y} {d}" for x, y, d in robots]
    (HERE / name).write_text("\n".join(lines) + "\n")


VOID = "#" * W
EMPTY = "." * W

# 01 — couloir de 5 cases.
row = "###............####"
rows = [VOID] * 4 + [row] + [VOID] * 5
write("01_row.in", rows, [(3, 4, "R")])

# 02 — grille pleine, robot en haut à gauche.
write("02_full.in", [EMPTY] * H, [(0, 0, "R")])

# 03 — flèche préexistante au centre.
r = ".........D........."
write("03_fixed.in", [VOID] * 4 + [r] + [VOID] * 5, [(9, 4, "R")])

# 04 — robot face au vide : flèche initiale obligatoire.
rows = [VOID] * 4 + ["###............####"] + [VOID] * 5
write("04_buggy.in", rows, [(3, 4, "L")])

# 05 — plateau 3x3, trois robots.
rows = [VOID] * 3 + [VOID + "...#############", VOID + "...#############",
                     VOID + "...#############"] + [VOID] * 4
write("05_multi.in", rows, [(1, 3, "R"), (2, 4, "D"), (3, 5, "U")])

# 06 — robot sur le bord, traversée du torus.
rows = [VOID] * 4 + ["..................#"] + [VOID] * 5
write("06_wrap.in", rows, [(18, 4, "R")])
