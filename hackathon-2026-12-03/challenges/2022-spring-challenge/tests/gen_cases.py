#!/usr/bin/env python3
"""Génère des entrées Spring Challenge 2022 déterministes (10 cas, 8 tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20220421)
MAP_W, MAP_H = 17630, 9000


def case(k: int) -> str:
    bx, by = (0, 0) if k % 2 == 0 else (MAP_W, MAP_H)
    ex, ey = MAP_W - bx, MAP_H - by
    lines = [f"{bx} {by}", "3"]
    sign = 1 if bx == 0 else -1
    my = [
        [bx + sign * 500, by + sign * 300],
        [bx + sign * 800, by + sign * 700],
        [bx + sign * 1200, by + sign * 400],
    ]
    opp = [
        [ex - sign * 500, ey - sign * 300],
        [ex - sign * 900, ey - sign * 600],
    ]
    next_id = 100

    for turn in range(8):
        entities = []
        for i, (hx, hy) in enumerate(my):
            entities.append(f"{i} 1 {hx} {hy} -1 -1 -1 -1 -1 -1 -1")
        for i, (ox, oy) in enumerate(opp):
            entities.append(f"{50 + i} 2 {ox} {oy} -1 -1 -1 -1 -1 -1 -1")

        for j in range(4 + rng.randrange(6)):
            hp = rng.choice([1, 2, 3, 5])
            mode = j % 4
            if mode == 0:  # cible notre base, déjà en approche
                x = bx + sign * rng.randrange(900, 4500)
                y = by + sign * rng.randrange(700, 3500)
                vx, vy = 0, 0
                near, threat = 1, 1
            elif mode == 1:  # cible notre base, pas encore en approche
                x = bx + sign * rng.randrange(4500, 9000)
                y = by + sign * rng.randrange(3000, 7000)
                vx = -sign * rng.randrange(100, 401)
                vy = -sign * rng.randrange(100, 401)
                near, threat = 0, 1
            elif mode == 2:  # cible l'adversaire : doit être ignoré
                x = ex - sign * rng.randrange(900, 4500)
                y = ey - sign * rng.randrange(700, 3500)
                vx, vy = 0, 0
                near, threat = 1, 2
            else:  # trajectoire sans menace
                x = rng.randrange(2000, MAP_W - 2000)
                y = rng.randrange(1000, MAP_H - 1000)
                vx = rng.choice([-300, -150, 150, 300])
                vy = rng.choice([-200, 100, 200])
                near, threat = 0, 0
            entities.append(
                f"{next_id} 0 {x} {y} 0 0 {hp} {vx} {vy} {near} {threat}"
            )
            next_id += 1

        rng.shuffle(entities)
        lines += ["3 0", "3 0", str(len(entities)), *entities]

        for pos in my:
            pos[0] = max(0, min(MAP_W, pos[0] - sign * rng.randrange(0, 201)))
            pos[1] = max(0, min(MAP_H, pos[1] - sign * rng.randrange(0, 201)))
        for pos in opp:
            pos[0] = max(0, min(MAP_W, pos[0] + sign * rng.randrange(0, 201)))
            pos[1] = max(0, min(MAP_H, pos[1] + sign * rng.randrange(0, 201)))

    return "\n".join(lines) + "\n"


for k in range(10):
    (HERE / f"{k + 1:02d}.in").write_text(case(k))
