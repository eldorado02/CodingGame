#!/usr/bin/env python3
"""Génère les cas déterministes de CodeBusters."""
from pathlib import Path

HERE = Path(__file__).parent


def write(name: str, init: tuple[int, int, int], turns: list[list[tuple[int, ...]]]) -> None:
    lines = [str(v) for v in init]
    for entities in turns:
        lines.append(str(len(entities)))
        lines += [" ".join(map(str, e)) for e in entities]
    (HERE / name).write_text("\n".join(lines) + "\n")


def b(eid: int, x: int, y: int, team: int, state: int = 0, value: int = -1) -> tuple[int, ...]:
    return (eid, x, y, team, state, value)


def g(eid: int, x: int, y: int) -> tuple[int, ...]:
    return (eid, x, y, -1, 0, 0)


# Team 0, base (0,0). Buster en (3000,3000), fantôme à 1300 sur l'axe X.
write("02_bust_band.in", (2, 8, 0), [[b(0, 3000, 3000, 0), g(10, 4300, 3000)]])
# Fantôme à 500 : trop proche, le buster doit reculer.
write("03_too_close.in", (2, 8, 0), [[b(0, 3000, 3000, 0), g(10, 3500, 3000)]])
# Fantôme à 5000 : le buster approche de 800.
write("04_far.in", (2, 8, 0), [[b(0, 1000, 1000, 0), g(10, 6000, 1000)]])
# Buster porteur loin de la base : il rentre.
write("05_carry.in", (2, 8, 0), [[b(0, 5000, 5000, 0, 1, 10)]])
# Buster porteur à 600 de la base : RELEASE.
write("06_release.in", (2, 8, 0), [[b(0, 400, 400, 0, 1, 10)]])
# Aucun fantôme visible, team 1 (base 16000,9000) : patrouille vers le centre.
write("01_no_ghost.in", (2, 8, 1), [[b(0, 15000, 8500, 1)]])
# Deux busters, deux fantômes : réservation, un fantôme chacun.
write("07_two_busters.in", (2, 8, 0), [[
    b(0, 1000, 1000, 0), b(1, 2000, 1000, 0),
    g(10, 2300, 1000), g(11, 3300, 1000),
]])
# Buster porteur team 1 près de sa base.
write("08_release_team1.in", (2, 8, 1), [[b(0, 15800, 8900, 1, 1, 10)]])
