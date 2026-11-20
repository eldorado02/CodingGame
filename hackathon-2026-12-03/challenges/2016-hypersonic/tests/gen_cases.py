#!/usr/bin/env python3
"""Génère les cas déterministes de Hypersonic (Wood 3)."""
from pathlib import Path

HERE = Path(__file__).parent
W, H = 13, 11


def grid_with(boxes: set[tuple[int, int]]) -> list[str]:
    return [
        "".join("0" if (x, y) in boxes else "." for x in range(W))
        for y in range(H)
    ]


def write(name: str, boxes: set[tuple[int, int]], entities: list[tuple[int, ...]]) -> None:
    lines = [f"{W} {H} 0"] + grid_with(boxes) + [str(len(entities))]
    lines += [" ".join(map(str, e)) for e in entities]
    (HERE / name).write_text("\n".join(lines) + "\n")


def player(x: int, y: int, bombs: int = 1) -> tuple[int, ...]:
    return (0, 0, x, y, bombs, 3)


def bomb(owner: int, x: int, y: int, timer: int = 5) -> tuple[int, ...]:
    return (1, owner, x, y, timer, 3)


# Grille vide, joueur au centre : MOVE vers le centre.
write("01_empty.in", set(), [player(1, 1)])
# Boîte juste à droite : BOMB avec fuite vers la gauche.
write("02_box_adjacent.in", {(2, 1)}, [player(1, 1)])
# Deux boîtes alignées à portée.
write("03_two_boxes.in", {(2, 1), (3, 1)}, [player(1, 1)])
# Bombe alliée à droite : MOVE doit contourner.
write("04_bomb_block.in", {(2, 1)}, [player(1, 1), bomb(0, 2, 1)])
# Plus de bombe disponible : MOVE vers la boîte.
write("05_no_bomb_left.in", {(2, 1)}, [player(1, 1, 0)])
# Boîte loin : MOVE se rapproche.
write("06_far_box.in", {(8, 5)}, [player(1, 1)])
# Bombe adverse près d'une boîte : on évite la case.
write("07_enemy_bomb.in", {(5, 5)}, [player(1, 1), bomb(1, 3, 3)])
# Deux joueurs : on lit bien myId=1.
lines = [f"{W} {H} 1"] + grid_with({(10, 9)}) + ["2", " ".join(map(str, player(1, 1))), " ".join(map(str, (0, 1, 11, 9, 1, 3)))]
(HERE / "08_myid_1.in").write_text("\n".join(lines) + "\n")
