#!/usr/bin/env python3
"""A Code of Ice and Fire (Wood 3) — rush du QG adverse."""
import sys
from collections import deque

W = H = 12
DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))
INF = 10**9


def rd() -> str:
    return sys.stdin.readline().strip()


def main() -> None:
    line = rd()
    if not line:
        return
    mines = int(line)
    for _ in range(mines):
        rd()

    while True:
        line = rd()
        if not line:
            break
        gold = int(line)
        rd()  # income
        rd()  # opponent gold
        rd()  # opponent income
        grid = [rd() for _ in range(H)]

        nb = int(rd())
        my_hq = None
        enemy_hq = None
        for _ in range(nb):
            owner, kind, x, y = map(int, rd().split())
            if kind == 0:
                if owner == 0:
                    my_hq = (x, y)
                else:
                    enemy_hq = (x, y)

        nu = int(rd())
        units = []
        for _ in range(nu):
            owner, uid, _level, x, y = map(int, rd().split())
            if owner == 0:
                units.append((uid, x, y))

        if enemy_hq is None:
            print("WAIT")
            sys.stdout.flush()
            continue

        # Distance (en nombre de cases non-void) jusqu'au QG adverse.
        dist = [[INF] * W for _ in range(H)]
        ex, ey = enemy_hq
        dist[ey][ex] = 0
        q = deque([(ex, ey)])
        while q:
            x, y = q.popleft()
            nd = dist[y][x] + 1
            for dx, dy in DIRS:
                nx, ny = x + dx, y + dy
                if 0 <= nx < W and 0 <= ny < H and grid[ny][nx] != '#':
                    if dist[ny][nx] == INF:
                        dist[ny][nx] = nd
                        q.append((nx, ny))

        actions = [f"MOVE {uid} {ex} {ey}" for uid, _x, _y in units]

        # Cases où l'on peut entraîner : à nous active, ou voisine d'une telle case.
        own_unit_cells = {(x, y) for _uid, x, y in units}
        candidates = []
        for y in range(H):
            for x in range(W):
                if grid[y][x] == '#':
                    continue
                playable = grid[y][x] == 'O'
                if not playable:
                    for dx, dy in DIRS:
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < W and 0 <= ny < H and grid[ny][nx] == 'O':
                            playable = True
                            break
                if not playable:
                    continue
                if my_hq is not None and (x, y) == my_hq:
                    continue
                if dist[y][x] == INF:
                    continue
                # On garde les cases occupées par nos unités : les MOVE les libèrent.
                candidates.append((dist[y][x], x, y, (x, y) in own_unit_cells))

        candidates.sort()
        for _d, x, y, _occupied in candidates[: gold // 10]:
            actions.append(f"TRAIN 1 {x} {y}")

        print(";".join(actions) if actions else "WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
