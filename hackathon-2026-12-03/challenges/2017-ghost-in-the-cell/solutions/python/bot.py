#!/usr/bin/env python3
"""Ghost in the Cell Wood 3 — forces effectives + meilleure action globale."""
import sys

INF = 10**6
RESERVE = 5


def distances(n: int, links: list[tuple[int, int, int]]) -> list[list[int]]:
    dist = [[0 if i == j else INF for j in range(n)] for i in range(n)]
    for a, b, d in links:
        dist[a][b] = dist[b][a] = d
    for k in range(n):
        for i in range(n):
            for j in range(n):
                if dist[i][k] + dist[k][j] < dist[i][j]:
                    dist[i][j] = dist[i][k] + dist[k][j]
    return dist


def choose(owner: list[int], cyborgs: list[int], prod: list[int],
           dist: list[list[int]], n: int) -> str:
    force = cyborgs[:]
    for i in range(n):
        if owner[i] == 0:
            force[i] = 0

    best: tuple[int, int, int, int] | None = None  # score, src, dst, count

    # Défense : renforcer l'usine la plus menacée.
    for i in range(n):
        if owner[i] != 1 or force[i] >= 0:
            continue
        for j in range(n):
            if owner[j] != 1 or force[j] <= -force[i] + RESERVE:
                continue
            count = min(force[j] - RESERVE, -force[i] + 2)
            score = 1000 + dist[j][i] * -1
            if best is None or score > best[0]:
                best = (score, j, i, count)

    # Expansion / attaque.
    if best is None:
        for src in range(n):
            if owner[src] != 1 or force[src] <= RESERVE:
                continue
            for dst in range(n):
                if owner[dst] == 1 or dist[src][dst] >= INF:
                    continue
                need = force[dst] + 1
                if need <= 0 or force[src] - RESERVE < need:
                    continue
                score = (100 if owner[dst] == 0 else 0) + prod[dst] * 20 - dist[src][dst] * 2 - need
                if best is None or score > best[0]:
                    best = (score, src, dst, need)

    if best is None:
        return "WAIT"
    return f"MOVE {best[1]} {best[2]} {best[3]}"


def main() -> None:
    line = sys.stdin.readline()
    if not line:
        return
    n = int(line)
    m = int(sys.stdin.readline())
    links = []
    for _ in range(m):
        a, b, d = map(int, sys.stdin.readline().split())
        links.append((a, b, d))
    dist = distances(n, links)

    while True:
        line = sys.stdin.readline()
        if not line:
            return
        count = int(line)
        owner = [0] * n
        cyborgs = [0] * n
        prod = [0] * n
        troops: list[tuple[int, int, int, int]] = []
        for _ in range(count):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            eid = int(parts[0])
            kind = parts[1]
            a, b, c, d, e = map(int, parts[2:7])
            if kind == "FACTORY":
                owner[eid], cyborgs[eid], prod[eid] = a, b, c
            elif kind == "TROOP":
                troops.append((a, b, c, d))  # owner, src, dst, count (e inutilisé)

        # Troupes en approche : on ajuste la force de la cible.
        for own, _src, dst, cnt in troops:
            cyborgs[dst] += cnt if own == 1 else -cnt

        print(choose(owner, cyborgs, prod, dist, n), flush=True)


if __name__ == "__main__":
    main()
