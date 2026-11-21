#!/usr/bin/env python3
"""Cellularena (Wood 3+) — HARVESTER puis croissance greedy.

Dès que C et D sont disponibles, on place un HARVESTER face à une protéine non
déjà récoltée. Les cases sources d'un HARVESTER sont **bloquées dans le BFS** :
sans cela, un GROW BASIC vers une source plus lointaine atterrit sur la source
harvesée, l'absorbe (3 protéines) et stoppe la récolte.
"""
import sys
from collections import deque

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))
DIR_NAME = {(0, -1): "N", (1, 0): "E", (0, 1): "S", (-1, 0): "W"}


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def main() -> None:
    w, h = map(int, rd().split())
    while True:
        line = rd()
        if not line:
            return
        n = int(line)
        grid = [[{"wall": False, "organ": False, "protein": "", "dir": ""}
                 for _ in range(w)] for _ in range(h)]
        mine = []
        harvested: set[tuple[int, int]] = set()
        for _ in range(n):
            f = rd().split()
            x, y, kind, owner, oid = int(f[0]), int(f[1]), f[2], int(f[3]), int(f[4])
            d = f[5]
            cell = grid[y][x]
            if kind == "WALL":
                cell["wall"] = True
            elif kind in ("A", "B", "C", "D"):
                cell["protein"] = kind
            else:
                cell["organ"] = True
                cell["dir"] = d
                if owner == 1:
                    mine.append((x, y, oid))
                    if kind == "HARVESTER" and d in DIR_NAME.values():
                        dx, dy = next((dx, dy) for dx, dy in DIRS
                                      if DIR_NAME[(dx, dy)] == d)
                        sx, sy = x + dx, y + dy
                        if 0 <= sx < w and 0 <= sy < h:
                            harvested.add((sx, sy))
        proteins = list(map(int, rd().split()))
        rd()
        required = int(rd())

        answer = "WAIT"
        if mine and required > 0:
            info: dict[tuple[int, int], tuple[int, int, int, int]] = {}
            q = deque()
            for x, y, oid in mine:
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if (0 <= nx < w and 0 <= ny < h and not grid[ny][nx]["wall"]
                            and not grid[ny][nx]["organ"] and (nx, ny) not in info
                            and (nx, ny) not in harvested):
                        info[(nx, ny)] = (oid, nx, ny, 1)
                        q.append((nx, ny))
            while q:
                px, py = q.popleft()
                parent, fx, fy, dist = info[(px, py)]
                for dx, dy in DIRS:
                    nx, ny = px + dx, py + dy
                    if (0 <= nx < w and 0 <= ny < h and not grid[ny][nx]["wall"]
                            and not grid[ny][nx]["organ"] and (nx, ny) not in info
                            and (nx, ny) not in harvested):
                        info[(nx, ny)] = (parent, fx, fy, dist + 1)
                        q.append((nx, ny))

            if proteins[2] > 0 and proteins[3] > 0:
                best = None
                for (tx, ty), (pid, _fx, _fy, dist) in info.items():
                    if dist != 1:
                        continue
                    for dx, dy in DIRS:
                        sx, sy = tx + dx, ty + dy
                        if (0 <= sx < w and 0 <= sy < h and grid[sy][sx]["protein"]
                                and (sx, sy) not in harvested):
                            prio = 0 if grid[sy][sx]["protein"] == "A" else 1
                            key = (prio, ty, tx, DIR_NAME[(dx, dy)])
                            if best is None or key < best[0]:
                                best = (key, pid, tx, ty, DIR_NAME[(dx, dy)])
                if best:
                    _, pid, tx, ty, direction = best
                    answer = f"GROW {pid} {tx} {ty} HARVESTER {direction}"

            if answer == "WAIT" and proteins[0] > 0:
                goals = [p for p in info if grid[p[1]][p[0]]["protein"] == "A"]
                if not goals:
                    goals = [p for p in info if grid[p[1]][p[0]]["protein"]]
                if not goals:
                    goals = sorted(info)
                if goals:
                    target = min(goals, key=lambda p: (info[p][3], p[1], p[0]))
                    parent, fx, fy, _ = info[target]
                    answer = f"GROW {parent} {fx} {fy} BASIC"

        print(answer)
        for _ in range(max(0, required - 1)):
            print("WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
