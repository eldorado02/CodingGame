#!/usr/bin/env python3
"""Wondev Woman Wood 3 — meilleure action légale : victoire, déni, montée."""
import sys

DELTA = {
    "N": (0, -1), "NE": (1, -1), "E": (1, 0), "SE": (1, 1),
    "S": (0, 1), "SW": (-1, 1), "W": (-1, 0), "NW": (-1, -1),
}


def height(grid: list[list[int]], x: int, y: int, size: int) -> int:
    if 0 <= x < size and 0 <= y < size:
        return grid[y][x]
    return -1


def main() -> None:
    size = int(sys.stdin.readline())
    int(sys.stdin.readline())  # unitsPerPlayer (1)
    while True:
        raw = [sys.stdin.readline().strip() for _ in range(size)]
        if not raw[0]:
            return
        grid = [[-1 if c == "." else int(c) for c in row] for row in raw]

        ux, uy = map(int, sys.stdin.readline().split())
        ox, oy = map(int, sys.stdin.readline().split())
        n = int(sys.stdin.readline())
        actions = []
        for _ in range(n):
            parts = sys.stdin.readline().split()
            if len(parts) < 4:
                return
            actions.append((parts[2], parts[3]))  # dir1 (move), dir2 (build)

        best_action = None
        best_score = -(10**9)
        for d1, d2 in actions:
            dx, dy = DELTA[d1]
            mx, my = ux + dx, uy + dy
            h_dest = height(grid, mx, my, size)

            bx, by = DELTA[d2]
            cx, cy = mx + bx, my + by
            h_build = height(grid, cx, cy, size)

            score = 0
            if h_dest == 3:
                score += 1_000_000                      # victoire immédiate
            if h_dest < 0:
                score -= 100_000
            else:
                score += 1000 * h_dest                  # monter vers le niveau 3
            if h_build == 3:
                if ox >= 0 and abs(cx - ox) <= 1 and abs(cy - oy) <= 1:
                    score += 50_000                     # déni : retirer sa case 3
                else:
                    score -= 50
            elif h_build == 2:
                score += 300                            # tremplin de niveau 3
            elif h_build == 1:
                score += 100

            if score > best_score:
                best_score = score
                best_action = (d1, d2)

        if best_action is None:
            print("WAIT", flush=True)
        else:
            print(f"MOVE&BUILD 0 {best_action[0]} {best_action[1]}", flush=True)


if __name__ == "__main__":
    main()
