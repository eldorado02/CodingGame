#!/usr/bin/env python3
"""Spring Challenge 2022 (Wood 4) — défense de base par 3 héros."""
import math
import sys

MAP_W, MAP_H = 17630, 9000


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def dist(ax: int, ay: int, bx: int, bx2: int) -> float:
    return math.hypot(ax - bx, ay - bx2)


def main() -> None:
    base = list(map(int, rd().split()))
    bx, by = base[0], base[1]
    n_heroes = int(rd())

    while True:
        line = rd()
        if not line:
            break
        f = line.split()
        if len(f) < 2:
            break
        my_hp, _mana = int(f[0]), int(f[1])
        rd()  # adversaire
        n = int(rd())
        monsters = []
        heroes = []
        for _ in range(n):
            e = list(map(int, rd().split()))
            if len(e) < 11:
                continue
            eid, etype, x, y = e[0], e[1], e[2], e[3]
            hp, vx, vy, near, threat = e[6], e[7], e[8], e[9], e[10]
            if etype == 0:
                if near:
                    px, py = (bx, by) if threat == 1 else (MAP_W - bx, MAP_H - by)
                    dx, dy = px - x, py - y
                    d = max(1, math.hypot(dx, dy))
                    nx = x + int(400 * dx / d)
                    ny = y + int(400 * dy / d)
                else:
                    nx, ny = x + vx, y + vy
                monsters.append({"x": x, "y": y, "nx": nx, "ny": ny,
                                 "hp": hp, "threat": threat, "near": near})
            elif etype == 1:
                heroes.append((eid, x, y))

        threats = [m for m in monsters if m["threat"] == 1]
        threats.sort(key=lambda m: (dist(m["nx"], m["ny"], bx, by), m["hp"], m["x"], m["y"]))
        used: set[int] = set()
        commands = {}
        for hid, hx, hy in heroes:
            choice = None
            for i, m in enumerate(threats):
                if i in used:
                    continue
                score = dist(hx, hy, m["nx"], m["ny"])
                if choice is None or score < choice[0]:
                    choice = (score, i)
            if choice is None:
                commands[hid] = "WAIT"
                continue
            used.add(choice[1])
            m = threats[choice[1]]
            tx, ty = m["nx"], m["ny"]
            d = dist(hx, hy, tx, ty)
            if d <= 800:
                tx, ty = hx, hy  # reste à portée de la cible
            commands[hid] = f"MOVE {int(tx)} {int(ty)}"
        for hid, _hx, _hy in heroes:
            print(commands.get(hid, "WAIT"))
        sys.stdout.flush()


if __name__ == "__main__":
    main()
