#!/usr/bin/env python3
"""Code of Kutulu Wood 3 — réassurance prioritaire, fuite des wanderers."""
import sys

DIRS = ((0, 0), (0, -1), (0, 1), (-1, 0), (1, 0))


def main() -> None:
    w = int(sys.stdin.readline())
    h = int(sys.stdin.readline())
    grid = [sys.stdin.readline().strip() for _ in range(h)]
    sys.stdin.readline()  # 4 entiers sans signification

    while True:
        line = sys.stdin.readline()
        if not line:
            return
        n = int(line)
        mx = my = 0
        others: list[tuple[int, int, int]] = []  # x, y, sanity
        wanderers: list[tuple[int, int]] = []
        for i in range(n):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            kind = parts[0]
            x, y = int(parts[2]), int(parts[3])
            sanity = int(parts[4])
            state = int(parts[5])
            if i == 0:
                mx, my = x, y
            elif kind == "EXPLORER":
                if sanity > 0:
                    others.append((x, y, sanity))
            elif kind == "WANDERER" and state == 1:
                wanderers.append((x, y))

        best_score = -10**18
        best_move = (mx, my)
        for dx, dy in DIRS:
            px, py = mx + dx, my + dy
            if not (0 <= px < w and 0 <= py < h) or grid[py][px] == "#":
                continue
            reassured = any(abs(px - ox) + abs(py - oy) <= 2
                            for ox, oy, _s in others)
            dw = min((abs(px - wx) + abs(py - wy) for wx, wy in wanderers),
                     default=100)
            de = min((abs(px - ox) + abs(py - oy) for ox, oy, _s in others),
                     default=100)
            score = (100000 if reassured else 0) + 100 * dw - de
            if score > best_score:
                best_score, best_move = score, (px, py)

        if best_move == (mx, my):
            print("WAIT", flush=True)
        else:
            print(f"MOVE {best_move[0]} {best_move[1]}", flush=True)


if __name__ == "__main__":
    main()
