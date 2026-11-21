#!/usr/bin/env python3
"""Back Track King (Wood 2) — ink une région contenant un track adverse.

Chaque tour : cibler la région ennemie non town la plus instable ; 4 `DISRUPT`
consécutifs l'encrent et détruisent les tracks, ce qui remplit l'objectif.
"""
import sys
from collections import deque


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
        region_at = []
        for _ in range(width * height):
            region_at.append(int(token()))
            token()  # type

        town_count = int(token())
        town_regions: set[int] = set()
        for _ in range(town_count):
            token()  # townId
            x, y = int(token()), int(token())
            town_regions.add(region_at[y * width + x])
            token()  # desiredConnections
    except EOFError:
        return

    opponent = 1 - my_id
    while True:
        best: tuple[int, int] | None = None
        try:
            token()  # myScore
            token()  # foeScore
            for cell in range(width * height):
                owner = int(token())
                instability = int(token())
                inked = token().lower() not in ("0", "false")
                token()  # partOfActiveConnections
                rid = region_at[cell]
                if owner != opponent or inked or rid in town_regions:
                    continue
                key = (instability, -rid)
                if best is None or key > best:
                    best = key
        except EOFError:
            return
        print(f"DISRUPT {-best[1]}" if best is not None else "WAIT", flush=True)


if __name__ == "__main__":
    main()
