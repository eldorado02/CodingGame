#!/usr/bin/env python3
"""Génère les cas déterministes de Botters of the Galaxy."""
from pathlib import Path

HERE = Path(__file__).parent

# Le referee réel ajoute 9 champs « héros » à TOUTES les entités (heroType='-'
# pour les non-héros). On teste les deux formats pour garantir la robustesse.
HERO_TAIL = " 0 0 0 100 100 1 HULK 1 0"
TOWER_TAIL = " 0 0 0 0 0 0 - 1 0"


def entity(eid: int, team: int, kind: str, x: int, y: int, health: int,
           full: bool = False) -> str:
    base = f"{eid} {team} {kind} {x} {y} 100 {health} {max(health,1)} 0 80 200 0 0"
    if kind == "HERO":
        return base + HERO_TAIL
    if full:
        return base + TOWER_TAIL
    return base


def write(name: str, turns: list[tuple[int, list[str]]]) -> None:
    lines = ["0", "0", "0"]  # team, bushes, items
    for round_type, entities in turns:
        lines += ["0", "0", str(round_type), str(len(entities))] + entities
    (HERE / name).write_text("\n".join(lines) + "\n")


ME = entity(0, 0, "HERO", 200, 590, 1450)
FOE_HERO = entity(1, 1, "HERO", 1720, 590, 1000)
FOE_TOWER = entity(2, 1, "TOWER", 1800, 375, 3000)

# 01 — pick avec entités (bug réel : ils doivent être consommés).
write("01_pick.in", [(-1, [ME, FOE_TOWER, FOE_HERO])])
# 02 — héros adverse prioritaire.
write("02_hero.in", [(1, [ME, FOE_TOWER, FOE_HERO])])
# 03 — plus de héros adverse : la tour.
write("03_tower.in", [(1, [ME, FOE_TOWER])])
# 04 — aucune cible ennemie : WAIT.
write("04_wait.in", [(1, [ME])])
# 05 — héros adverse mort : la tour.
write("05_dead.in", [(1, [ME, entity(1, 1, "HERO", 1720, 590, 0), FOE_TOWER])])
# 06 — deux héros ennemis : attaquer le plus proche (1000,100).
write("06_order.in", [(1, [ME,
                           entity(1, 1, "HERO", 1720, 590, 1000),
                           entity(3, 1, "HERO", 1000, 100, 900),
                           FOE_TOWER])])
# 07 — plusieurs tours : pick puis partie. Détecte toute ligne non consommée.
write("07_multiturn.in", [(-1, [ME, FOE_TOWER, FOE_HERO]),
                          (1, [ME, FOE_TOWER, FOE_HERO])])
# 08 — format réel du referee (22 champs sur TOWER) + deux tours.
me_full = entity(0, 0, "HERO", 200, 590, 1450, full=True)
tower_full = entity(2, 1, "TOWER", 1800, 375, 3000, full=True)
foe_full = entity(1, 1, "HERO", 1720, 590, 1000, full=True)
write("08_realformat.in", [(-1, [me_full, tower_full, foe_full]),
                           (1, [me_full, tower_full, foe_full])])
