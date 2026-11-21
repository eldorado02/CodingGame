#!/usr/bin/env python3
"""Génère des entrées déterministes pour les 4 implémentations."""
import os
import random

W, H = 13, 7
OUT = os.path.dirname(os.path.abspath(__file__))


def gen(seed: int) -> str:
    r = random.Random(seed)
    lines = [str(r.randrange(2)), f"{W} {H}"]
    cells = [(x, y) for x in range(W) for y in range(H)]
    left = [(x, y) for x, y in cells if x < 6 and (x, y) != (0, 3)]
    obstacles = set()
    for x, y in r.sample(left, r.randrange(2, 12)):
        obstacles.add((x, y))
        obstacles.add((W - 1 - x, y))
    for y in range(H):
        lines.append("".join("x" if (x, y) in obstacles else "." for x in range(W)))

    free = [(x, y) for x, y in cells
            if (x, y) not in obstacles and (x, y) not in ((0, 3), (W - 1, 3))]
    r.shuffle(free)
    units = [
        (2, 1, 10, 0, 3, 0),
        (3, 1, 10, W - 1, 3, 1),
    ]
    uid = 4
    for x, y in free[:6]:
        units.append((uid, 0, 10, x, y, 2))
        uid += 1
    for x, y in free[6:12]:
        owner = r.randrange(2)
        units.append((uid, 0, r.randrange(4, 11), x, y, owner))
        uid += 1

    for turn in range(12):
        lines.append(str(len(units)))
        for u in units:
            lines.append(" ".join(map(str, u)))
        r.shuffle(units)
        for i, u in enumerate(units):
            if u[5] == 2:
                dx, dy = r.choice([(0, 1), (1, 0), (0, -1), (-1, 0)])
                nx, ny = u[3] + dx, u[4] + dy
                if (0 <= nx < W and 0 <= ny < H and (nx, ny) not in obstacles
                        and all(j == i or v[3] != nx or v[4] != ny
                                for j, v in enumerate(units))):
                    units[i] = (u[0], u[1], u[2], nx, ny, u[5])
    lines.append("")
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    for seed in range(1, 11):
        with open(os.path.join(OUT, f"{seed:02d}.in"), "w") as f:
            f.write(gen(seed))
    fuzz = os.path.join(OUT, "fuzz")
    os.makedirs(fuzz, exist_ok=True)
    for old in os.listdir(fuzz):
        if old.endswith(".in"):
            os.remove(os.path.join(fuzz, old))
    for seed in range(200):
        with open(os.path.join(fuzz, f"case_{seed:03d}.in"), "w") as f:
            f.write(gen(1000 + seed))
