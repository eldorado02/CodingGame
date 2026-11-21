#!/usr/bin/env python3
"""Générateur déterministe de cas Ataxx valides (10 cas figés + 200 fuzz).

Chaque cas = une seule tournée complète (init + 1 tour), donc exactement une
ligne de sortie attendue par implémentation. Les actions générées sont
*légales* par construction : elles sont dérivées du plateau.
"""
from pathlib import Path
import random

HERE = Path(__file__).parent
NEIGH = [(-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0),
         (-1, 1), (0, 1), (1, 1)]
JUMPS = [(dx * 2, dy * 2) for dx, dy in NEIGH]


def in_bounds(x, y):
    return 0 <= x < 7 and 0 <= y < 7


def coord(x, y):
    return chr(ord('a') + x) + str(y + 1)


def legal_actions(cells, me_char):
    """Calcule toutes les actions légales pour `me_char` sur le plateau."""
    actions = []
    for y in range(7):
        for x in range(7):
            if cells[y][x] != '.':
                continue
            # Spawn: case vide adjacente à une pièce à nous.
            if any(in_bounds(x + dx, y + dy) and cells[y + dy][x + dx] == me_char
                   for dx, dy in NEIGH):
                actions.append(coord(x, y))
    for y in range(7):
        for x in range(7):
            if cells[y][x] != me_char:
                continue
            # Jump: piece à nous, cible vide à deux cases (Chebyshev).
            for dx, dy in JUMPS:
                nx, ny = x + dx, y + dy
                if in_bounds(nx, ny) and cells[ny][nx] == '.':
                    actions.append(coord(x, y) + coord(nx, ny))
    return actions


def make_case(seed):
    rng = random.Random(seed)
    me = rng.randrange(0, 2)
    me_char = str(me)
    opp_char = str(1 - me)

    # Plateau : murs, pièces à nous, pièces adverses, cases vides.
    cells = [['.' for _ in range(7)] for _ in range(7)]
    for y in range(7):
        for x in range(7):
            if rng.random() < 0.10:
                cells[y][x] = '#'

    density = rng.choice((0.15, 0.30, 0.50, 0.75, 0.92))
    if seed % 23 == 0:
        # Plateau plein (hors murs) : aucune case vide -> aucune action légale.
        # C'est un état « passed » valide côté referee ; le bot doit dire "random".
        density = 1.0
    for y in range(7):
        for x in range(7):
            if cells[y][x] != '.':
                continue
            r = rng.random()
            if r < density * 0.50:
                cells[y][x] = me_char
            elif r < density * 0.85:
                cells[y][x] = opp_char
            elif density >= 1.0:
                cells[y][x] = opp_char

    actions = legal_actions(cells, me_char)
    rng.shuffle(actions)

    if actions:
        # Tantôt toutes les actions, tantôt un sous-ensemble (test du parsing).
        if rng.random() < 0.5:
            actions = actions[:rng.randint(1, len(actions))]

    last_action = rng.choice((
        "null", "pass", "b7", "a7c7", "b1;d3", "random",
    ))

    lines = [str(me)]
    for y in range(7):                       # bas (y=0) -> haut (y=6)
        lines.append("".join(cells[y]))
    lines.append(last_action)
    lines.append(str(len(actions)))
    lines.extend(actions)
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    # 10 cas figés commités dans tests/*.in.
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
    # Le harnais utilise make_case(seed) pour seeds 200..399 (200 cas de fuzz).
    print("wrote 10 fixed cases (seeds 1..10); fuzz = seeds 200..399")
