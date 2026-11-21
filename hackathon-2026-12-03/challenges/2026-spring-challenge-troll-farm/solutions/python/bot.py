#!/usr/bin/env python3
"""Troll Farm (Wood) — un troll : récolte l'arbre fruitier le plus proche,
puis rentre déposer au plus près de la cabane."""
import sys
from collections import deque

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def bfs(w: int, h: int, blocked: set[tuple[int, int]], start: tuple[int, int]):
    """Distances et premier pas depuis start vers toutes les cases praticables."""
    dist = {start: 0}
    first: dict[tuple[int, int], tuple[int, int]] = {}
    q = deque([start])
    while q:
        x, y = q.popleft()
        for dx, dy in DIRS:
            nxt = (x + dx, y + dy)
            if (0 <= nxt[0] < w and 0 <= nxt[1] < h and nxt not in blocked
                    and nxt not in dist):
                dist[nxt] = dist[(x, y)] + 1
                first[nxt] = nxt if dist[(x, y)] == 0 else first[(x, y)]
                q.append(nxt)
    return dist, first


def main() -> None:
    w, h = map(int, rd().split())
    lines = [rd() for _ in range(h)]
    shack = next((x, y) for y, row in enumerate(lines)
                 for x, ch in enumerate(row) if ch == "0")
    # Seules les cases GRASS sont praticables : la cabane elle-même ne l'est pas.
    blocked = {(x, y) for y, row in enumerate(lines)
               for x, ch in enumerate(row) if ch != "."}

    while True:
        if not rd():
            return
        rd()  # inventaire adverse
        trees = []
        for _ in range(int(rd())):
            kind, xs, ys, size, health, fruits, cooldown = rd().split()
            trees.append((int(xs), int(ys), int(size), int(fruits), int(cooldown)))
        my_troll = None
        for _ in range(int(rd())):
            fields = rd().split()
            if int(fields[1]) == 0:
                my_troll = fields
        if my_troll is None:
            print("WAIT", flush=True)
            continue

        tid = int(my_troll[0])
        x, y = int(my_troll[2]), int(my_troll[3])
        carrying = sum(int(v) for v in my_troll[8:12])
        here = (x, y)
        dist, first = bfs(w, h, blocked, here)

        if carrying:
            if (abs(x - shack[0]) + abs(y - shack[1])) == 1:
                print(f"DROP {tid}", flush=True)
                continue
            goals = [(shack[0] + dx, shack[1] + dy) for dx, dy in DIRS
                     if (shack[0] + dx, shack[1] + dy) in dist]
            if not goals:
                print("WAIT", flush=True)
                continue
            goal = min(goals, key=lambda p: (dist[p], p[1], p[0]))
            step = first[goal]
            print(f"MOVE {tid} {step[0]} {step[1]}", flush=True)
            continue

        fruit_trees = [(tx, ty, f) for tx, ty, _s, f, _cd in trees if f > 0]
        if fruit_trees and (x, y) in [(tx, ty) for tx, ty, _ in fruit_trees]:
            print(f"HARVEST {tid}", flush=True)
            continue
        if fruit_trees:
            target = min(fruit_trees, key=lambda t: (dist.get((t[0], t[1]), 10**9),
                                                     -t[2], t[1], t[0]))
            if (target[0], target[1]) in first:
                step = first[(target[0], target[1])]
                print(f"MOVE {tid} {step[0]} {step[1]}", flush=True)
            else:
                print("WAIT", flush=True)
            continue

        # Aucun fruit : on attend près d'un arbre mature prêt à produire.
        print("WAIT", flush=True)


if __name__ == "__main__":
    main()
