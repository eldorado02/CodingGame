#!/usr/bin/env python3
"""Fantastic Bits Wood 2 — attaquant + défenseur, grab puis THROW au but."""
import sys

W, H = 16000, 7500
GOAL_Y = 3750


def d2(ax: int, ay: int, bx: int, by: int) -> int:
    return (ax - bx) ** 2 + (ay - by) ** 2


def play(wizards: list[dict], snaffles: list[dict], team: int) -> list[str]:
    goal_x = W if team == 0 else 0
    own_x = 0 if team == 0 else W
    chosen: set[int] = set()
    orders: list[str] = []

    for i, w in enumerate(wizards):
        if w["state"] == 1:
            orders.append(f"THROW {goal_x} {GOAL_Y} 500")
            continue

        if i == 0:  # attaquant : Snaffle le plus proche
            pool = [s for s in snaffles if s["id"] not in chosen]
            target = min(pool, key=lambda s: d2(w["x"], w["y"], s["x"], s["y"])) if pool else None
        else:  # défenseur : Snaffle le plus proche de notre but, dans notre camp
            in_own_half = [s for s in snaffles if (s["x"] < W // 2) == (team == 0)]
            pool = [s for s in in_own_half if s["id"] not in chosen]
            if pool:
                target = min(pool, key=lambda s: d2(own_x, GOAL_Y, s["x"], s["y"]))
            else:
                pool = [s for s in snaffles if s["id"] not in chosen]
                target = min(pool, key=lambda s: d2(w["x"], w["y"], s["x"], s["y"])) if pool else None

        if target is None:
            orders.append(f"MOVE {goal_x} {GOAL_Y} 150")
            continue
        chosen.add(target["id"])

        # Position anticipée : le Snaffle dérive de sa vitesse.
        tx = max(0, min(W, target["x"] + 3 * target["vx"]))
        ty = max(0, min(H, target["y"] + 3 * target["vy"]))
        distance2 = d2(w["x"], w["y"], tx, ty)
        thrust = 150 if distance2 > 1200 * 1200 else 60
        orders.append(f"MOVE {tx} {ty} {thrust}")

    return orders


def main() -> None:
    line = sys.stdin.readline()
    if not line:
        return
    team = int(line)
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        line2 = sys.stdin.readline()
        if not line2:
            return
        line3 = sys.stdin.readline()
        if not line3:
            return
        n = int(line3)
        wizards: list[dict] = []
        snaffles: list[dict] = []
        for _ in range(n):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            eid = int(parts[0])
            kind = parts[1]
            x, y, vx, vy, state = map(int, parts[2:7])
            if kind == "WIZARD":
                wizards.append({"id": eid, "x": x, "y": y, "vx": vx, "vy": vy, "state": state})
            elif kind == "SNAFFLE":
                snaffles.append({"id": eid, "x": x, "y": y, "vx": vx, "vy": vy, "state": state})
        for order in play(wizards, snaffles, team):
            print(order, flush=True)


if __name__ == "__main__":
    main()
