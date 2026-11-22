#!/usr/bin/env python3
"""Elemental Wars — un élémental : fuir le prédateur, poursuivre la proie."""
import sys
from collections import deque

PREY = {'W': 'F', 'F': 'P', 'P': 'W'}
PRED = {'W': 'P', 'F': 'W', 'P': 'F'}
DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))  # nord, est, sud, ouest


def bfs_dist(grid, sx, sy):
    n = len(grid)
    dist = [[-1] * n for _ in range(n)]
    if not (0 <= sx < n and 0 <= sy < n) or grid[sy][sx] == '#':
        return dist
    dist[sy][sx] = 0
    q = deque([(sx, sy)])
    while q:
        x, y = q.popleft()
        for dx, dy in DIRS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < n and 0 <= ny < n and grid[ny][nx] != '#' and dist[ny][nx] < 0:
                dist[ny][nx] = dist[y][x] + 1
                q.append((nx, ny))
    return dist


def decide(tribe, grid, mine, prey, pred):
    n = len(grid)
    my_id, mx, my = mine
    px, py = prey
    qx, qy = pred
    dprey = bfs_dist(grid, px, py)
    dpred = bfs_dist(grid, qx, qy)
    options = [(mx, my)]
    for dx, dy in DIRS:
        nx, ny = mx + dx, my + dy
        if 0 <= nx < n and 0 <= ny < n and grid[ny][nx] != '#':
            options.append((nx, ny))
    best = (mx, my)
    best_key = None
    for x, y in options:
        dp = dprey[y][x]
        dq = dpred[y][x]
        if dp < 0:
            dp = 100
        if dq < 0:
            dq = 100
        score = -100 * dp + 20 * min(dq, 6)
        if x == px and y == py and not (x == qx and y == qy):
            score += 10000
        if x == qx and y == qy:
            score -= 10000
        if dq <= 1 and dp > 1:
            score -= 2000
        key = (score, -x, -y)
        if best_key is None or key > best_key:
            best_key, best = key, (x, y)
    return f'{my_id} MOVE {best[0]} {best[1]}'


def main():
    tribe = input().strip()
    n = int(input().strip())
    grid = [input().strip() for _ in range(n)]
    while True:
        try:
            line = input()
        except EOFError:
            break
        if not line.strip():
            continue
        _scores = line
        count = int(input().strip())
        units = {}
        for _ in range(count):
            parts = input().split()
            uid = int(parts[0]); t = parts[1]
            x = int(parts[2]); y = int(parts[3]); prisoner = int(parts[4])
            units[t] = (uid, x, y, prisoner)
        if tribe not in units or PREY[tribe] not in units or PRED[tribe] not in units:
            print('0 WAIT', flush=True)
            continue
        mine = units[tribe]
        prey = units[PREY[tribe]]
        pred = units[PRED[tribe]]
        if mine[3] or prey[3]:
            print(f'{mine[0]} WAIT', flush=True)
            continue
        if pred[3]:
            print(decide(tribe, grid, (mine[0], mine[1], mine[2]),
                         (prey[1], prey[2]), (100, 100)), flush=True)
            continue
        print(decide(tribe, grid, (mine[0], mine[1], mine[2]),
                     (prey[1], prey[2]), (pred[1], pred[2])), flush=True)


if __name__ == '__main__':
    main()
