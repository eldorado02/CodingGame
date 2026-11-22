#!/usr/bin/env python3
"""Elemental Wars — Wood 2+ : commande tous les élémentaux de sa tribu.

Chaque ennemi non-prisonnier fournit une carte BFS de distances.  Pour chaque
élémental allié, on évalue rester + 4 voisins : on poursuit la proie, on fuit
le prédateur, avec les mêmes scores et le même départage que la version C++.
"""

import sys
from collections import deque

DX = (0, 1, 0, -1)
DY = (-1, 0, 1, 0)


def prey_of(tribe):
    return 'F' if tribe == 'W' else 'P' if tribe == 'F' else 'W'


def pred_of(tribe):
    return 'P' if tribe == 'W' else 'W' if tribe == 'F' else 'F'


def bfs(grid, n, sx, sy):
    """Distances BFS depuis (sx, sy), -1 si mur/inaccessible."""
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
            if 0 <= nx < n and 0 <= ny < n and grid[ny][nx] != '#' and dist[ny][nx] < 0:
                dist[ny][nx] = nd
                q.append((nx, ny))
    return dist


def tokens(stream):
    """Lecteur paresseux, équivalent à `cin >> ...` (séparateurs = espaces)."""
    for line in stream:
        for tok in line.split():
            yield tok


def main():
    ts = tokens(sys.stdin)

    def nxt():
        return next(ts, None)

    tok = nxt()
    if tok is None:
        return
    me = tok[0]
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

    while True:
        # Les trois scores sont lus puis ignorés, comme dans le C++.
        raw = [nxt() for _ in range(3)]
        if any(v is None for v in raw):
            break
        _sw, _sf, _sp = (int(v) for v in raw)

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

        # Une carte BFS par ennemi non-prisonnier, dans l'ordre d'apparition.
        prey_dist = []
        pred_dist = []
        for _uid, tribe, x, y, prisoner in els:
            if prisoner or tribe == me:
                continue
            d = bfs(grid, n, x, y)
            if tribe == prey:
                prey_dist.append(d)
            elif tribe == pred:
                pred_dist.append(d)

        commands = []
        for uid, tribe, x, y, prisoner in els:
            if tribe != me or prisoner:
                continue
            bx, by = x, y
            best = -1000000000
            # k = -1 : rester ; puis nord, est, sud, ouest.
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

                score = -100 * (dp if dp < 20 else 20) + 15 * (dq if dq < 8 else 8)
                if dp == 0:
                    score += 10000
                if dq == 0:
                    score -= 12000
                if dq <= 1 and dp > 1:
                    score -= 2000

                if score > best or (score == best and
                                    (cx < bx or (cx == bx and cy < by))):
                    best = score
                    bx, by = cx, cy

            if bx == x and by == y:
                commands.append(f'{uid} WAIT')
            else:
                commands.append(f'{uid} MOVE {bx} {by}')

        print(';'.join(commands) if commands else 'MSG', flush=True)


if __name__ == '__main__':
    main()
