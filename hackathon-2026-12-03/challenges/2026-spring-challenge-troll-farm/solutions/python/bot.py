#!/usr/bin/env python3
"""Troll Farm Wood 1 — entraîne un troll polyvalent, récolte et dépose."""
import sys
from collections import deque

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))
KINDS = ("PLUM", "LEMON", "APPLE", "BANANA")


def main() -> None:
    rd = sys.stdin.readline
    line = rd()
    if not line:
        return
    w, h = map(int, line.split())
    grid = [rd().strip() for _ in range(h)]
    shack = next((x, y) for y, row in enumerate(grid)
                 for x, ch in enumerate(row) if ch == "0")
    blocked = {(x, y) for y, row in enumerate(grid)
               for x, ch in enumerate(row) if ch != "."}
    drop_cells = [(shack[0] + dx, shack[1] + dy) for dx, dy in DIRS
                  if (shack[0] + dx, shack[1] + dy) not in blocked]
    MAX_TROLLS = 3

    while True:
        inv_line = rd()
        if not inv_line:
            return
        inv = list(map(int, inv_line.split()))
        rd()  # foe inventory
        trees = []
        for _ in range(int(rd())):
            f = rd().split()
            trees.append((f[0], int(f[1]), int(f[2]), int(f[5]), int(f[6])))
        my_trolls = []
        foe_pos = set()
        for _ in range(int(rd())):
            f = rd().split()
            if int(f[1]) == 0:
                my_trolls.append((int(f[0]), int(f[2]), int(f[3]),
                                  int(f[4]), int(f[5]), int(f[6]),
                                  sum(int(v) for v in f[8:12])))
            else:
                foe_pos.add((int(f[2]), int(f[3])))

        actions = []
        if len(my_trolls) < MAX_TROLLS:
            n = len(my_trolls)
            best = None
            for s in range(1, 4):
                for c in range(1, 4):
                    for hv in range(1, 4):
                        cp, cl, ca = n + s * s, n + c * c, n + hv * hv
                        if cp <= inv[0] and cl <= inv[1] and ca <= inv[2]:
                            score = (3 * hv + 2 * c + s, -cp - cl - ca)
                            if best is None or score > best[0]:
                                best = (score, s, c, hv)
            if best:
                actions.append(f"TRAIN {best[1]} {best[2]} {best[3]} 0")

        occupied = {(t[1], t[2]) for t in my_trolls} | foe_pos
        for tid, x, y, speed, carry, power, load in my_trolls:
            here = (x, y)
            # BFS distances / first steps over walkable cells.
            dist = {here: 0}
            first = {here: here}
            q = deque([here])
            while q:
                px, py = q.popleft()
                for dx, dy in DIRS:
                    nxt = (px + dx, py + dy)
                    if (0 <= nxt[0] < w and 0 <= nxt[1] < h and nxt not in blocked
                            and nxt not in occupied and nxt not in dist):
                        dist[nxt] = dist[(px, py)] + 1
                        first[nxt] = nxt if (px, py) == here else first[(px, py)]
                        q.append(nxt)

            fruit = [(tx, ty, nf) for _k, tx, ty, nf, _cd in trees if nf > 0]
            on_fruit = [t for t in fruit if (t[0], t[1]) == here]
            if on_fruit and load < carry:
                actions.append(f"HARVEST {tid}")
                continue
            if load > 0:
                if (x, y) in [(sx, sy) for sx, sy in drop_cells]:
                    actions.append(f"DROP {tid}")
                    continue
                goals = [p for p in drop_cells if p in dist]
                if goals:
                    goal = min(goals, key=lambda p: (dist[p], p[1], p[0]))
                    step = _step(goal, speed, dist, first)
                    actions.append(f"MOVE {tid} {step[0]} {step[1]}")
                else:
                    actions.append(f"WAIT")
                continue
            targets = [p for p in fruit if (p[0], p[1]) in dist]
            if targets:
                goal = min(targets, key=lambda p: (dist[(p[0], p[1])], -p[2], p[1], p[0]))
                step = _step((goal[0], goal[1]), speed, dist, first)
                actions.append(f"MOVE {tid} {step[0]} {step[1]}")
            else:
                actions.append("WAIT")

        print(";".join(actions) if actions else "WAIT", flush=True)


def _step(goal, speed, dist, first):
    """Case à `speed` pas au plus sur le chemin vers goal."""
    if dist[goal] <= speed:
        return goal
    cur = goal
    while dist[cur] > speed:
        cur = first[cur]
    return cur


if __name__ == "__main__":
    main()
