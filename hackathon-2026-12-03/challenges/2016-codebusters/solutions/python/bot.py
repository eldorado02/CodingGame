#!/usr/bin/env python3
"""CodeBusters — stratégie Wood : un fantôme par buster, bande de capture."""
import math
import sys

W, H = 16000, 9000
BUST_MIN2 = 900 * 900
BUST_MAX2 = 1760 * 1760
STEP = 800


def step_toward(x: int, y: int, tx: int, ty: int) -> tuple[int, int]:
    """Point à 800 unités de (x,y) vers (tx,ty), arrondi et borné."""
    dx, dy = tx - x, ty - y
    d2 = dx * dx + dy * dy
    if d2 == 0:
        return x, y
    d = math.sqrt(d2)
    nx = math.floor(x + STEP * dx / d + 0.5)
    ny = math.floor(y + STEP * dy / d + 0.5)
    return max(0, min(W, nx)), max(0, min(H, ny))


def choose(busters: list[dict], ghosts: list[dict], team: int) -> list[str]:
    bx, by = (0, 0) if team == 0 else (W, H)
    reserved: set[int] = set()
    orders: list[str] = []

    for b in busters:
        if b["state"] == 1:
            dx, dy = bx - b["x"], by - b["y"]
            if dx * dx + dy * dy <= STEP * STEP:
                orders.append("RELEASE")
            else:
                nx, ny = step_toward(b["x"], b["y"], bx, by)
                orders.append(f"MOVE {nx} {ny}")
            continue

        candidates = [g for g in ghosts if g["id"] not in reserved]
        if not candidates:
            nx, ny = step_toward(b["x"], b["y"], W // 2, H // 2)
            orders.append(f"MOVE {nx} {ny}")
            continue

        g = min(candidates, key=lambda g: (g["x"] - b["x"]) ** 2 + (g["y"] - b["y"]) ** 2)
        dx, dy = g["x"] - b["x"], g["y"] - b["y"]
        d2 = dx * dx + dy * dy
        if d2 < BUST_MIN2:
            nx, ny = step_toward(b["x"], b["y"], b["x"] - dx, b["y"] - dy)
        elif d2 <= BUST_MAX2:
            reserved.add(g["id"])
            orders.append(f"BUST {g['id']}")
            continue
        else:
            nx, ny = step_toward(b["x"], b["y"], g["x"], g["y"])
        orders.append(f"MOVE {nx} {ny}")

    return orders


def main() -> None:
    per_player = int(sys.stdin.readline())
    ghost_count = int(sys.stdin.readline())
    team = int(sys.stdin.readline())

    while True:
        line = sys.stdin.readline()
        if not line:
            return
        n = int(line)
        busters: list[dict] = []
        ghosts: list[dict] = []
        for _ in range(n):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            eid, x, y, kind, state, value = map(int, parts)
            if kind == team:
                busters.append({"id": eid, "x": x, "y": y, "state": state, "value": value})
            elif kind == -1:
                ghosts.append({"id": eid, "x": x, "y": y})
        for order in choose(busters, ghosts, team):
            print(order, flush=True)


if __name__ == "__main__":
    main()
