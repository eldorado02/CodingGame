#!/usr/bin/env python3
"""Génère des scénarios Cellularena déterministes (10 cas + fuzz).

Les organes BASIC sont explicitement générés : c'est un piège classique,
leur type commence par 'B' comme la protéine B.
"""
from pathlib import Path
import random

HERE = Path(__file__).parent
W, H = 18, 9
DIRS = "NESW"


def walls() -> set[tuple[int, int]]:
    blocked = {(x, 0) for x in range(W)} | {(x, H - 1) for x in range(W)}
    blocked |= {(0, y) for y in range(H)} | {(W - 1, y) for y in range(H)}
    blocked |= {(x, 4) for x in range(W) if x != 16}
    return blocked


def make_case(seed: int) -> str:
    rng = random.Random(seed)
    blocked = walls()
    free = [(x, y) for y in range(1, H - 1) for x in range(1, W - 1)
            if (x, y) not in blocked]
    rng.shuffle(free)

    my_root, opp_root = (1, 2), (1, 6)
    occupied = {my_root, opp_root}
    organs: list[tuple[int, int, int, str, int, int, int]] = [
        (my_root[0], my_root[1], 1, "ROOT", 1, 0, 1),
        (opp_root[0], opp_root[1], 0, "ROOT", 2, 0, 2),
    ]
    # Quelques organes BASIC des deux camps, avec des types qui piègent
    # un parseur comparant seulement le premier caractère (B/C/D).
    next_id = 3
    for owner, root_id in ((1, 1), (0, 2)):
        for _ in range(rng.randrange(2, 9)):
            pos = next(p for p in free if p not in occupied)
            occupied.add(pos)
            organs.append((pos[0], pos[1], owner, "BASIC", next_id,
                           rng.randrange(4), root_id))
            next_id += 1

    sources = set()
    for pos in free:
        if pos in occupied:
            continue
        if rng.random() < 0.16:
            sources.add(pos)
    # Au moins une source A atteignable, sinon le cas est trivial.
    if not any(p[1] < 4 for p in sources):
        candidates = [p for p in free if p not in occupied and p[1] < 4]
        if candidates:
            sources.add(candidates[0])

    entities = []
    for x in range(W):
        for y in range(H):
            if (x, y) in blocked:
                entities.append(f"{x} {y} WALL -1 0 X 0 0")
    for x, y in sorted(sources):
        entities.append(f"{x} {y} A -1 0 X 0 0")
    for x, y, owner, kind, oid, direction, root in organs:
        entities.append(f"{x} {y} {kind} {owner} {oid} {DIRS[direction]} {root} {root}")

    my_a = rng.randrange(1, 13)
    opp_a = rng.randrange(1, 13)
    lines = [f"{W} {H}", str(len(entities))] + entities
    lines.append(f"{my_a} 0 0 0")
    lines.append(f"{opp_a} 0 0 0")
    lines.append("1")
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        (HERE / f"{seed:02d}.in").write_text(make_case(seed))
