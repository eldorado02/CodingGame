#!/usr/bin/env python3
"""Génère des entrées Olymbits Silver déterministes (10 cas + 200 fuzz).

Silver enchaîne 4 mini-jeux en parallèle : course de haies, tir à l'arc,
roller et plongée. Le générateur couvre les quatre types ainsi que les tours
GAME_OVER et les identifications ambiguës (roller vs plongée).
"""
from pathlib import Path
import random

HERE = Path(__file__).parent
LETTERS = "ULDR"


def track(rng: random.Random) -> str:
    """Reproduit le générateur officiel de la course de haies."""
    out = ["."] * rng.randrange(3, 8)
    for _ in range(rng.randrange(3, 7)):
        out.extend(list("#...." if rng.getrandbits(1) else "#..."))
    while len(out) < 30:
        out.append(".")
    return "".join(out[:29]) + "."


def game_line(rng: random.Random, kind: int) -> str:
    if kind == 0:  # course de haies
        positions = [rng.randrange(-20, 30) if rng.random() < 0.3
                     else rng.randrange(30) for _ in range(3)]
        stuns = [rng.randrange(4) for _ in range(3)]
        return " ".join([track(rng)] + [str(v) for v in positions + stuns + [-1]])
    if kind == 1:  # tir à l'arc : GPU = vents (1 chiffre par tour)
        winds = "".join(str(rng.randrange(1, 10)) for _ in range(14))
        coords = [rng.randrange(-20, 21) for _ in range(6)]
        return " ".join([winds] + [str(v) for v in coords + [-1]])
    if kind == 2:  # roller : permutation ULDR + reg6 >= 0
        order = list(LETTERS)
        rng.shuffle(order)
        positions = [rng.randrange(10) for _ in range(3)]
        risks = [rng.choice([-1, -1, 0, 1, 2, 3, 4]) for _ in range(3)]
        return " ".join(["".join(order)] +
                        [str(v) for v in positions + risks + [rng.randrange(15)]])
    # plongée : séquence ULDR, reg6 = -1 pour lever l'ambiguïté avec roller
    goal = "".join(rng.choice(LETTERS) for _ in range(14))
    points = [rng.randrange(200) for _ in range(3)]
    combos = [rng.randrange(14) for _ in range(3)]
    return " ".join([goal] + [str(v) for v in points + combos + [-1]])


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    player = rng.randrange(3)
    nb = 1 + seed % 4
    lines = [str(player), str(nb)]
    kinds = [rng.randrange(4) for _ in range(nb)]
    for turn in range(rng.randrange(6, 20)):
        lines.extend(["0 0 0 0"] * 3)
        for g in range(nb):
            if turn % 9 == 8 or (nb > 1 and rng.random() < 0.1):
                lines.append("GAME_OVER -1 -1 -1 -1 -1 -1 -1")
                continue
            kind = kinds[g] if rng.random() < 0.85 else rng.randrange(4)
            lines.append(game_line(rng, kind))
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
