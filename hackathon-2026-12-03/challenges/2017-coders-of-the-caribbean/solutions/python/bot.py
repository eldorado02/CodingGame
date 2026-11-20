#!/usr/bin/env python3
"""Coders of the Caribbean Wood 3 — viser le baril le plus proche."""
import sys


def main() -> None:
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        int(line)  # myShipCount (toujours 1 en Wood 3)
        line = sys.stdin.readline()
        if not line:
            return
        n = int(line)
        sx = sy = -1
        barrels: list[tuple[int, int]] = []
        for _ in range(n):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            kind, x, y = parts[1], int(parts[2]), int(parts[3])
            args = parts[4:8]
            if kind == "SHIP" and int(args[3]) == 1:
                sx, sy = x, y
            elif kind == "BARREL":
                barrels.append((x, y))

        if sx < 0 or not barrels:
            print("WAIT", flush=True)
            continue
        bx, by = min(barrels, key=lambda b: (b[0] - sx) ** 2 + (b[1] - sy) ** 2)
        print(f"MOVE {bx} {by}", flush=True)


if __name__ == "__main__":
    main()
