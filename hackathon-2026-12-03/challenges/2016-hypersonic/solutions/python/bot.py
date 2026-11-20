#!/usr/bin/env python3
"""Hypersonic Wood — BFS vers les boîtes, bombes sur cibles rentables."""
import sys
from collections import deque

RANGE = 3


def blast_boxes(grid: list[str], x: int, y: int, height: int, width: int) -> int:
    """Nombre de boîtes détruites par une bombe posée en (x,y)."""
    total = 0
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        for step in range(1, RANGE + 1):
            nx, ny = x + dx * step, y + dy * step
            if not (0 <= nx < width and 0 <= ny < height):
                break
            if grid[ny][nx] == "0":
                total += 1
                break
    return total


def bfs_next(grid: list[str], sx: int, sy: int, blocked: set[tuple[int, int]],
             height: int, width: int, mode: str) -> tuple[int, int] | None:
    """Premier pas BFS. mode='box' : case adjacente à une boîte ;
    mode='center' : centre ; mode='flee' : case hors de portée 3 du départ."""
    q = deque([(sx, sy)])
    parent: dict[tuple[int, int], tuple[int, int] | None] = {(sx, sy): None}
    target: tuple[int, int] | None = None
    center = (width // 2, height // 2)
    while q:
        x, y = q.popleft()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if not (0 <= nx < width and 0 <= ny < height):
                continue
            if (nx, ny) in parent or (nx, ny) in blocked or grid[ny][nx] == "0":
                continue
            parent[(nx, ny)] = (x, y)
            if mode == "box":
                adjacent_box = any(
                    0 <= nx + ax < width and 0 <= ny + ay < height
                    and grid[ny + ay][nx + ax] == "0"
                    for ax, ay in ((1, 0), (-1, 0), (0, 1), (0, -1))
                )
                done = adjacent_box
            elif mode == "center":
                done = (nx, ny) == center
            else:  # flee
                done = abs(nx - sx) + abs(ny - sy) > 3
            if done:
                target = (nx, ny)
                q.clear()
                break
            q.append((nx, ny))
        if target:
            break
    if target is None:
        return None
    cur = target
    while parent[cur] != (sx, sy) and parent[cur] is not None:
        cur = parent[cur]
    return cur


def play(grid: list[str], me: tuple[int, int], bombs: int,
         bomb_positions: set[tuple[int, int]]) -> str:
    height, width = len(grid), len(grid[0])
    x, y = me
    if bombs > 0 and blast_boxes(grid, x, y, height, width) > 0:
        # Fuite : premier pas hors de la portée de la bombe qu'on pose.
        best = bfs_next(grid, x, y, bomb_positions | {(x, y)}, height, width, "flee")
        if best is None:
            best = (x, y)
        return f"BOMB {best[0]} {best[1]}"
    nxt = bfs_next(grid, x, y, bomb_positions, height, width, "box")
    if nxt is None:
        nxt = bfs_next(grid, x, y, bomb_positions, height, width, "center")
    if nxt is None:
        nxt = (width // 2, height // 2)
    return f"MOVE {nxt[0]} {nxt[1]}"


def main() -> None:
    header = sys.stdin.readline().split()
    if not header:
        return
    width, height, my_id = map(int, header)
    while True:
        grid = [sys.stdin.readline().rstrip("\n") for _ in range(height)]
        if not grid[0]:
            return
        line = sys.stdin.readline()
        if not line:
            return
        entities = int(line)
        me: tuple[int, int] | None = None
        bombs = 0
        bomb_positions: set[tuple[int, int]] = set()
        for _ in range(entities):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            kind, owner, x, y, param1, _param2 = map(int, parts)
            if kind == 0 and owner == my_id:
                me = (x, y)
                bombs = param1
            elif kind == 1:
                bomb_positions.add((x, y))
        if me is None:
            print(f"MOVE {width // 2} {height // 2}", flush=True)
            continue
        print(play(grid, me, bombs, bomb_positions), flush=True)


if __name__ == "__main__":
    main()
