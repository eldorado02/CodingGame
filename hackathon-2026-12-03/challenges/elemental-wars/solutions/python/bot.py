#!/usr/bin/env python3
"""Elemental Wars — predicted opponent moves, interception, evasion, liberation.

Strict port of the v6 C++ bot.  The output must be byte-identical to the C++
version: same BFS neighbour order (N,E,S,W), same enemy prediction, same
scores, same tie-breaking and same command formatting.
"""

import sys
from collections import deque

DX = (0, 1, 0, -1)
DY = (-1, 0, 1, 0)
INF = 1 << 28


def prey_of(tribe):
    return 'F' if tribe == 'W' else 'P' if tribe == 'F' else 'W'


def pred_of(tribe):
    return 'P' if tribe == 'W' else 'W' if tribe == 'F' else 'F'


def bfs(grid, n, sx, sy):
    """Distance map from (sx, sy).  -1 for walls / unreachable cells."""
    dist = [[-1] * n for _ in range(n)]
    if sx < 0 or sx >= n or sy < 0 or sy >= n or grid[sy][sx] == '#':
        return dist
    dist[sy][sx] = 0
    q = deque([(sx, sy)])
    while q:
        x, y = q.popleft()
        nd = dist[y][x] + 1
        for k in range(4):
            nx = x + DX[k]
            ny = y + DY[k]
            if nx < 0 or nx >= n or ny < 0 or ny >= n:
                continue
            if grid[ny][nx] == '#' or dist[ny][nx] >= 0:
                continue
            dist[ny][nx] = nd
            q.append((nx, ny))
    return dist


def tokens(stream):
    """Lazy whitespace-separated tokens, equivalent to C++ `cin >> ...`."""
    for line in stream:
        for tok in line.split():
            yield tok


def main():
    ts = tokens(sys.stdin)

    def nxt():
        return next(ts, None)

    first = nxt()
    if first is None:
        return
    me = first[0]                      # C++ reads a single char
    tok = nxt()
    if tok is None:
        return
    n = int(tok)

    grid = []
    for _ in range(n):
        row = nxt()
        if row is None:
            row = ''
        grid.append([row[x] if x < len(row) else '.' for x in range(n)])

    prey = prey_of(me)
    pred = pred_of(me)

    # The C++ keeps scanning the whole map: the camp is the LAST predator cell.
    camp_x = camp_y = -1
    for y in range(n):
        for x in range(n):
            if grid[y][x] == pred:
                camp_x, camp_y = x, y

    out_stream = sys.stdout
    while True:
        raw = [nxt() for _ in range(3)]
        if any(v is None for v in raw):
            break
        # scoreW scoreF scoreP are read but unused.
        int(raw[0]); int(raw[1]); int(raw[2])

        tok = nxt()
        if tok is None:
            break
        count = int(tok)

        els = []
        incomplete = False
        for _ in range(count):
            parts = [nxt() for _ in range(5)]
            if any(p is None for p in parts):
                incomplete = True
                break
            els.append((int(parts[0]), parts[1][0], int(parts[2]),
                        int(parts[3]), int(parts[4])))
        if incomplete:
            break

        # My free elementals (indices into els) and prisoner count.
        prisoners = 0
        mine = []
        for i, (uid, tribe, x, y, pris) in enumerate(els):
            if tribe != me:
                continue
            if pris:
                prisoners += 1
            else:
                mine.append(i)

        # One BFS map per free enemy, grouped in appearance order.
        prey_dist = []
        pred_dist = []
        for uid, tribe, x, y, pris in els:
            if pris or tribe == me:
                continue
            d = bfs(grid, n, x, y)
            if tribe == prey:
                prey_dist.append(d)
            elif tribe == pred:
                pred_dist.append(d)

        # Predict the next cell of every free enemy: one step towards its
        # nearest free prey (nearest by Manhattan distance, first on ties).
        pred_next = []
        prey_next = []
        for uid, tribe, x, y, pris in els:
            if pris or tribe == me:
                continue
            et = prey_of(tribe)
            tx = ty = -1
            best = INF
            for qid, qtribe, qx, qy, qpris in els:
                if qpris or qtribe != et:
                    continue
                d = abs(qx - x) + abs(qy - y)
                if d < best:
                    best = d
                    tx, ty = qx, qy
            nx, ny = x, y
            if tx >= 0:
                dt = bfs(grid, n, tx, ty)
                cur = dt[y][x]
                if cur > 0:
                    for k in range(4):
                        ax, ay = x + DX[k], y + DY[k]
                        if ax < 0 or ax >= n or ay < 0 or ay >= n:
                            continue
                        if grid[ay][ax] == '#' or dt[ay][ax] != cur - 1:
                            continue
                        nx, ny = ax, ay
                        break
            if tribe == prey:
                prey_next.append((nx, ny))
            else:
                pred_next.append((nx, ny))

        # Which of my elementals goes for the prisoners?
        have_camp = prisoners > 0 and camp_x >= 0
        dcamp = bfs(grid, n, camp_x, camp_y) if have_camp else None
        liberator = -1
        if have_camp:
            best = INF
            for i in mine:
                ex, ey = els[i][2], els[i][3]
                d = dcamp[ey][ex]
                if d >= 0 and d < best:
                    best = d
                    liberator = i
            if best > 14:
                liberator = -1

        assigned = []
        commands = []
        for idx in mine:
            uid, tribe, x, y, pris = els[idx]
            lib = (idx == liberator)
            bx, by = x, y
            best_score = -(1 << 50)
            for k in range(-1, 4):
                cx, cy = x, y
                if k >= 0:
                    cx += DX[k]
                    cy += DY[k]
                if cx < 0 or cx >= n or cy < 0 or cy >= n or grid[cy][cx] == '#':
                    continue

                dp = 100
                dq = 100
                for d in prey_dist:
                    v = d[cy][cx]
                    if 0 <= v < dp:
                        dp = v
                for d in pred_dist:
                    v = d[cy][cx]
                    if 0 <= v < dq:
                        dq = v

                if lib:
                    dc = dcamp[cy][cx] if dcamp[cy][cx] >= 0 else 100
                    score = -100 * (dc if dc < 20 else 20) + 20 * (dq if dq < 8 else 8)
                    if dc == 0:
                        score += 3000 * prisoners
                    if dq == 0:
                        score -= 20000
                    elif dq == 1:
                        score -= 5000
                else:
                    score = -100 * (dp if dp < 20 else 20) + 20 * (dq if dq < 8 else 8)
                    for px, py in prey_next:
                        if px == cx and py == cy:
                            score += 12000
                    for px, py in pred_next:
                        if px == cx and py == cy:
                            score -= 18000
                    if dq == 0:
                        score -= 20000
                    elif dq == 1:
                        score -= 4000

                for ax, ay in assigned:
                    if ax == cx and ay == cy:
                        score -= 5000

                if score > best_score or (score == best_score and
                                          (cx < bx or (cx == bx and cy < by))):
                    best_score = score
                    bx, by = cx, cy

            assigned.append((bx, by))
            if bx == x and by == y:
                commands.append(f'{uid} WAIT')
            else:
                commands.append(f'{uid} MOVE {bx} {by}')

        out_stream.write(';'.join(commands) if commands else 'MSG')
        out_stream.write('\n')
        out_stream.flush()


if __name__ == '__main__':
    main()
