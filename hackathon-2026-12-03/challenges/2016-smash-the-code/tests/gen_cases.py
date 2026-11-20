#!/usr/bin/env python3
"""Génère des cas déterministes pour Smash The Code."""
from pathlib import Path

HERE = Path(__file__).parent
EMPTY = [".", ".", ".", ".", ".", "."]


def make(name: str, pair: tuple[int, int], own: list[list[str]] | None = None) -> None:
    grid = [row[:] for row in (own or [EMPTY[:] for _ in range(12)])]
    lines = [f"{pair[0]} {pair[1]}" for _ in range(8)]
    lines += ["0"]
    lines += ["".join(row) for row in grid]
    lines += ["0"]
    lines += ["......" for _ in range(12)]
    (HERE / f"{name}.in").write_text("\n".join(lines) + "\n")


def column(col: int, value: str, rows: range) -> list[list[str]]:
    grid = [EMPTY[:] for _ in range(12)]
    for r in rows:
        grid[r][col] = value
    return grid


make("01_empty", (3, 3))
make("02_clear_left", (3, 3), column(0, "3", range(9, 12)))
make("03_clear_middle", (3, 3), column(3, "3", range(9, 12)))

# Crâne adjacent au groupe : il doit partir avec lui.
grid = column(0, "3", range(9, 12))
grid[11][1] = "0"
make("04_skull", (3, 3), grid)

# Petit groupe de 2 + crâne voisin : le crâne doit rester.
grid = column(0, "3", range(10, 12))
grid[11][1] = "0"
make("05_small_group", (3, 3), grid)

# Colonne 0 presque pleine : ne pas choisir un coup qui top out.
grid = column(0, "1", range(0, 11))
grid[11][0] = "1"
make("06_full_column", (4, 4), grid)

print("cas générés :", sorted(p.name for p in HERE.glob("*.in")))
