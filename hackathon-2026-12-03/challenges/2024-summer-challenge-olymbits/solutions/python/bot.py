#!/usr/bin/env python3
"""Olymbits (Wood 1, 4 courses simultanées) — énumération de séquences courtes.

Toutes les machines reçoivent la MÊME action à chaque tour. On énumère les
4^5 séquences d'actions possibles, on simule les 4 courses sur 5 tours, et on
choisit la séquence qui minimise la somme des tours restants estimés. On joue
le premier pas de cette séquence.

`UP` saute la première haie et avance de 2 ; heurter une haie immobilise 3 tours.
"""
import itertools
import sys

MOVES = ((1, False), (2, False), (3, False), (2, True))  # LEFT DOWN RIGHT UP
NAMES = ("LEFT", "DOWN", "RIGHT", "UP")
HORIZON = 5


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def step(track: str, pos: int, stun: int, steps: int, jump: bool) -> tuple[int, int]:
    """Applique une action ; renvoie (position, stun)."""
    if stun > 0:
        return pos, stun - 1
    last = len(track) - 1
    if jump:
        np = min(last, pos + 2)
        if pos + 2 < last and track[pos + 2] == "#":
            return np, 3
        return np, 0
    np = pos
    for s in range(1, steps + 1):
        if pos + s > last:
            return last, 0
        if track[pos + s] == "#":
            return pos + s, 3
        np = pos + s
    return np, 0


def main() -> None:
    player = int(rd())
    nb = int(rd())
    while True:
        infos = [rd() for _ in range(3)]
        if not infos[0]:
            return
        games = [rd().split() for _ in range(nb)]
        states = []
        for f in games:
            if len(f) < 8 or f[0] == "GAME_OVER":
                states.append(None)
                continue
            states.append((f[0], int(f[1 + player]), int(f[4 + player])))

        best = None
        best_seq = None
        for seq in itertools.product(range(4), repeat=HORIZON):
            total = 0
            for st in states:
                if st is None:
                    continue
                track, pos, stun = st
                done = -1
                for t, m in enumerate(seq):
                    pos, stun = step(track, pos, stun, *MOVES[m])
                    if pos >= len(track) - 1:
                        done = t + 1
                        break
                if done < 0:
                    done = HORIZON + (len(track) - 1 - pos)
                total += done
            if best is None or total < best:
                best = total
                best_seq = seq
                if best == 0:
                    break
        print(NAMES[best_seq[0]] if best_seq else "LEFT", flush=True)


if __name__ == "__main__":
    main()
