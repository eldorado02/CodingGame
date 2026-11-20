#!/usr/bin/env python3
"""Olymbits — Hurdle Race : chemin minimal sans jamais heurter un obstacle."""
import sys
from collections import deque

# LEFT=+1, DOWN=+2, RIGHT=+3, UP=+2 en sautant la première case.
ACTIONS = (
    ("LEFT", 1, False),
    ("DOWN", 2, False),
    ("RIGHT", 3, False),
    ("UP", 2, True),
)


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def move(track: str, pos: int, steps: int, jump: bool) -> int | None:
    """Position après l'action, ou None si le coureur heurte une haie."""
    last = len(track) - 1
    np = pos
    for k in range(1, steps + 1):
        np = min(last, np + 1)
        if track[np] == "#" and not jump:
            return None
        if np == last:
            break
        jump = False
    return np


def best_action(track: str, pos: int) -> str:
    """Premier pas d'un plus court chemin vers la dernière case."""
    goal = len(track) - 1
    if pos >= goal:
        return "LEFT"
    q = deque([pos])
    first = {pos: ""}
    while q:
        p = q.popleft()
        for name, steps, jump in ACTIONS:
            np = move(track, p, steps, jump)
            if np is None or np in first:
                continue
            first[np] = name if p == pos else first[p]
            if np == goal:
                return first[np]
            q.append(np)
    return "LEFT"


def main() -> None:
    player_idx = int(rd())
    nb_games = int(rd())
    while True:
        info = [rd() for _ in range(3)]
        if not info[0]:
            return
        regs = [rd() for _ in range(nb_games)]
        if not regs:
            print("LEFT")
            sys.stdout.flush()
            continue
        f = regs[0].split()
        if not f or f[0] == "GAME_OVER":
            print("LEFT")
            sys.stdout.flush()
            continue
        track = f[0]
        pos = int(f[1 + player_idx])
        print(best_action(track, pos))
        sys.stdout.flush()


if __name__ == "__main__":
    main()
