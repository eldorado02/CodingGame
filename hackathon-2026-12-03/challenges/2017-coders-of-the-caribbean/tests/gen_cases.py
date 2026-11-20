#!/usr/bin/env python3
"""Génère les cas déterministes de Coders of the Caribbean."""
from pathlib import Path

HERE = Path(__file__).parent


def ship(eid: int, x: int, y: int, mine: bool = True) -> str:
    return f"{eid} SHIP {x} {y} 0 2 50 {1 if mine else 0}"


def barrel(eid: int, x: int, y: int, rum: int = 15) -> str:
    return f"{eid} BARREL {x} {y} {rum} 0 0 0"


def write(name: str, entities: list[str]) -> None:
    (HERE / name).write_text("1\n" + str(len(entities)) + "\n" + "\n".join(entities) + "\n")


write("01_one.in", [ship(0, 5, 5), barrel(1, 8, 5)])
write("02_two.in", [ship(0, 5, 5), barrel(1, 20, 5), barrel(2, 7, 5)])
write("03_none.in", [ship(0, 5, 5)])
# Barils listés avant le navire : le ciblage doit rester correct.
write("04_barrels_first.in", [barrel(1, 20, 5), barrel(2, 7, 5), ship(0, 5, 5)])
write("05_opponent.in", [ship(0, 5, 5), ship(1, 15, 15, False), barrel(2, 6, 6)])
write("06_on_position.in", [ship(0, 5, 5), barrel(1, 5, 5)])
# Égalité de distance : départage vers le premier lu (min stabilizing).
write("07_tie.in", [ship(0, 5, 5), barrel(1, 7, 5), barrel(2, 5, 7)])
