#!/usr/bin/env python3
"""Fall Challenge 2020 (Wood 4) — brasser la recette la plus rentable."""
import sys


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def main() -> None:
    while True:
        line = rd()
        if not line:
            break
        n = int(line)
        orders = []
        for _ in range(n):
            f = rd().split()
            if len(f) < 6:
                break
            aid = int(f[0])
            deltas = tuple(int(f[i]) for i in range(2, 6))
            price = int(f[6])
            if f[1] == "BREW":
                orders.append((aid, deltas, price))
        mine = rd().split()
        if len(mine) < 5:
            break
        inv = tuple(int(v) for v in mine[:4])
        rd()  # adversaire (inutile dans cette ligue)

        best = None
        for aid, deltas, price in orders:
            need = tuple(-d for d in deltas)
            if any(need[i] > inv[i] for i in range(4)):
                continue
            key = (-price, sum(need), aid)
            if best is None or key < best[0]:
                best = (key, aid)
        print(f"BREW {best[1]}" if best else "WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
