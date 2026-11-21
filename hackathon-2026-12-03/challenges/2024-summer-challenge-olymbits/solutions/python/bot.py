#!/usr/bin/env python3
"""Olymbits (Silver, 4 mini-jeux) — décision jointe, arithmétique entière.

Une seule action est envoyée aux quatre machines. Pour chaque action possible
on estime un score dans chaque mini-jeu, on les pondère, et on joue l'action
totale maximale. Toutes les valeurs sont des entiers afin que C, C++, Rust et
Python produisent exactement les mêmes décisions (fuzz différentiel).

Identification des machines :
  * course de haies : GPU ne contient que '.' et '#' (longueur 30) ;
  * tir à l'arc     : GPU ne contient que des chiffres (vents, 1 chiffre/tour) ;
  * roller          : GPU est une permutation de 4 lettres ULDR, reg6 >= 0 ;
  * plongée         : GPU est une suite de lettres ULDR (objectif du tour).
"""
import sys

# index -> (nom, dx, dy, lettre).  Ordre imposed par le stub : sortie = nom.
MOVES = (
    ("LEFT", -1, 0, "L"),
    ("DOWN", 0, 1, "D"),
    ("RIGHT", 1, 0, "R"),
    ("UP", 0, -1, "U"),
)
NAMES = tuple(m[0] for m in MOVES)
HURDLE_STEPS = (1, 2, 3, 2)     # LEFT, DOWN, RIGHT, UP
HURDLE_JUMP = (False, False, False, True)


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def hurdle_step(track: str, pos: int, stun: int, steps: int, jump: bool) -> tuple[int, int]:
    """Applique une action dans la course de haies."""
    if stun > 0:
        return pos, stun - 1
    if pos < 0:
        pos = 0
    last = len(track) - 1
    if jump:
        np = min(last, pos + 2)
        if np < last and track[np] == "#":
            return np, 3
        return np, 0
    np = pos
    for s in range(1, steps + 1):
        if pos + s >= last:
            return last, 0
        if track[pos + s] == "#":
            return pos + s, 3
        np = pos + s
    return np, 0


def hurdle_scores(track: str, pos: int, stun: int) -> list[int]:
    """Score = -temps estimé pour finir la course (×100 pour rester entier)."""
    out = []
    for a in range(4):
        p, s = hurdle_step(track, pos, stun, HURDLE_STEPS[a], HURDLE_JUMP[a])
        done = -1
        if p >= len(track) - 1:
            done = 1
        else:
            for t in range(3):
                options = (2, 1, 3, 0)  # RIGHT, DOWN, UP, LEFT
                moved = False
                for m in options:
                    q, ss = hurdle_step(track, p, s, HURDLE_STEPS[m], HURDLE_JUMP[m])
                    if q > p:
                        p, s = q, ss
                        moved = True
                        break
                if not moved:
                    p, s = hurdle_step(track, p, s, 2, True)
                if p >= len(track) - 1:
                    done = t + 2
                    break
        if done < 0:
            done = 5 + (len(track) - 1 - p) // 2
        out.append(-100 * done)
    return out


def archery_scores(gpu: str, x: int, y: int) -> list[int]:
    """Distance euclidienne au centre : on compare les carrés (entiers)."""
    winds = [int(c) for c in gpu if c.isdigit()]
    w = winds[0] if winds else 1
    future = winds[1:6]
    out = []
    for _name, dx, dy, _letter in MOVES:
        nx = max(-20, min(20, x + dx * w))
        ny = max(-20, min(20, y + dy * w))
        d2 = nx * nx + ny * ny
        px, py = nx, ny
        for fw in future:
            if px > 0:
                px = max(-20, px - fw)
            elif px < 0:
                px = min(20, px + fw)
            if py > 0:
                py = max(0, py - fw)
            elif py < 0:
                py = min(0, py + fw)
        d2f = px * px + py * py
        out.append(-2 * d2 - d2f)
    return out


def roller_scores(gpu: str, pos: int, risk: int, turns_left: int, others: list[int]) -> list[int]:
    order = gpu[:4]
    steps = (0, 1, 2, 2, 3)
    out = []
    for _n, _dx, _dy, letter in MOVES:
        idx = order.find(letter)
        if idx < 0:
            out.append(-10000)
        elif risk < 0:
            out.append(-100)
        else:
            nr = risk + (-1 if idx == 0 else 0 if idx == 1 else 1 if idx == 2 else 2)
            collide = 0
            for p in others:
                if (p - pos) % 10 == 0:
                    collide = 1
                    break
            if collide:
                nr += 2
            if nr >= 5:
                out.append(-100 - turns_left)
            else:
                out.append(10 * steps[idx] - 7 * nr - 40 * collide)
    return out


def diving_scores(gpu: str, combo: int) -> list[int]:
    goal = gpu[0] if gpu else "U"
    out = []
    for _n, _dx, _dy, letter in MOVES:
        if letter == goal:
            out.append(10 * (8 + min(combo + 1, 10)))
        else:
            out.append(10 * (-min(combo, 10) - 1))
    return out


def main() -> None:
    player = int(rd())
    nb = int(rd())
    while True:
        infos = [rd() for _ in range(3)]
        if not infos[0]:
            return
        games = [rd().split() for _ in range(nb)]
        total = [0] * 4
        for f in games:
            if len(f) < 8 or f[0] == "GAME_OVER":
                continue
            gpu = f[0]
            r = [int(v) for v in f[1:8]]
            if len(gpu) >= 10 and all(c in ".#" for c in gpu):
                vals = hurdle_scores(gpu, r[player], r[3 + player])
                for a in range(4):
                    total[a] += 3 * vals[a]
            elif gpu.isdigit():
                vals = archery_scores(gpu, r[2 * player], r[2 * player + 1])
                for a in range(4):
                    total[a] += vals[a]
            elif len(gpu) == 4 and all(c in "ULDR" for c in gpu) and r[6] >= 0:
                others = [r[i] for i in range(3) if i != player]
                vals = roller_scores(gpu, r[player], r[3 + player], r[6], others)
                for a in range(4):
                    total[a] += 2 * vals[a]
            elif gpu and all(c in "ULDR" for c in gpu):
                vals = diving_scores(gpu, r[3 + player])
                for a in range(4):
                    total[a] += 2 * vals[a]
        best = max(range(4), key=lambda a: (total[a], -a))
        print(NAMES[best], flush=True)


if __name__ == "__main__":
    main()
