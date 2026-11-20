#!/usr/bin/env python3
"""Génère les cas déterministes de Ghost in the Cell."""
from pathlib import Path

HERE = Path(__file__).parent


def factory(eid: int, owner: int, cyborgs: int, prod: int) -> str:
    return f"{eid} FACTORY {owner} {cyborgs} {prod} 0 0"


def troop(eid: int, owner: int, src: int, dst: int, count: int, eta: int) -> str:
    return f"{eid} TROOP {owner} {src} {dst} {count} {eta}"


def write(name: str, n: int, links: list[tuple[int, int, int]], entities: list[str]) -> None:
    lines = [str(n), str(len(links))]
    lines += [f"{a} {b} {d}" for a, b, d in links]
    lines += [str(len(entities))] + entities
    (HERE / name).write_text("\n".join(lines) + "\n")


# Usine 0 à nous (20), usine 1 neutre (5) à 2 tours : capture avec 6.
write("01_capture_neutral.in", 2, [(0, 1, 2)],
      [factory(0, 1, 20, 3), factory(1, 0, 5, 2)])
# Troupe ennemie arrive sur notre usine : renfort.
write("02_defense.in", 3, [(0, 1, 1), (1, 2, 1), (0, 2, 2)],
      [factory(0, 1, 10, 3), factory(1, -1, 5, 2), factory(2, 1, 30, 1),
       troop(3, -1, 1, 0, 15, 1)])
# Aucune cible abordable : WAIT.
write("03_wait.in", 2, [(0, 1, 3)],
      [factory(0, 1, 4, 1), factory(1, 0, 10, 0)])
# Usine ennemie faible : attaque.
write("04_attack.in", 2, [(0, 1, 1)],
      [factory(0, 1, 30, 3), factory(1, -1, 3, 2)])
# Quatre usines : choisir la neutre la plus rentable.
write("05_multi.in", 4, [(0, 1, 2), (0, 2, 1), (0, 3, 4), (1, 2, 3), (2, 3, 2)],
      [factory(0, 1, 40, 3), factory(1, 0, 8, 3), factory(2, 0, 4, 1), factory(3, -1, 20, 2)])
# Graphe déconnecté : WAIT.
write("06_no_route.in", 3, [(0, 1, 2)],
      [factory(0, 1, 20, 3), factory(1, 0, 5, 1), factory(2, -1, 5, 3)])
