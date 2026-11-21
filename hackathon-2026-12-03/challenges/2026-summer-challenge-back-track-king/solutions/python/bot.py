#!/usr/bin/env python3
"""Back Track King (Bronze+) — construit la connexion désirée non active la
moins chère et disrupte la région où l'adversaire possède le plus de rails.

Les connexions actives sont lues directement dans l'entrée (4e champ de chaque
case) : inutile de reconstruire les chemins. Le DISRUPT privilégie les régions
traversées par une connexion active, puis le nombre de rails ennemis, puis
l'instabilité.
"""
import heapq
import sys
from collections import deque

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def main() -> None:
    buf: deque[str] = deque()

    def token() -> str:
        while not buf:
            line = sys.stdin.readline()
            if not line:
                raise EOFError
            buf.extend(line.split())
        return buf.popleft()

    try:
        my_id = int(token())
        width, height = int(token()), int(token())
        cells = width * height
        region_at = []
        terrain = []
        for _ in range(cells):
            region_at.append(int(token()))
            terrain.append(int(token()))

        towns: dict[int, tuple[int, int]] = {}
        desired: set[tuple[int, int]] = set()
        town_regions: set[int] = set()
        for _ in range(int(token())):
            tid = int(token())
            x, y = int(token()), int(token())
            towns[tid] = (x, y)
            town_regions.add(region_at[y * width + x])
            wanted = token()
            if wanted != "x":
                for w in wanted.split(","):
                    a, b = int(tid), int(w)
                    desired.add((min(a, b), max(a, b)))
    except EOFError:
        return

    def parse_pairs(s: str) -> set[tuple[int, int]]:
        out = set()
        for item in s.split(","):
            if "-" not in item:
                continue
            a_s, b_s = item.split("-", 1)
            if a_s.isdigit() and b_s.isdigit():
                a, b = int(a_s), int(b_s)
                out.add((min(a, b), max(a, b)))
        return out

    while True:
        try:
            token()  # myScore
            token()  # foeScore
            owner = []
            instability = []
            inked = []
            cell_active = []
            active: set[tuple[int, int]] = set()
            for _ in range(cells):
                owner.append(int(token()))
                instability.append(int(token()))
                inked.append(token() not in ("0", "false", "False"))
                pairs = parse_pairs(token())
                cell_active.append(pairs)
                active |= pairs
        except EOFError:
            return

        def path_cost(a: int, b: int) -> int | None:
            sx, sy = towns[a]
            tx, ty = towns[b]
            start, goal = sy * width + sx, ty * width + tx
            dist = {start: 0}
            heap = [(0, start)]
            while heap:
                d, i = heapq.heappop(heap)
                if d != dist.get(i):
                    continue
                if i == goal:
                    return d
                x, y = i % width, i // width
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if not (0 <= nx < width and 0 <= ny < height):
                        continue
                    ni = ny * width + nx
                    if inked[ni]:
                        continue
                    nd = d + (0 if owner[ni] >= 0 else terrain[ni] + 1)
                    if nd < dist.get(ni, 10**9):
                        dist[ni] = nd
                        heapq.heappush(heap, (nd, ni))
            return None

        actions = []
        targets = []
        for a, b in desired:
            if (a, b) in active:
                continue
            d = path_cost(a, b)
            if d is None or d == 0:
                continue
            x1, y1 = towns[a]
            x2, y2 = towns[b]
            targets.append((d, a, b, x1, y1, x2, y2))
        if targets:
            targets.sort()
            _d, a, b, x1, y1, x2, y2 = targets[0]
            actions.append(f"AUTOPLACE {x1} {y1} {x2} {y2}")

        regions: dict[int, dict[str, int]] = {}
        for i in range(cells):
            rid = region_at[i]
            if rid in town_regions or inked[i]:
                continue
            r = regions.setdefault(rid, {"mine": 0, "foe": 0, "inst": 0, "active": 0})
            o = owner[i]
            if o == my_id:
                r["mine"] += 1
            elif o >= 0 and o != 2:
                r["foe"] += 1
            if instability[i] > r["inst"]:
                r["inst"] = instability[i]
            if cell_active[i]:
                r["active"] = 1
        candidates = []
        for rid, r in regions.items():
            if r["foe"] > r["mine"]:
                candidates.append((r["active"], r["foe"], r["inst"], rid))
        disrupted = None
        if candidates:
            candidates.sort(reverse=True)
            disrupted = candidates[0][3]
            actions.append(f"DISRUPT {disrupted}")

        # Fin de partie : toutes les connexions sont actives, on place des rails
        # adjacents aux chemins qui paient pour les voler au tie-break N/E/S/O.
        if not targets:
            town_cells = {y * width + x for x, y in towns.values()}
            steal = []
            for i in range(cells):
                if owner[i] != -1 or inked[i] or i in town_cells:
                    continue
                if region_at[i] == disrupted:
                    continue
                x, y = i % width, i // width
                adj = []
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < width and 0 <= ny < height:
                        j = ny * width + nx
                        if cell_active[j]:
                            adj.append(j)
                if not adj:
                    continue
                foe = sum(1 for j in adj if owner[j] >= 0 and owner[j] != 2
                          and owner[j] != my_id)
                steal.append((len(adj), foe, -terrain[i], y, x, i))
            if steal:
                steal.sort(reverse=True)
                for _n, _f, _t, _y, _x, i in steal[:3]:
                    actions.append(f"PLACE_TRACKS {i % width} {i // width}")

        print(";".join(actions) if actions else "WAIT", flush=True)


if __name__ == "__main__":
    main()
