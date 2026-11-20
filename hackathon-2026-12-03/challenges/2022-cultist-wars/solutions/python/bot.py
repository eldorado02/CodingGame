#!/usr/bin/env python3
"""Cultist Wars — conversions prioritaire, neutralisation des menaces, tirs."""
import sys
from collections import deque


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def manhattan(a: dict, b: dict) -> int:
    return abs(a["x"] - b["x"]) + abs(a["y"] - b["y"])


def bullet_hit(shooter: dict, target: dict, grid: list[str], occupied: dict) -> tuple[int, int] | None:
    """Bresenham du moteur, tracé de la coordonnée Y la plus basse."""
    if shooter["y"] < target["y"]:
        x0, y0, x1, y1 = shooter["x"], shooter["y"], target["x"], target["y"]
        backward = False
    else:
        x0, y0, x1, y1 = target["x"], target["y"], shooter["x"], shooter["y"]
        backward = True

    dx, dy = abs(x1 - x0), abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy
    cx, cy = x0, y0
    hit = (x1, y1)
    while True:
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            cx += sx
        if e2 < dx:
            err += dx
            cy += sy
        if cx == x1 and cy == y1:
            break
        if not (0 <= cx < len(grid[0]) and 0 <= cy < len(grid)):
            return None
        if grid[cy][cx] == "x" or (cx, cy) in occupied:
            if backward:
                hit = (cx, cy)
            else:
                return (cx, cy)
    return hit


def bfs_distance(start: dict, goal: dict, grid: list[str]) -> int:
    """Distance en cases ignorants les unités (le moteur pathfind pour CONVERT)."""
    sx, sy, gx, gy = start["x"], start["y"], goal["x"], goal["y"]
    if (sx, sy) == (gx, gy):
        return 0
    q = deque([(sx, sy, 0)])
    seen = {(sx, sy)}
    i = 0
    while i < len(q):
        x, y, d = q[i]
        i += 1
        for dx, dy in [(0, -1), (1, 0), (0, 1), (-1, 0)]:
            nx, ny = x + dx, y + dy
            if not (0 <= nx < len(grid[0]) and 0 <= ny < len(grid)):
                continue
            if (nx, ny) in seen or grid[ny][nx] == "x":
                continue
            if (nx, ny) == (gx, gy):
                return d + 1
            seen.add((nx, ny))
            q.append((nx, ny, d + 1))
    return 10 ** 6


def emit(action: str) -> None:
    print(action)
    sys.stdout.flush()


def main() -> None:
    my_id = int(rd())
    w, h = map(int, rd().split())
    grid = [rd() for _ in range(h)]

    while True:
        line = rd()
        if not line:
            break
        n = int(line)
        units = {}
        for _ in range(n):
            f = rd().split()
            if len(f) < 6:
                continue
            uid, utype, hp, x, y, owner = map(int, f[:6])
            units[uid] = {"id": uid, "type": utype, "hp": hp,
                          "x": x, "y": y, "owner": owner}

        mine = [u for u in units.values() if u["owner"] == my_id]
        leader = next((u for u in mine if u["type"] == 1), None)
        cultists = [u for u in mine if u["type"] == 0]
        neutrals = [u for u in units.values() if u["owner"] == 2]
        enemies = [u for u in units.values()
                   if u["owner"] not in (my_id, 2) and u["hp"] > 0]
        enemy_cultists = [u for u in enemies if u["type"] == 0]
        occupied = {(u["x"], u["y"]): u for u in units.values() if u["hp"] > 0}

        def find_shot(allowed: set[int] | None = None):
            best = None
            for c in cultists:
                for e in enemies:
                    if allowed is not None and e["id"] not in allowed:
                        continue
                    d = manhattan(c, e)
                    if d > 6:
                        continue
                    hit = bullet_hit(c, e, grid, occupied)
                    if hit is None:
                        continue
                    victim = occupied.get(hit)
                    if victim is not None and victim["owner"] not in (my_id, 2):
                        dv = manhattan(c, victim)
                        damage = 7 - dv
                        key = (0 if damage >= victim["hp"] else 1, -damage, dv,
                               victim["id"], c["id"])
                        if best is None or key < best[0]:
                            best = (key, c, victim)
            return best

        # Neutraliser en priorité un cultiste ennemi qui menace nos unités.
        threats = set()
        for e in enemy_cultists:
            for u in mine:
                if manhattan(e, u) > 6:
                    continue
                hit = bullet_hit(e, u, grid, occupied)
                if hit == (u["x"], u["y"]):
                    threats.add(e["id"])
                    break
        if threats:
            shot = find_shot(threats)
            if shot:
                _, c, victim = shot
                emit(f"{c['id']} SHOOT {victim['id']}")
                continue

        # Le leader cible ses neutres de notre moitié.
        targets: list[dict] = []
        if neutrals:
            mid = w // 2
            if my_id == 0:
                targets = [u for u in neutrals if u["x"] <= mid]
            else:
                targets = [u for u in neutrals if u["x"] >= mid]

        if leader and targets:
            t = min(targets, key=lambda u: (bfs_distance(leader, u, grid), u["id"]))
            emit(f"{leader['id']} CONVERT {t['id']}")
            continue

        # Si un cultiste ennemi est tout proche, le leader le vole (swing de 2).
        if leader and enemy_cultists:
            nearby = [u for u in enemy_cultists if bfs_distance(leader, u, grid) <= 2]
            if nearby:
                t = min(nearby, key=lambda u: (bfs_distance(leader, u, grid), u["id"]))
                emit(f"{leader['id']} CONVERT {t['id']}")
                continue

        # Plus rien à convertir : les cultistes éliminent les ennemis.
        shot = find_shot()
        if shot:
            _, c, victim = shot
            emit(f"{c['id']} SHOOT {victim['id']}")
            continue

        # Un SHOOT hors portée déplace le cultiste vers la cible : jamais de WAIT.
        if cultists and enemies:
            c = min(cultists, key=lambda u: (min(manhattan(u, e) for e in enemies), u["id"]))
            e = min(enemies, key=lambda u: (manhattan(c, u), u["id"]))
            emit(f"{c['id']} SHOOT {e['id']}")
            continue

        emit("WAIT")


if __name__ == "__main__":
    main()
