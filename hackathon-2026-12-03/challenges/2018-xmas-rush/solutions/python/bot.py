#!/usr/bin/env python3
"""Xmas Rush Bronze — poussées locales, distance de Manhattan atteignable, anti-stale."""
import sys
from collections import deque

N = 7
DX = (0, 1, 0, -1)
DY = (-1, 0, 1, 0)
DIRS = ("UP", "RIGHT", "DOWN", "LEFT")


def connected(grid, x, y, d):
    if not (grid[y][x] >> d) & 1:
        return False
    nx, ny = x + DX[d], y + DY[d]
    return 0 <= nx < N and 0 <= ny < N and bool((grid[ny][nx] >> ((d + 2) % 4)) & 1)


def bfs_dist(grid, sx, sy, tx, ty):
    if (sx, sy) == (tx, ty):
        return 0
    seen = {(sx, sy)}
    q = deque([(sx, sy, 0)])
    while q:
        x, y, steps = q.popleft()
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if (nx, ny) == (tx, ty):
                return steps + 1
            if (nx, ny) not in seen:
                seen.add((nx, ny))
                q.append((nx, ny, steps + 1))
    return 999


def multi_bfs(grid, px, py, targets):
    """Meilleur chemin de <=20 pas collectant le plus de quêtes."""
    if not targets:
        return 0, [], 999
    index = {p: i for i, p in enumerate(targets)}
    start = (px, py, 0)
    parent = {start: None}
    q = deque([(px, py, 0, 0)])
    best = (0, 0, start, 999)  # collected, steps, state, best_manhattan
    def manhattan(x, y):
        return min(abs(x - tx) + abs(y - ty) for tx, ty in targets)
    best_md = manhattan(px, py)
    while q:
        x, y, mask, steps = q.popleft()
        md = manhattan(x, y)
        got = mask.bit_count()
        # Priorité : quêtes collectées, puis distance de Manhattan minimale.
        if (got, -md, -steps) > (best[0], -best_md, -best[1]):
            best = (got, steps, (x, y, mask), md)
            best_md = md
        if steps >= 20:
            continue
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            nm = mask
            idx = index.get((nx, ny))
            if idx is not None:
                nm |= 1 << idx
            st = (nx, ny, nm)
            if st not in parent:
                parent[st] = ((x, y, mask), d)
                q.append((nx, ny, nm, steps + 1))
    moves = []
    st = best[2]
    while parent[st] is not None:
        prev, d = parent[st]
        moves.append(DIRS[d])
        st = prev
    moves.reverse()
    return best[0], moves, best[3]


def reachable_manhattan(grid, px, py, targets):
    """Distance de Manhattan minimale atteignable en <=20 pas (pour évaluer une poussée)."""
    if not targets:
        return 0
    best = min(abs(px - tx) + abs(py - ty) for tx, ty in targets)
    if best == 0:
        return 0
    seen = {(px, py)}
    q = deque([(px, py, 0)])
    while q:
        x, y, steps = q.popleft()
        if steps >= 20:
            continue
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if (nx, ny) in seen:
                continue
            seen.add((nx, ny))
            md = min(abs(nx - tx) + abs(ny - ty) for tx, ty in targets)
            if md < best:
                best = md
            if best == 0:
                return 0
            q.append((nx, ny, steps + 1))
    return best


def target_path(grid, px, py, targets):
    """Chemin BFS vers la quête la plus proche (repli quand multi_bfs stagne)."""
    best = None
    for tx, ty in targets:
        d = bfs_dist(grid, px, py, tx, ty)
        if d < 999 and (best is None or d < best[0]):
            best = (d, tx, ty)
    if best is None or best[0] == 0:
        return []
    _d, tx, ty = best
    q = deque([(px, py, [])])
    seen = {(px, py)}
    while q:
        x, y, moves = q.popleft()
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if (nx, ny) in seen:
                continue
            nm = moves + [DIRS[d]]
            if (nx, ny) == (tx, ty):
                return nm[:20]
            seen.add((nx, ny))
            q.append((nx, ny, nm))
    return []


def edge_path(grid, px, py):
    goals = [(x, y) for x in range(N) for y in (0, N - 1)]
    goals += [(x, y) for y in range(N) for x in (0, N - 1)]
    best = None
    for tx, ty in goals:
        d = bfs_dist(grid, px, py, tx, ty)
        if d < 999 and (best is None or d < best[0]):
            best = (d, tx, ty)
    if best is None or best[0] == 0:
        return []
    _d, tx, ty = best
    q = deque([(px, py, [])])
    seen = {(px, py)}
    while q:
        x, y, moves = q.popleft()
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if (nx, ny) in seen:
                continue
            nm = moves + [DIRS[d]]
            if (nx, ny) == (tx, ty):
                return nm[:20]
            seen.add((nx, ny))
            q.append((nx, ny, nm))
    return []


def simulate(grid, tile, px, py, board_targets, hand_count, line_id, d):
    g = [row[:] for row in grid]
    horizontal = d in (1, 3)
    if d == 1:
        ins, pop = (0, line_id), (N - 1, line_id)
        for x in range(N - 1, 0, -1):
            g[x][line_id] = g[x - 1][line_id]
    elif d == 3:
        ins, pop = (N - 1, line_id), (0, line_id)
        for x in range(N - 1):
            g[x][line_id] = g[x + 1][line_id]
    elif d == 0:
        ins, pop = (line_id, N - 1), (line_id, 0)
        for y in range(N - 1):
            g[line_id][y] = g[line_id][y + 1]
    else:
        ins, pop = (line_id, 0), (line_id, N - 1)
        for y in range(N - 1, 0, -1):
            g[line_id][y] = g[line_id][y - 1]
    g[ins[1]][ins[0]] = tile
    on_line = (py == line_id) if horizontal else (px == line_id)
    npx, npy = ((px + DX[d]) % N, (py + DY[d]) % N) if on_line else (px, py)
    nboard = []
    popped = 0
    for pos in board_targets:
        if pos == pop:
            popped += 1
        elif (horizontal and pos[1] == line_id) or (not horizontal and pos[0] == line_id):
            nboard.append(((pos[0] + DX[d]) % N, (pos[1] + DY[d]) % N))
        else:
            nboard.append(pos)
    instant = hand_count if on_line else 0
    if hand_count and not on_line:
        nboard.append(ins)
    return g, npx, npy, nboard, popped, instant


def read_input():
    first = sys.stdin.readline()
    if not first:
        return None
    turn_type = int(first.strip())
    grid = []
    for _ in range(N):
        row = []
        for s in sys.stdin.readline().strip().split():
            mask = 0
            for d, c in enumerate("URDL"):
                if s[d] == "1":
                    mask |= 1 << d
            row.append(mask)
        grid.append(row)
    px = py = 0
    tile = 0
    for i in range(2):
        f = sys.stdin.readline().split()
        if i == 0:
            px, py = int(f[1]), int(f[2])
            for d, c in enumerate("URDL"):
                if f[3][d] == "1":
                    tile |= 1 << d
    items = {}
    for _ in range(int(sys.stdin.readline())):
        f = sys.stdin.readline().split()
        x, y, pid = int(f[1]), int(f[2]), int(f[3])
        if pid == 0:
            items[f[0]] = None if x == -1 else ((-2, -2) if x == -2 else (x, y))
    quests = []
    for _ in range(int(sys.stdin.readline())):
        f = sys.stdin.readline().split()
        if int(f[1]) == 0:
            quests.append(f[0])
    board_targets = []
    hand_count = 0
    for name in quests:
        pos = items.get(name)
        if pos is None:
            hand_count += 1
        elif pos != (-2, -2):
            board_targets.append(pos)
    return turn_type, grid, px, py, tile, board_targets, hand_count


def best_push(grid, px, py, tile, targets, hand, last_push, stale):
    candidates = []
    # Poussées locales : lignes proches du joueur.
    lines_y = {py - 1, py, py + 1}
    lines_x = {px - 1, px, px + 1}
    for horizontal in (True, False):
        for line_id in range(N):
            if horizontal and line_id not in lines_y:
                continue
            if not horizontal and line_id not in lines_x:
                continue
            for d in ((1, 3) if horizontal else (0, 2)):
                g, nx, ny, ts, popped, instant = simulate(
                    grid, tile, px, py, targets, hand, line_id, d)
                md = reachable_manhattan(g, nx, ny, ts) if ts else 0
                dists = [bfs_dist(g, nx, ny, t[0], t[1]) for t in ts]
                collectable = sum(1 for x in dists if x <= 20)
                conn = sum(connected(g, nx, ny, k) for k in range(4))
                cmd = f"PUSH {line_id} {DIRS[d]}"
                score = (instant, -popped, -md, collectable, conn,
                         -hand, -line_id, -d)
                candidates.append((score, cmd))
    candidates.sort(reverse=True)
    if stale >= 3 and len(candidates) > 1:
        for i, (_score, cmd) in enumerate(candidates):
            if cmd != last_push:
                return cmd
    return candidates[0][1]


def main():
    last_push = None
    stale = 0
    while True:
        data = read_input()
        if data is None:
            return
        turn_type, grid, px, py, tile, targets, hand = data
        if turn_type == 1:
            moves = []
            if targets:
                _got, moves, _md = multi_bfs(grid, px, py, targets)
            if not moves and targets:
                moves = target_path(grid, px, py, targets)
            if not moves and hand:
                moves = edge_path(grid, px, py)
            print("MOVE " + " ".join(moves) if moves else "PASS", flush=True)
        else:
            cmd = best_push(grid, px, py, tile, targets, hand, last_push, stale)
            if cmd == last_push:
                stale += 1
            else:
                stale = 0
                last_push = cmd
            print(cmd, flush=True)


if __name__ == "__main__":
    main()
