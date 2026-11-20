#!/usr/bin/env python3
"""Spring Challenge 2021 (Wood 4) — compléter les meilleurs arbres au plus vite."""
import sys


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def main() -> None:
    n = int(rd())
    richness: dict[int, int] = {}
    for _ in range(n):
        f = rd().split()
        if len(f) >= 2:
            richness[int(f[0])] = int(f[1])

    while True:
        line = rd()
        if not line:
            break
        _day = int(line)
        _nutrients = int(rd())
        f = rd().split()
        if len(f) < 2:
            break
        my_sun, _my_score = int(f[0]), int(f[1])
        rd()  # adversaire
        n_trees = int(rd())
        best: tuple[int, int] | None = None
        for _ in range(n_trees):
            t = rd().split()
            if len(t) < 4:
                break
            idx, size, mine = int(t[0]), int(t[1]), int(t[2])
            if mine == 1 and size == 3:
                key = (-richness.get(idx, 0), idx)
                if best is None or key < best:
                    best = key
        n_actions = int(rd())
        for _ in range(n_actions):
            rd()
        if my_sun >= 4 and best is not None:
            print(f"COMPLETE {best[1]}")
        else:
            print("WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
