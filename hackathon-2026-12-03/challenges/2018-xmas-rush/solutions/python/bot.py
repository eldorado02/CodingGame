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


def dist_field(grid, tx, ty):
    """Distance BFS exacte de chaque case à (tx,ty)."""
    dist = [[999] * N for _ in range(N)]
    dist[ty][tx] = 0
    q = deque([(tx, ty)])
    while q:
        x, y = q.popleft()
        nd = dist[y][x] + 1
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if dist[ny][nx] == 999:
                dist[ny][nx] = nd
                q.append((nx, ny))
    return dist


def multi_bfs(grid, px, py, targets):
    """Meilleur chemin de <=20 pas collectant le plus de quêtes (distance BFS exacte)."""
    if not targets:
        return 0, [], 999
    index = {p: i for i, p in enumerate(targets)}
    fields = [dist_field(grid, tx, ty) for tx, ty in targets]
    def exact(x, y):
        return min(f[y][x] for f in fields)
    start = (px, py, 0)
    parent = {start: None}
    q = deque([(px, py, 0, 0)])
    best = (0, 0, start, 999)  # collected, steps, state, exact distance
    best_d = exact(px, py)
    while q:
        x, y, mask, steps = q.popleft()
        got = mask.bit_count()
        md = exact(x, y) if got < len(targets) else 0
        if (got, -md, -steps) > (best[0], -best[3], -best[1]):
            best = (got, steps, (x, y, mask), md)
            best_d = md
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
    return best[0], moves, best_d


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
            g[line_id][x] = g[line_id][x - 1]
    elif d == 3:
        ins, pop = (N - 1, line_id), (0, line_id)
        for x in range(N - 1):
            g[line_id][x] = g[line_id][x + 1]
    elif d == 0:
        ins, pop = (line_id, N - 1), (line_id, 0)
        for y in range(N - 1):
            g[y][line_id] = g[y + 1][line_id]
    else:
        ins, pop = (line_id, 0), (line_id, N - 1)
        for y in range(N - 1, 0, -1):
            g[y][line_id] = g[y - 1][line_id]
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
    instant = hand_count if (on_line and (npx, npy) == ins) else 0
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
        for tok in sys.stdin.readline().strip().split():
            mask = 0
            for d, c in enumerate("URDL"):
                if tok[d] == "1":
                    mask |= 1 << d
            row.append(mask)
        grid.append(row)
    players = []
    for _ in range(2):
        f = sys.stdin.readline().split()
        if len(f) < 4:
            return None
        mask = 0
        for d, c in enumerate("URDL"):
            if f[3][d] == "1":
                mask |= 1 << d
        players.append((int(f[1]), int(f[2]), mask))
    px, py, tile = players[0]
    ox, oy, _ = players[1]
    items = {0: [], 1: []}
    for _ in range(int(sys.stdin.readline())):
        f = sys.stdin.readline().split()
        if len(f) < 4:
            return None
        x, y, pid = int(f[1]), int(f[2]), int(f[3])
        if x == -1:
            pos = None
        elif x == -2:
            pos = (-2, -2)
        else:
            pos = (x, y)
        items.setdefault(pid, []).append((f[0], pos))
    quests = {0: [], 1: []}
    for _ in range(int(sys.stdin.readline())):
        f = sys.stdin.readline().split()
        if len(f) >= 2:
            quests.setdefault(int(f[1]), []).append(f[0])
    qpos = {name: pos for name, pos in items.get(0, []) + items.get(1, [])}
    board_targets = []
    opp_targets = []
    hand = 0
    our_all = []
    opp_all = []
    for pid, lst, tgts in ((0, items.get(0, []), board_targets),
                           (1, items.get(1, []), opp_targets)):
        for name, pos in lst:
            if pos is None:
                if pid == 0:
                    hand += 1
            elif pos != (-2, -2):
                if pid == 0:
                    our_all.append(pos)
                else:
                    opp_all.append(pos)
                if name in quests.get(pid, []):
                    tgts.append(pos)
    return (turn_type, grid, px, py, tile, ox, oy,
            board_targets, hand, our_all, opp_all, opp_targets)


def reachable_count(grid, sx, sy):
    seen = {(sx, sy)}
    q = deque([(sx, sy)])
    while q:
        x, y = q.popleft()
        for d in range(4):
            if not connected(grid, x, y, d):
                continue
            nx, ny = x + DX[d], y + DY[d]
            if (nx, ny) not in seen:
                seen.add((nx, ny))
                q.append((nx, ny))
    return len(seen)


def parse_line(cmd):
    if not cmd:
        return None
    parts = cmd.split()
    if len(parts) != 3:
        return None
    return int(parts[1]), parts[2] in ("RIGHT", "LEFT")


def best_push(grid, px, py, tile, our_all, opp_all, ox, oy, hand, last_push, stale,
              board_targets, opp_targets):
    candidates = []
    all_lines = [(True, line, d) for line in range(N) for d in (1, 3)]
    all_lines += [(False, line, d) for line in range(N) for d in (0, 2)]
    last_line = parse_line(last_push)
    if last_line is not None:
        all_lines = [c for c in all_lines if (c[1], c[0]) != last_line]
    for horizontal, line_id, d in all_lines:
        g, nx, ny, ts, popped, instant = simulate(
            grid, tile, px, py, board_targets, hand, line_id, d)
        _, nox, noy, ots, _, _ = simulate(
            grid, tile, ox, oy, opp_targets, 0, line_id, d)
        score = 1000 * instant - 100 * popped
        for t in ts:
            dist = bfs_dist(g, nx, ny, t[0], t[1])
            if dist <= 20:
                score += 1000 - 10 * dist
            elif dist < 999:
                score += 200.0 / (dist + 1)
            else:
                md = abs(nx - t[0]) + abs(ny - t[1])
                score += 500.0 - 30.0 * md
        score += 2 * reachable_count(g, nx, ny)
        #Les quêtes adverse portent sur d'autres objets : les bloquer ne
        #rapporte rien. On garde une legsère pénalité symbolique.
        for t in ots:
            dist = bfs_dist(g, nox, noy, t[0], t[1])
            if dist <= 20:
                score -= 10
        #Accès aux objets encore cachés : ils deviendront des quêtes.
        for t in our_all:
            if t in board_targets:
                continue
            dist = bfs_dist(g, nx, ny, t[0], t[1])
            if dist <= 20:
                score += 20
            elif dist < 999:
                score += 20.0 / (dist + 1)
        conn = sum(connected(g, nx, ny, k) for k in range(4))
        score += conn
        cmd = "PUSH %d %s" % (line_id, DIRS[d])
        candidates.append((score, instant, -popped, conn, -line_id, -d, cmd))
    candidates.sort(reverse=True)
    if stale >= 3 and len(candidates) > 1:
        for cand in candidates:
            if cand[-1] != last_push:
                return cand[-1]
    return candidates[0][-1]


def main():
    last_push = None
    stale = 0
    while True:
        data = read_input()
        if data is None:
            return
        (turn_type, grid, px, py, tile, ox, oy,
         board_targets, hand, our_all, opp_all, opp_targets) = data
        if turn_type == 1:
            moves = []
            if board_targets:
                _got, moves, _md = multi_bfs(grid, px, py, board_targets)
            if not moves and board_targets:
                moves = target_path(grid, px, py, board_targets)
            if not moves and hand:
                moves = edge_path(grid, px, py)
            print("MOVE " + " ".join(moves) if moves else "PASS", flush=True)
        else:
            cmd = best_push(grid, px, py, tile, our_all, opp_all, ox, oy,
                            hand, last_push, stale, board_targets, opp_targets)
            if cmd == last_push:
                stale += 1
            else:
                stale = 0
            last_push = cmd
            print(cmd, flush=True)


if __name__ == "__main__":
    main()
