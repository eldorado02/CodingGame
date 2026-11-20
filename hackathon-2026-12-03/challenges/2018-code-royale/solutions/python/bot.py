#!/usr/bin/env python3
"""Code Royale Wood 3 — barracks KNIGHT, TRAIN en boucle."""
import sys


def dist(ax: int, ay: int, bx: int, by: int) -> float:
    return ((ax - bx) ** 2 + (ay - by) ** 2) ** 0.5


def main() -> None:
    line = sys.stdin.readline()
    if not line:
        return
    n = int(line)
    # siteId -> [x, y, radius, structureType, owner, param1, param2]
    sites: dict[int, list[int]] = {}
    for _ in range(n):
        sid, x, y, r = map(int, sys.stdin.readline().split())
        sites[sid] = [x, y, r, -1, -1, -1, -1]

    while True:
        line = sys.stdin.readline()
        if not line:
            return
        gold, _touched = map(int, line.split())

        for _ in range(n):
            line = sys.stdin.readline()
            if not line:
                return
            sid, i1, i2, stype, owner, p1, p2 = map(int, line.split())
            if sid in sites:
                sites[sid][3:] = [stype, owner, p1, p2]

        line = sys.stdin.readline()
        if not line:
            return
        nu = int(line)
        mx = my = 200
        ex = ey = -1
        for _ in range(nu):
            line = sys.stdin.readline()
            if not line:
                return
            x, y, owner, utype, hp = map(int, line.split())
            if utype == -1:
                if owner == 0:
                    mx, my = x, y
                else:
                    ex, ey = x, y
        if ex < 0:
            ex, ey = 1920 - mx, 1000 - my

        # 1. Détruire un barracks ennemi proche par contact.
        destroy = None
        best_d = 250.0
        for sid, s in sites.items():
            if s[3] == 2 and s[4] == 1:  # barracks ennemi
                d = dist(mx, my, s[0], s[1])
                if d < best_d:
                    best_d, destroy = d, s
        if destroy is not None:
            action = f"MOVE {destroy[0]} {destroy[1]}"
        else:
            # 2. Construire un barracks KNIGHT sur le meilleur site libre.
            best_sid = None
            best_score = float("inf")
            for sid, s in sites.items():
                stype, owner, p1, p2 = s[3:]
                if owner == 1:
                    continue  # site ennemi
                if stype == 2 and p1 > 0:
                    continue  # entraînement en cours : non remplaçable
                if stype == 2 and p2 == 0:
                    continue  # déjà un barracks KNIGHT
                de = dist(s[0], s[1], ex, ey)
                dm = dist(s[0], s[1], mx, my)
                score = de + 0.6 * dm
                if de < 140:
                    score += 10000  # éviter la zone de destruction
                if score < best_score:
                    best_score, best_sid = score, sid
            if best_sid is None:
                action = "WAIT"
            else:
                action = f"BUILD {best_sid} BARRACKS-KNIGHT"

        # 3. Entraîner les barracks KNIGHT prêts et abordables.
        train: list[int] = []
        left = gold
        for sid, s in sites.items():
            if s[3] == 2 and s[4] == 0 and s[5] == 0 and s[6] == 0 and left >= 80:
                train.append(sid)
                left -= 80
        order = "TRAIN" + (" " + " ".join(map(str, train)) if train else "")
        print(action, flush=True)
        print(order, flush=True)


if __name__ == "__main__":
    main()
