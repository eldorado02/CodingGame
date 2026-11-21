#!/usr/bin/env python3
"""Back Track King (Wood) — relier deux villes dès que possible.

Le boss Wood passe son tour : il suffit de construire une connexion active.
`AUTOPLACE` trace le chemin le moins cher par tranches de 3 points de peinture ;
on réémet la même commande jusqu'à la fin de la partie.
"""
import sys
from collections import deque


def main() -> None:
    buffered: deque[str] = deque()

    def token() -> str:
        while not buffered:
            line = sys.stdin.readline()
            if not line:
                raise EOFError
            buffered.extend(line.split())
        return buffered.popleft()

    try:
        token()  # myId
        width = int(token())
        height = int(token())
        for _ in range(width * height):
            token()  # regionId
            token()  # type

        town_count = int(token())
        towns: dict[int, tuple[int, int]] = {}
        targets: list[tuple[int, int]] = []
        for _ in range(town_count):
            town_id = int(token())
            x, y = int(token()), int(token())
            towns[town_id] = (x, y)
            desired = token()
            if desired != "x":
                targets.extend((town_id, int(wanted)) for wanted in desired.split(","))
    except EOFError:
        return

    best = min(targets, key=lambda pair: (
        abs(towns[pair[0]][0] - towns[pair[1]][0])
        + abs(towns[pair[0]][1] - towns[pair[1]][1]),
        pair[0], pair[1],
    ))
    (x1, y1), (x2, y2) = towns[best[0]], towns[best[1]]
    command = f"AUTOPLACE {x1} {y1} {x2} {y2}"

    while True:
        try:
            token()  # myScore
            token()  # foeScore
            for _ in range(width * height * 4):
                token()  # trackOwner, instability, inked, partOfActiveConnections
        except EOFError:
            return
        print(command, flush=True)


if __name__ == "__main__":
    main()
