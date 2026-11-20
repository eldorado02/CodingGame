#!/usr/bin/env python3
"""Xmas Rush — BFS sur les tuiles, poussée qui minimise la distance à la quête."""
import sys
from collections import deque

N = 7
DX = [0, 1, 0, -1]
DY = [-1, 0, 1, 0]
DIRS = ("UP", "RIGHT", "DOWN", "LEFT")


def line() -> str:
    return sys.stdin.readline().strip()


def parse(turn: int) -> tuple[list[list[int]], int, int, int, tuple[int, int] | None]:
    grid = []
    for _ in range(N):
        cells = line().split()
        row = []
        for s in cells:
            mask = 0
            for d, c in enumerate("URDL"):
                if s[d] == "1":
                    mask |= 1 << d
            row.append(mask)
        grid.append(row)
    # Deux lignes joueurs : nbCartes x y tuile
    px = py = 0
    tile = 0
    for i in range(2):
        f = line().split()
        if i == 0:
            px, py = int(f[1]), int(f[2])
            tile = 0
            for d, c in enumerate("URDL"):
                if f[3][d] == "1":
                    tile |= 1 << d
    nitems = int(line())
    items = {}
    for _ in range(nitems):
        f = line().split()
        ix, iy, pid = int(f[1]), int(f[2]), int(f[3])
        if pid == 0:
            items[f[0]] = None if ix < 0 else (ix, iy)
    nquests = int(line())
    target = None
    for _ in range(nquests):
        f = line().split()
        if int(f[1]) == 0 and f[0] in items:
            target = items[f[0]]
    return grid, px, py, tile, target


def bfs(grid: list[list[int]], sx: int, sy: int, tx: int, ty: int) -> list[int] | None:
    if (sx, sy) == (tx, ty):
        return []
    prev: dict[tuple[int, int], tuple[tuple[int, int], int]] = {}
    q = deque([(sx, sy)])
    prev[(sx, sy)] = ((sx, sy), -1)
    while q:
        x, y = q.popleft()
        for d in range(4):
            if not (grid[y][x] >> d) & 1:
                continue
            nx, ny = x + DX[d], y + DY[d]
            if not (0 <= nx < N and 0 <= ny < N):
                continue
            if not (grid[ny][nx] >> ((d + 2) % 4)) & 1:
                continue
            if (nx, ny) in prev:
                continue
            prev[(nx, ny)] = ((x, y), d)
            if (nx, ny) == (tx, ty):
                path = []
                cur = (nx, ny)
                while prev[cur][0] != cur:
                    p, dd = prev[cur]
                    path.append(dd)
                    cur = p
                path.reverse()
                return path
            q.append((nx, ny))
    return None


def shift(x: int, y: int, dx: int, dy: int) -> tuple[int, int]:
    return (x + dx) % N, (y + dy) % N


def simulate(grid, tile, px, py, item, line, d):
    """Applique une poussée ; item = (x,y) ou None si sur notre tuile."""
    g = [row[:] for row in grid]
    dx, dy = DX[d], DY[d]
    horizontal = d in (1, 3)
    idx = py if horizontal else px
    if idx != line:
        # Poussée hors ligne : joueur et item immobiles, plateau modifié.
        pass
    # Insertion / pop
    if d == 1:      # RIGHT : insertion (0,line), pop (6,line)
        ins, pop = (0, line), (N - 1, line)
        for x in range(N - 1, 0, -1):
            g[x][line] = g[x - 1][line]
    elif d == 3:    # LEFT : insertion (6,line), pop (0,line)
        ins, pop = (N - 1, line), (0, line)
        for x in range(N - 1):
            g[x][line] = g[x + 1][line]
    elif d == 0:    # UP : insertion (col,6), pop (col,0)
        ins, pop = (line, N - 1), (line, 0)
        for y in range(N - 1):
            g[line][y] = g[line][y + 1]
    else:           # DOWN : insertion (col,0), pop (col,6)
        ins, pop = (line, 0), (line, N - 1)
        for y in range(N - 1, 0, -1):
            g[line][y] = g[line][y - 1]
    g[ins[1]][ins[0]] = tile
    nitem = item
    if item is None:
        nitem = ins
    elif item == pop:
        nitem = None
    elif (horizontal and item[1] == line) or (not horizontal and item[0] == line):
        nitem = shift(*item, dx, dy)
    npx, npy = px, py
    if (horizontal and py == line) or (not horizontal and px == line):
        npx, npy = shift(px, py, dx, dy)
    return g, npx, npy, nitem


def solve(turn: int) -> None:
    grid, px, py, tile, item = parse(turn)
    if turn == 1:
        if item is None or item[0] < 0:
            print("PASS")
            return
        path = bfs(grid, px, py, item[0], item[1])
        print("MOVE" + "".join(" " + DIRS[d] for d in path[:20]) if path else "PASS")
        return

    best = None
    best_move = None
    for horizontal in (True, False):
        count = N if horizontal else N
        for line in range(count):
            for d in ((1, 3) if horizontal else (0, 2)):
                g, nx, ny, ni = simulate(grid, tile, px, py, item, line, d)
                if ni is None or ni[0] < 0:
                    dist = 1000
                elif ni == (nx, ny):
                    dist = -1
                else:
                    path = bfs(g, nx, ny, ni[0], ni[1])
                    dist = len(path) if path is not None else 100
                key = (dist, line, d)
                if best is None or key < best:
                    best = key
                    best_move = (line, d)
    line, d = best_move
    print(f"PUSH {line} {DIRS[d]}")


if __name__ == "__main__":
    # Le processus reste vivant pendant toute la partie : un tour par itération.
    while True:
        first = line()
        if not first:
            break
        solve(int(first))
        sys.stdout.flush()
