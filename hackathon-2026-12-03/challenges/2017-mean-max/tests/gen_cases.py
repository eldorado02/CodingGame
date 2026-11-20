#!/usr/bin/env python3
"""Génère les cas déterministes de Mean Max."""
from pathlib import Path

HERE = Path(__file__).parent


def reaper(eid: int, player: int, x: int, y: int, vx: int, vy: int) -> str:
    return f"{eid} 0 {player} 0.5 400 {x} {y} {vx} {vy} -1 -1"


def wreck(eid: int, x: int, y: int, radius: int, water: int) -> str:
    return f"{eid} 4 -1 -1 {radius} {x} {y} 0 0 {water} -1"


def write(name: str, me: tuple[int, int, int, int], entities: list[str]) -> None:
    units = [reaper(0, 0, *me)] + entities + [
        reaper(90, 1, -5000, -5000, 0, 0),
        reaper(91, 2, 5000, 5000, 0, 0),
    ]
    # 3 lignes de scores + 3 lignes de rages (placeholders), puis unitCount.
    lines = ["0"] * 6 + [str(len(units))] + units
    (HERE / name).write_text("\n".join(lines) + "\n")


# 01 — épave proche : accélérer vers elle.
write("01_reach.in", (0, 0, 0, 0), [wreck(10, 3000, 0, 500, 20)])
# 02 — vitesse élevée au centre : freiner pour rester dans l'épave.
write("02_stop.in", (0, 0, 600, 0), [wreck(10, 0, 0, 400, 20)])
# 03 — déjà stable au centre : WAIT.
write("03_stay.in", (0, 0, 0, 0), [wreck(10, 0, 0, 400, 20)])
# 04 — deux épaves à égale distance : privilégier la plus riche.
write("04_choose.in", (0, 0, 0, 0),
      [wreck(10, 1000, 0, 400, 5), wreck(11, 0, 1000, 400, 30)])
# 05 — aucune épave : WAIT.
write("05_none.in", (0, 0, 0, 0), [])
# 06 — deux épaves superposées : rester dans les deux.
write("06_overlap.in", (0, 0, 0, 0),
      [wreck(10, 500, 0, 600, 10), wreck(11, 500, 100, 600, 10)])
