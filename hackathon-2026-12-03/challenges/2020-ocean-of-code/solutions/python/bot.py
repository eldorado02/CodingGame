#!/usr/bin/env python3
"""Ocean of Code (Wood 3) — croyance sur l'adversaire + torpedo."""
import sys
from collections import deque

N = 15
DIRS = (("N", 0, -1), ("E", 1, 0), ("S", 0, 1), ("W", -1, 0))
DIR_VEC = {"N": (0, -1), "E": (1, 0), "S": (0, 1), "W": (-1, 0)}


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def sector(x: int, y: int) -> int:
    return x // 5 + 1 + (y // 5) * 3


def bfs(start_x: int, start_y: int, water: list[list[bool]]) -> list[list[int]]:
    dist = [[-1] * N for _ in range(N)]
    dist[start_y][start_x] = 0
    q = deque([(start_x, start_y)])
    while q:
        x, y = q.popleft()
        for _name, dx, dy in DIRS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < N and 0 <= ny < N and water[ny][nx] and dist[ny][nx] < 0:
                dist[ny][nx] = dist[y][x] + 1
                q.append((nx, ny))
    return dist


def main() -> None:
    water: list[list[bool]] = []
    visited: list[list[bool]] = []
    bel: list[list[bool]] = []
    prev_opp_life = None
    last_fire = None

    header = rd()
    if not header:
        return

    if len(header.split()) == 3:
        _w, h, _myid = map(int, header.split())
        grid = [rd() for _ in range(h)]
        water = [[grid[y][x] == '.' for x in range(N)] for y in range(N)]
        visited = [[False] * N for _ in range(N)]
        bel = [[water[y][x] for x in range(N)] for y in range(N)]

        best_x = best_y = -1
        best_deg = -1
        best_center = 10**9
        for y in range(N):
            for x in range(N):
                if not water[y][x]:
                    continue
                deg = 0
                for _n, dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < N and 0 <= ny < N and water[ny][nx]:
                        deg += 1
                center = abs(x - 7) + abs(y - 7)
                if (deg > best_deg or
                        (deg == best_deg and center < best_center) or
                        (deg == best_deg and center == best_center and
                         (x < best_x or (x == best_x and y < best_y)))):
                    best_x, best_y = x, y
                    best_deg, best_center = deg, center
        visited[best_y][best_x] = True
        print(f"{best_x} {best_y}")
        sys.stdout.flush()
    else:
        # Format de tour直接 (utilisé uniquement par les tests unitaires).
        pass

    while True:
        line = rd()
        if not line:
            break
        fields = line.split()
        if len(fields) != 8:
            break
        px, py, my_life, opp_life = map(int, fields[:4])
        cooldown = int(fields[4])
        rd()  # sonar result
        orders = rd()

        delta = None if prev_opp_life is None else prev_opp_life - opp_life

        # 1) Retour de notre dernier tir.
        if last_fire is not None and delta is not None:
            tx, ty = last_fire
            if delta == 2:
                newbel = [[False] * N for _ in range(N)]
                if water[ty][tx]:
                    newbel[ty][tx] = True
                bel = newbel
            elif delta == 1:
                newbel = [[False] * N for _ in range(N)]
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        if dx == 0 and dy == 0:
                            continue
                        nx, ny = tx + dx, ty + dy
                        if 0 <= nx < N and 0 <= ny < N and water[ny][nx]:
                            newbel[ny][nx] = True
                bel = newbel
            elif delta == 0:
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        nx, ny = tx + dx, ty + dy
                        if 0 <= nx < N and 0 <= ny < N:
                            bel[ny][nx] = False
        last_fire = None

        def reset_if_empty() -> None:
            nonlocal bel
            if not any(any(row) for row in bel):
                bel = [[water[y][x] for x in range(N)] for y in range(N)]

        reset_if_empty()

        # 2) Ordres adverses.
        if orders != "NA":
            for raw in orders.split('|'):
                tok = raw.split()
                if not tok:
                    continue
                if tok[0] == "MOVE" and len(tok) >= 2 and tok[1] in DIR_VEC:
                    dx, dy = DIR_VEC[tok[1]]
                    newbel = [[False] * N for _ in range(N)]
                    for y in range(N):
                        for x in range(N):
                            if not bel[y][x]:
                                continue
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < N and 0 <= ny < N and water[ny][nx]:
                                newbel[ny][nx] = True
                    bel = newbel
                elif tok[0] == "SURFACE" and len(tok) >= 2:
                    s = int(tok[1])
                    for y in range(N):
                        for x in range(N):
                            if bel[y][x] and sector(x, y) != s:
                                bel[y][x] = False
                elif tok[0] == "TORPEDO" and len(tok) >= 3:
                    ax, ay = int(tok[1]), int(tok[2])
                    d = bfs(ax, ay, water)
                    for y in range(N):
                        for x in range(N):
                            if bel[y][x] and (d[y][x] < 0 or d[y][x] > 4):
                                bel[y][x] = False
                reset_if_empty()

        # 3) Centre de gravité de la croyance.
        cnt = 0
        sumx = 0
        sumy = 0
        for y in range(N):
            for x in range(N):
                if bel[y][x]:
                    cnt += 1
                    sumx += x
                    sumy += y
        if cnt == 0:
            bel = [[water[y][x] for x in range(N)] for y in range(N)]
            cnt = sum(sum(row) for row in water)
            sumx = sum(x for y in range(N) for x in range(N) if water[y][x])
            sumy = sum(y for y in range(N) for x in range(N) if water[y][x])

        def best_dir(ignore_visited: bool) -> tuple[int, int, int] | None:
            best = None
            for idx, (name, dx, dy) in enumerate(DIRS):
                nx, ny = px + dx, py + dy
                if not (0 <= nx < N and 0 <= ny < N and water[ny][nx]):
                    continue
                if not ignore_visited and visited[ny][nx]:
                    continue
                free = 0
                for _n, ddx, ddy in DIRS:
                    ax, ay = nx + ddx, ny + ddy
                    if 0 <= ax < N and 0 <= ay < N and water[ay][ax]:
                        if ignore_visited:
                            blocked = (ax == px and ay == py)
                        else:
                            blocked = visited[ay][ax]
                        if not blocked:
                            free += 1
                cdist = abs(2 * cnt * nx - sumx) + abs(2 * cnt * ny - sumy)
                key = (-free, cdist, idx)
                if best is None or key < best[0]:
                    best = (key, nx, ny)
            if best is None:
                return None
            return best[1], best[2], best[0][2]

        visited[py][px] = True
        actions = []
        choice = best_dir(False)
        if choice is not None:
            nx, ny, _idx = choice
            # Direction déterminée directement par le delta.
            dx, dy = nx - px, ny - py
            dname = next(name for name, ddx, ddy in DIRS if ddx == dx and ddy == dy)
            actions = [f"MOVE {dname} TORPEDO"]
            final_x, final_y = nx, ny
            visited[ny][nx] = True
        else:
            # Pas de case libre : surface, puis déplacement possible.
            for y in range(N):
                for x in range(N):
                    visited[y][x] = False
            visited[py][px] = True
            choice2 = best_dir(True)
            actions = ["SURFACE"]
            if choice2 is not None:
                nx, ny, _idx = choice2
                dx, dy = nx - px, ny - py
                dname = next(name for name, ddx, ddy in DIRS if ddx == dx and ddy == dy)
                actions.append(f"MOVE {dname} TORPEDO")
                final_x, final_y = nx, ny
                visited[py][px] = True
                visited[ny][nx] = True
            else:
                final_x, final_y = px, py

        # 4) Tir si le torpedo est prêt.
        if cooldown == 0:
            dstart = bfs(px, py, water)
            best_score = -1
            best_tx = best_ty = -1
            for ty in range(N):
                for tx in range(N):
                    if not water[ty][tx] or dstart[ty][tx] < 0 or dstart[ty][tx] > 4:
                        continue
                    if abs(tx - final_x) <= 1 and abs(ty - final_y) <= 1:
                        continue
                    score = 2 if bel[ty][tx] else 0
                    for dy in (-1, 0, 1):
                        for dx in (-1, 0, 1):
                            if dx == 0 and dy == 0:
                                continue
                            nx, ny = tx + dx, ty + dy
                            if 0 <= nx < N and 0 <= ny < N and bel[ny][nx]:
                                score += 1
                    if (score > best_score or
                            (score == best_score and
                             (tx < best_tx or (tx == best_tx and ty < best_ty)))):
                        best_score, best_tx, best_ty = score, tx, ty
            if best_score > 0:
                actions.append(f"TORPEDO {best_tx} {best_ty}")
                last_fire = (best_tx, best_ty)

        print("|".join(actions))
        sys.stdout.flush()
        prev_opp_life = opp_life


if __name__ == "__main__":
    main()
