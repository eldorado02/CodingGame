#!/usr/bin/env python3
"""Génère les cas déterministes de Fantastic Bits (Wood 2)."""
from pathlib import Path

HERE = Path(__file__).parent


def wizard(eid: int, x: int, y: int, state: int = 0) -> str:
    return f"{eid} WIZARD {x} {y} 0 0 {state}"


def opponent(eid: int, x: int, y: int) -> str:
    return f"{eid} OPPONENT_WIZARD {x} {y} 0 0 0"


def snaffle(eid: int, x: int, y: int, vx: int = 0, vy: int = 0, state: int = 0) -> str:
    return f"{eid} SNAFFLE {x} {y} {vx} {vy} {state}"


def write(name: str, team: int, entities: list[str]) -> None:
    lines = [str(team), "0 0", "0 0", str(len(entities))] + entities
    (HERE / name).write_text("\n".join(lines) + "\n")


# Grab : THROW vers le but adverse (16000,3750) pour team 0.
write("01_grab.in", 0, [wizard(0, 8000, 3000, 1), wizard(1, 1000, 3000), snaffle(2, 9000, 4000)])
# Poursuite lointaine : thrust 150, visée anticipée (x+3vx).
write("02_chase.in", 0, [wizard(0, 1000, 1000), wizard(1, 500, 3000), snaffle(2, 9000, 4000, 20, 10)])
# Snaffle proche : thrust 60.
write("03_close.in", 0, [wizard(0, 8000, 4000), wizard(1, 500, 3000), snaffle(2, 8300, 4000)])
# Défense : le défenseur (id 1) intercepte le Snaffle dans notre camp.
write("04_defense.in", 0, [wizard(0, 8000, 3000), wizard(1, 2000, 3000), snaffle(2, 3000, 3000)])
# Team 1 : THROW vers (0,3750).
write("05_team1.in", 1, [wizard(0, 8000, 3000, 1), wizard(1, 15000, 3000), snaffle(2, 9000, 4000)])
# Les deux sorciers ont grab : deux THROW.
write("06_two_grab.in", 0, [wizard(0, 8000, 3000, 1), wizard(1, 7000, 3000, 1), snaffle(2, 9000, 4000)])
# Vitesse négative : anticipation bornée au terrain.
write("07_negative_v.in", 1, [wizard(0, 8000, 3000), wizard(1, 15000, 3000), snaffle(2, 100, 100, -100, -100)])
