#!/usr/bin/env python3
"""Mean Max Wood 3 — simulation 1 pas des actions, reste dans les épaves."""
import math
import sys

POWERS = (0, 100, 200, 300)
BRAKES = (150, 300)


def round_half_away(z: float) -> int:
    """Arrondi du referee : demi loin de zéro."""
    return math.floor(z + 0.5) if z >= 0 else -math.floor(-z + 0.5)


def simulate(x: int, y: int, vx: int, vy: int,
             target: tuple[int, int] | None, power: int) -> tuple[int, int, int, int]:
    if target is not None and power > 0:
        dx, dy = target[0] - x, target[1] - y
        dist = math.sqrt(dx * dx + dy * dy)
        if dist > 1e-9:
            coef = 2.0 * power / dist  # accél = THROTTLE / masse(0.5)
            nvx, nvy = vx + dx * coef, vy + dy * coef
        else:
            nvx, nvy = float(vx), float(vy)
    else:
        nvx, nvy = float(vx), float(vy)
    nx = round_half_away(x + nvx)
    ny = round_half_away(y + nvy)
    nvx_i = round_half_away(nvx * 0.8)
    nvy_i = round_half_away(nvy * 0.8)
    return nx, ny, nvx_i, nvy_i


def score(nx: int, ny: int, nvx: int, nvy: int,
          wrecks: list[tuple[int, int, int, int]]) -> float:
    inside = 0
    water = 0
    closest = 1e18
    for wx, wy, wr, wwater in wrecks:
        d = math.sqrt((nx - wx) ** 2 + (ny - wy) ** 2)
        if d <= wr:
            inside += 1
            water += wwater
        if d < closest:
            closest = d
    speed = math.sqrt(nvx * nvx + nvy * nvy)
    return inside * 1_000_000 + water * 10 - closest - speed


def main() -> None:
    while True:
        lines = [sys.stdin.readline() for _ in range(3)]  # scores
        if not lines[0]:
            return
        for _ in range(3):  # rages (placeholders)
            if not sys.stdin.readline():
                return
        line = sys.stdin.readline()
        if not line:
            return
        n = int(line)
        reaper = None
        wrecks: list[tuple[int, int, int, int]] = []  # x, y, radius, water
        for _ in range(n):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            kind = int(parts[1])
            player = int(parts[2])
            radius = int(parts[4])
            x, y = int(parts[5]), int(parts[6])
            vx, vy = int(parts[7]), int(parts[8])
            extra = int(parts[9])
            if kind == 0 and player == 0:
                reaper = (x, y, vx, vy)
            elif kind == 4:
                wrecks.append((x, y, radius, extra))

        if reaper is None:
            print("WAIT\nWAIT\nWAIT", flush=True)
            continue
        x, y, vx, vy = reaper

        candidates: list[tuple[tuple[int, int] | None, int]] = [(None, 0)]
        for wx, wy, _wr, _ww in wrecks:
            for p in POWERS:
                candidates.append(((wx, wy), p))
        if vx or vy:
            for p in BRAKES:
                candidates.append(((x - vx, y - vy), p))

        best_score = -1e18
        best_target = None
        best_power = 0
        for target, power in candidates:
            nx, ny, nvx, nvy = simulate(x, y, vx, vy, target, power)
            s = score(nx, ny, nvx, nvy, wrecks)
            if s > best_score:
                best_score = s
                best_target, best_power = target, power

        if best_target is None or best_power == 0:
            action = "WAIT"
        else:
            action = f"{best_target[0]} {best_target[1]} {best_power}"
        print(f"{action}\nWAIT\nWAIT", flush=True)


if __name__ == "__main__":
    main()
