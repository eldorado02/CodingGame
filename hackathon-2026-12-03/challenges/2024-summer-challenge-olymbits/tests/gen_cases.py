#!/usr/bin/env python3
"""Génère des entrées Olymbits déterministes (10 cas + fuzz).

La ligue Wood joue jusqu'à 4 courses d'obstacles en parallèle ; on varie donc
`nbGames` de 1 à 4 et on insère des tours GAME_OVER (et des courses isolées
réinitialisées) pour couvrir la logique multi-machines.
"""
from pathlib import Path
import random

HERE = Path(__file__).parent


def track(rng: random.Random) -> str:
    """Reproduit le générateur officiel : 3..7 cases, 3..6 haies espacées de 3/4."""
    out = ["."] * rng.randrange(3, 8)
    for _ in range(rng.randrange(3, 7)):
        out.extend(list("#...." if rng.getrandbits(1) else "#..."))
    while len(out) < 30:
        out.append(".")
    return "".join(out[:29]) + "."


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    player = rng.randrange(3)
    nb = 1 + seed % 4
    lines = [str(player), str(nb)]
    for turn in range(rng.randrange(6, 20)):
        lines.extend(["0 0 0 0"] * 3)
        for g in range(nb):
            if turn % 9 == 8 or (nb > 1 and rng.random() < 0.1):
                lines.append("GAME_OVER -1 -1 -1 -1 -1 -1 -1")
                continue
            board = track(rng)
            positions = [rng.randrange(30) for _ in range(3)]
            stuns = [rng.randrange(4) for _ in range(3)]
            lines.append(" ".join([board] + [str(v) for v in positions + stuns + [-1]]))
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
