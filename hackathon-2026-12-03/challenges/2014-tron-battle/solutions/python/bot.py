#!/usr/bin/env python3
"""Tron Battle — Voronoi 1-ply + greedy primé.

Pour chaque coup légal, on évalue le territoire gagné par une BFS
multi-source depuis les têtes (le joueur qui atteint une case en premier
la possède, l'égalité est contestée). En fin de partie isolée, on choisit
le premier coup qui maximise le remplissage glouton.
"""
import sys
from collections import deque

W, H = 30, 20
DIRS = (("UP", 0, -1), ("DOWN", 0, 1), ("LEFT", -1, 0), ("RIGHT", 1, 0))
NEI = [[[] for _ in range(H)] for _ in range(W)]
for x in range(W):
    for y in range(H):
        for dx, dy in ((0, -1), (0, 1), (-1, 0), (1, 0)):
            nx, ny = x + dx, y + dy
            if 0 <= nx < W and 0 <= ny < H:
                NEI[x][y].append((nx, ny))


def read_heads(n):
    heads = []
    for _ in range(n):
        parts = sys.stdin.readline().split()
        if not parts:
            return None
        x0, y0, x1, y1 = map(int, parts)
        heads.append(None if x1 < 0 else (x1, y1))
    return heads


def rebuild(grid, heads, tails, trails):
    """Reconstruit les bandes à partir des têtes mémorisées."""
    for row in grid:
        for x in range(W):
            row[x] = False
    for p, head in enumerate(heads):
        if head is None:
            trails[p].clear()
            continue
        trail = trails[p]
        tail = tails[p]
        if tail is not None and tail != head:
            # chemin de la queue à la tête : BFS sur les cases déjà vues
            prev = {tail: None}
            q = deque([tail])
            while q:
                cur = q.popleft()
                if cur == head:
                    break
                for nxt in NEI[cur[0]][cur[1]]:
                    if nxt not in prev and nxt in trail:
                        prev[nxt] = cur
                        q.append(nxt)
            cur = head
            while cur is not None and cur != tail:
                trail.add(cur)
                cur = prev.get(cur)
        trail.add(head)
        for x, y in trail:
            if 0 <= x < W and 0 <= y < H:
                grid[y][x] = True


def voronoi_scores(heads, p, blocked):
    """BFS multi-source dans l'ordre des tours ; retourne (dist_moi, dist_foes)."""
    dm = [[99] * W for _ in range(H)]
    others = [i for i, h in enumerate(heads) if i != p and h is not None]
    ds = [[[99] * W for _ in range(H)] for _ in range(len(others))]
    order = [i for i in range(len(heads)) if heads[i] is not None]
    order.sort(key=lambda i: (i > p, i))  # p joue en premier dans l'évaluation
    for i in order:
        h = heads[i]
        dist = dm if i == p else ds[others.index(i)]
        # La tête est une case occupée, mais c'est légitimement l'origine du BFS.
        q = deque([(h[0], h[1])])
        dist[h[1]][h[0]] = 0
        while q:
            x, y = q.popleft()
            nd = dist[y][x] + 1
            for nx, ny in NEI[x][y]:
                if not blocked[ny][nx] and dist[ny][nx] == 99:
                    dist[ny][nx] = nd
                    q.append((nx, ny))
    return dm, ds


def greedy_fill(start, blocked, limit):
    """Remplissage glouton : choisir le voisin qui retire le moins d'arêtes."""
    local = [row[:] for row in blocked]
    x, y = start
    count = 0
    while count < limit:
        choices = []
        for nx, ny in NEI[x][y]:
            if not local[ny][nx]:
                free_next = sum(1 for a, b in NEI[nx][ny] if not local[b][a])
                choices.append((free_next, -ny, -nx, nx, ny))
        if not choices:
            break
        choices.sort()
        x, y = choices[0][3], choices[0][4]
        local[y][x] = True
        count += 1
    return count


def main():
    rd = sys.stdin.readline
    line = rd()
    grid = [[False] * W for _ in range(H)]
    tails = None
    trails = []
    while line:
        parts = line.split()
        if len(parts) < 2:
            line = rd()
            continue
        n, p = int(parts[0]), int(parts[1])
        heads = read_heads(n)
        if heads is None:
            return
        if tails is None:
            tails = list(heads)
            trails = [set() for _ in range(n)]
        rebuild(grid, heads, tails, trails)

        me = heads[p]
        if me is None:
            print("UP", flush=True)
            line = rd()
            continue

        blocked = [row[:] for row in grid]
        for h in heads:
            if h is not None:
                blocked[h[1]][h[0]] = True
        blocked[me[1]][me[0]] = False  # on part de notre tête

        best = None
        for name, dx, dy in DIRS:
            nx, ny = me[0] + dx, me[1] + dy
            if not (0 <= nx < W and 0 <= ny < H) or blocked[ny][nx]:
                continue
            trial = [row[:] for row in blocked]
            trial[me[1]][me[0]] = True
            trial[ny][nx] = True
            th = list(heads)
            th[p] = (nx, ny)
            dm, ds = voronoi_scores(th, p, trial)
            voronoi = 0
            reach = 0
            for y in range(H):
                rowm, tr = dm[y], trial[y]
                for x in range(W):
                    if tr[x]:
                        continue
                    a = rowm[x]
                    if a == 99:
                        continue
                    reach += 1
                    b = min(d[y][x] for d in ds) if ds else 99
                    if a < b:
                        voronoi += 1
                    elif a > b:
                        voronoi -= 1
            mob = sum(1 for a, b in NEI[nx][ny] if not trial[b][a])
            fill = greedy_fill((nx, ny), trial, 600)
            score = (100000 * voronoi + 100 * reach + 10 * mob + fill,
                     -ny, -nx, DIRS.index((name, dx, dy)))
            if best is None or score > best[0]:
                best = (score, name)
        print(best[1] if best else "UP", flush=True)
        line = rd()


if __name__ == "__main__":
    main()
