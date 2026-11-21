#!/usr/bin/env python3
"""Elemental Wars (Wood) — multi-source BFS, chase prey / evade predator.

The prey/predator cycle is W->F->P->W. Each turn we score the five possible
first steps (WAIT + 4 neighbours) with distances to prey and predator, then
emit `id MOVE x y` towards the chosen adjacent cell (or `id WAIT`).
"""
import sys
from collections import deque

DIRS = ((-1, 0, "N"), (0, 1, "E"), (1, 0, "S"), (0, -1, "W"))
PREY = {"W": "F", "F": "P", "P": "W"}
PRED = {"W": "P", "F": "W", "P": "F"}


def distances(sources, grid, size):
    """Multi-source BFS over walkable cells; walls block, camps do not."""
    dist = [[-1] * size for _ in range(size)]
    q = deque()
    for x, y in sources:
        if 0 <= x < size and 0 <= y < size and dist[y][x] < 0:
            dist[y][x] = 0
            q.append((x, y))
    while q:
        x, y = q.popleft()
        for dx, dy, _name in DIRS:
            nx, ny = x + dx, y + dy
            if (0 <= nx < size and 0 <= ny < size and grid[ny][nx] != "#"
                    and dist[ny][nx] < 0):
                dist[ny][nx] = dist[y][x] + 1
                q.append((nx, ny))
    return dist


def main():
    rd = sys.stdin.readline
    tribe = rd().strip()
    if not tribe:
        return
    size = int(rd())
    grid = [rd().strip() for _ in range(size)]

    while True:
        line = rd()  # scores (non utilisés)
        if not line:
            return
        count = int(rd())
        mine, prey, predator = [], [], []
        for _ in range(count):
            parts = rd().split()
            if len(parts) < 5:
                return
            eid = int(parts[0])
            t = parts[1]
            x, y = int(parts[2]), int(parts[3])
            prisoner = int(parts[4]) == 1
            if prisoner:
                continue
            if t == tribe:
                mine.append((eid, x, y))
            elif t == PREY[tribe]:
                prey.append((x, y))
            elif t == PRED[tribe]:
                predator.append((x, y))

        d_prey = distances(prey, grid, size)
        d_pred = distances(predator, grid, size)
        prey_set = set(prey)
        actions = []
        for eid, x, y in mine:
            best = None  # (key, order, nx, ny)
            # order 0 = WAIT, 1..4 = N,E,S,W (referee tie-break order)
            for order, (dx, dy, _name) in enumerate([(0, 0, "WAIT")] + list(DIRS)):
                nx, ny = x + dx, y + dy
                if not (0 <= nx < size and 0 <= ny < size):
                    continue
                if (nx, ny) != (x, y) and grid[ny][nx] == "#":
                    continue
                dp = d_prey[ny][nx]
                dd = d_pred[ny][nx]
                score = 0
                if (nx, ny) in prey_set:
                    score += 1000
                if dp >= 0:
                    score -= 2 * dp
                if dd >= 0:
                    score += 3 * dd
                    if dd <= 1:
                        score -= 50
                key = (score, -order, -ny, -nx)
                if best is None or key > best[0]:
                    best = (key, order, nx, ny)
            if best is None or best[1] == 0:
                actions.append("%d WAIT" % eid)
            else:
                actions.append("%d MOVE %d %d" % (eid, best[2], best[3]))
        print(";".join(actions) if actions else "WAIT", flush=True)


if __name__ == "__main__":
    main()
