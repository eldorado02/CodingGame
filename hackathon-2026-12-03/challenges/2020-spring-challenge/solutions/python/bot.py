#!/usr/bin/env python3
"""Spring Challenge 2020 (Wood 4) — BFS + cible pellet, évitement de collision."""
import sys
from collections import deque

W = H = 0
GRID: list[str] = []
DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def raw() -> str:
    line = sys.stdin.readline()
    return line[:-1] if line.endswith('\n') else line


def rd() -> str:
    return raw().strip()


def wrap_x(x: int) -> int:
    return x % W


def bfs(sx: int, sy: int):
    """Distances et prédécesseurs depuis (sx,sy), avec wrap horizontal."""
    dist = [[-1] * W for _ in range(H)]
    prev: list[list[tuple[int, int] | None]] = [[None] * W for _ in range(H)]
    dist[sy][sx] = 0
    q = deque([(sx, sy)])
    while q:
        x, y = q.popleft()
        for dx, dy in DIRS:
            nx, ny = wrap_x(x + dx), y + dy
            if nx < 0 or ny < 0 or ny >= H or GRID[ny][nx] == '#' or dist[ny][nx] != -1:
                continue
            dist[ny][nx] = dist[y][x] + 1
            prev[ny][nx] = (x, y)
            q.append((nx, ny))
    return dist, prev


def first_step(prev, sx: int, sy: int, tx: int, ty: int) -> tuple[int, int] | None:
    """Premier pas du chemin issu du BFS depuis (sx,sy)."""
    node = (tx, ty)
    start = (sx, sy)
    if node == start:
        return None
    while True:
        p = prev[node[1]][node[0]]
        if p is None:
            return None
        if p == start:
            return node
        node = p


def main() -> None:
    global W, H, GRID
    head = rd().split()
    if len(head) < 2:
        return
    W, H = int(head[0]), int(head[1])
    GRID = [row.ljust(W) for row in (raw() for _ in range(H))]
    target: tuple[int, int] | None = None

    while True:
        line = rd()
        if not line or len(line.split()) < 2:
            break
        n_pacs = int(rd())
        my_pos = None
        opp_pos = None
        for _ in range(n_pacs):
            f = rd().split()
            if len(f) < 4:
                break
            pac_id, mine, x, y = int(f[0]), int(f[1]), int(f[2]), int(f[3])
            if mine:
                my_pos = (x, y)
            elif opp_pos is None or pac_id == 0:
                opp_pos = (x, y)
        n_pellets = int(rd())
        pellets: list[tuple[int, int, int]] = []
        for _ in range(n_pellets):
            f = rd().split()
            if len(f) < 3:
                break
            pellets.append((int(f[0]), int(f[1]), int(f[2])))
        if my_pos is None:
            print("MOVE 0 0 0")
            sys.stdout.flush()
            continue

        sx, sy = my_pos
        my_dist, my_prev = bfs(sx, sy)
        candidates = [(tx, ty, v) for tx, ty, v in pellets
                      if 0 <= ty < H and 0 <= tx < W and my_dist[ty][tx] >= 0]
        pellet_set = {(tx, ty) for tx, ty, _v in candidates}

        if target is not None and (target == my_pos or target not in pellet_set):
            target = None
        if target is not None:
            tx, ty = target
            print(f"MOVE 0 {tx} {ty}")
            sys.stdout.flush()
            continue
        if not candidates:
            print(f"MOVE 0 {sx} {sy}")
            sys.stdout.flush()
            continue

        opp_step = None
        if opp_pos is not None:
            ox, oy = opp_pos
            if 0 <= oy < H and 0 <= ox < W:
                od, oprev = bfs(ox, oy)
                best_opp = None
                for tx, ty, value in candidates:
                    if od[ty][tx] < 0:
                        continue
                    key = (od[ty][tx], -value, tx, ty)
                    if best_opp is None or key < best_opp[0]:
                        best_opp = (key, (tx, ty))
                if best_opp is not None:
                    opp_step = first_step(oprev, ox, oy, *best_opp[1])

        ranked = []
        for tx, ty, value in candidates:
            s = first_step(my_prev, sx, sy, tx, ty)
            if s is None:
                continue
            penalty = 1 if opp_step is not None and s == opp_step else 0
            d = my_dist[ty][tx]
            key = (penalty, d * 10 // value, -value, d, tx, ty)
            ranked.append((key, tx, ty))
        if not ranked:
            print(f"MOVE 0 {sx} {sy}")
            sys.stdout.flush()
            continue
        ranked.sort(key=lambda c: c[0])
        _key, tx, ty = ranked[0]
        target = (tx, ty)
        print(f"MOVE 0 {tx} {ty}")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
