#!/usr/bin/env python3
"""Wondev Woman — une seule action par tour (le referee ne lit que outputs[0]).

 Protocole réel (stub officiel, ligue Wood 1) :
   init : size, unitsPerPlayer (= 2, malgré l'énoncé archivé qui dit 1)
   tour : size lignes de grille, puis unitsPerPlayer lignes "x y" pour nous,
          autant pour l'adversaire, puis legalActions, puis legalActions
          lignes "atype index dir1 dir2".
 Sortie : une ligne, la meilleure action parmi toutes les unités.

 Règle décisive : placer son unité sur une case de hauteur 3 gagne la partie.
 On construit donc des tremplins de hauteur 2 vers 3, et on détruit (hauteur 4)
 les cases de hauteur 3 accessibles à l'adversaire.
"""
import sys

DIRECTIONS = {
    "N": (0, -1), "NE": (1, -1), "E": (1, 0), "SE": (1, 1),
    "S": (0, 1), "SW": (-1, 1), "W": (-1, 0), "NW": (-1, -1),
}


def height(grid: list[list[int]], x: int, y: int) -> int:
    """-1 = hors plateau (trou), sinon hauteur 0..3."""
    if 0 <= y < len(grid) and 0 <= x < len(grid[0]):
        return grid[y][x]
    return -1


def main() -> None:
    rd = sys.stdin.readline
    first = rd()
    if not first:
        return
    size = int(first)
    units_per_player = int(rd())

    while True:
        raw = [rd().strip() for _ in range(size)]
        if not raw[0]:
            return
        grid = [[-1 if c == "." else int(c) for c in row] for row in raw]

        ours = []
        for _ in range(units_per_player):
            x, y = map(int, rd().split())
            ours.append((x, y))
        foes = []
        for _ in range(units_per_player):
            x, y = map(int, rd().split())
            foes.append((x, y))

        legal_count_line = rd()
        if not legal_count_line:
            return
        legal_count = int(legal_count_line)
        actions = []
        for _ in range(legal_count):
            parts = rd().split()
            if len(parts) < 4:
                return
            actions.append((parts[0], int(parts[1]), parts[2], parts[3]))

        # Le referee ne lit que outputs[0] : une seule action par tour,
        # choisie parmi toutes les unités.
        best: tuple[int, str] | None = None
        for atype, index, dir1, dir2 in actions:
            dx, dy = DIRECTIONS[dir1]
            x, y = ours[index]
            mx, my = x + dx, y + dy
            h_dest = height(grid, mx, my)

            bx, by = DIRECTIONS[dir2]
            cx, cy = mx + bx, my + by
            h_build = height(grid, cx, cy)

            score = 0
            if h_dest == 3:
                score += 1_000_000           # marche sur une case niveau 3 = 1 point
            elif h_dest >= 0:
                score += 400 * h_dest        # monter vers le niveau 3
            # Rester à côté d'une case niveau 3 permet de marquer au tour suivant.
            if h_dest >= 0:
                for dx2, dy2 in DIRECTIONS.values():
                    if height(grid, mx + dx2, my + dy2) == 3:
                        score += 1500
                        break

            if h_build == 3:
                # Détruire une case de niveau 3 voisine de l'adversaire :
                # elle passe à 4 et disparaît, lui retirant une victoire.
                if any(abs(cx - fx) <= 1 and abs(cy - fy) <= 1 for fx, fy in foes):
                    score += 50_000
                else:
                    score -= 40
            elif h_build == 2:
                # Tremplin : on pourra monter dessus au prochain tour.
                score += 3000 if abs(cx - mx) + abs(cy - my) <= 1 else 500
            elif h_build == 1:
                score += 100
            elif h_build == 0:
                score += 10

            token = f"{atype} {index} {dir1} {dir2}"
            if best is None or score > best[0]:
                best = (score, token)

        # `WAIT` n'est pas une commande valide dans cette version : sans action
        # légale, on concède formellement plutôt que d'être disqualifié.
        print(best[1] if best else "ACCEPT-DEFEAT", flush=True)


if __name__ == "__main__":
    main()
