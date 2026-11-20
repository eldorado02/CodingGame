#!/usr/bin/env python3
"""Cellularena (Wood) — croissance greedy vers la source A la plus proche."""
import sys
from collections import deque

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


class Cell:
    __slots__ = ("wall", "organ", "protein")

    def __init__(self) -> None:
        self.wall = False
        self.organ = False
        self.protein = ""


def main() -> None:
    w, h = map(int, rd().split())
    while True:
        line = rd()
        if not line:
            return
        n = int(line)
        grid = [[Cell() for _ in range(w)] for _ in range(h)]
        mine = []
        for _ in range(n):
            x, y, kind, owner, oid, _dir, _parent, _root = rd().split()
            x, y, owner, oid = int(x), int(y), int(owner), int(oid)
            cell = grid[y][x]
            if kind == "WALL":
                cell.wall = True
            elif kind in ("A", "B", "C", "D"):
                cell.protein = kind
            else:
                cell.organ = True
                if owner == 1:
                    mine.append((x, y, oid))
        my_a = int(rd().split()[0])
        rd()  # protéines adverses
        required = int(rd())

        answer = "WAIT"
        if my_a > 0 and mine:
            # BFS multi-source : pour chaque case libre, parent + premier pas + distance.
            info: dict[tuple[int, int], tuple[int, int, int, int]] = {}
            q = deque()
            for x, y, oid in mine:
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if (0 <= nx < w and 0 <= ny < h and not grid[ny][nx].wall
                            and not grid[ny][nx].organ and (nx, ny) not in info):
                        info[(nx, ny)] = (oid, nx, ny, 1)
                        q.append((nx, ny))
            while q:
                px, py = q.popleft()
                parent, fx, fy, dist = info[(px, py)]
                for dx, dy in DIRS:
                    nx, ny = px + dx, py + dy
                    if (0 <= nx < w and 0 <= ny < h and not grid[ny][nx].wall
                            and not grid[ny][nx].organ and (nx, ny) not in info):
                        info[(nx, ny)] = (parent, fx, fy, dist + 1)
                        q.append((nx, ny))

            sources = [p for p in info if grid[p[1]][p[0]].protein == "A"]
            targets = sources if sources else sorted(info)
            if targets:
                target = min(targets, key=lambda p: (info[p][3], p[1], p[0]))
                parent, fx, fy, _ = info[target]
                answer = f"GROW {parent} {fx} {fy} BASIC"

        print(answer)
        for _ in range(1, required):
            print("WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
