#!/usr/bin/env python3
"""Smash The Code — bot Wood league.

Both players own a 6x12 grid. Each turn a vertical pair of blocks falls in a
chosen column; groups of 4+ same-coloured cells disappear, skulls (`0`) next
to a cleared group go with them, then gravity may create chains.

We simulate the six possible drops and keep the best one.
"""
from __future__ import annotations

import sys
from collections import deque
from dataclasses import dataclass

WIDTH = 6
HEIGHT = 12
EMPTY = "."


@dataclass(frozen=True)
class Outcome:
    """Result of resolving a grid after a drop."""

    cleared: int
    chains: int


def drop(grid: list[list[str]], col: int, top: str, bottom: str) -> bool:
    """Drop a vertical pair into `col`; return False if the grid tops out."""
    row = next((r for r in range(HEIGHT) if grid[r][col] != EMPTY), HEIGHT)
    if row < 2:
        return False
    grid[row - 1][col] = bottom
    grid[row - 2][col] = top
    return True


def resolve(grid: list[list[str]]) -> Outcome:
    """Clear every group of 4+, apply gravity, repeat until stable."""
    total = 0
    chains = 0
    while True:
        visited = [[False] * WIDTH for _ in range(HEIGHT)]
        to_clear = [[False] * WIDTH for _ in range(HEIGHT)]
        found = False
        for row in range(HEIGHT):
            for col in range(WIDTH):
                colour = grid[row][col]
                if colour == EMPTY or colour == "0" or visited[row][col]:
                    continue
                stack = [(row, col)]
                visited[row][col] = True
                group: list[tuple[int, int]] = []
                while stack:
                    r, c = stack.pop()
                    group.append((r, c))
                    for dr, dc in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        nr, nc = r + dr, c + dc
                        if (
                            0 <= nr < HEIGHT
                            and 0 <= nc < WIDTH
                            and not visited[nr][nc]
                            and grid[nr][nc] == colour
                        ):
                            visited[nr][nc] = True
                            stack.append((nr, nc))
                if len(group) >= 4:
                    found = True
                    for r, c in group:
                        to_clear[r][c] = True
        if not found:
            break

        # A skull is removed when it touches a coloured group being cleared.
        for row in range(HEIGHT):
            for col in range(WIDTH):
                if grid[row][col] != "0":
                    continue
                for dr, dc in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nr, nc = row + dr, col + dc
                    if 0 <= nr < HEIGHT and 0 <= nc < WIDTH and to_clear[nr][nc]:
                        to_clear[row][col] = True
                        break

        for row in range(HEIGHT):
            for col in range(WIDTH):
                if to_clear[row][col]:
                    grid[row][col] = EMPTY
                    total += 1
        chains += 1

        # Gravity: compact each column towards the bottom, keeping order.
        for col in range(WIDTH):
            cells = [grid[r][col] for r in range(HEIGHT) if grid[r][col] != EMPTY]
            first = HEIGHT - len(cells)
            for row in range(HEIGHT):
                grid[row][col] = cells[row - first] if row >= first else EMPTY

    return Outcome(cleared=total, chains=chains)


def height(grid: list[list[str]]) -> int:
    """Height of the tallest column, counting from the bottom."""
    return max(
        sum(1 for r in range(HEIGHT) if grid[r][c] != EMPTY) for c in range(WIDTH)
    )


def evaluate(grid: list[list[str]], col: int, a: str, b: str) -> int | None:
    """Score a candidate drop, or None when it tops out."""
    trial = [row[:] for row in grid]
    if not drop(trial, col, a, b):
        return None
    outcome = resolve(trial)
    return outcome.cleared * 100 + outcome.chains * 50 - height(trial) * 2


def choose_column(grid: list[list[str]], pairs: list[tuple[str, str]]) -> int:
    """Pick the best column for the next pair, deterministically."""
    a, b = pairs[0]
    best_col, best_score = 0, None
    for col in range(WIDTH):
        score = evaluate(grid, col, a, b)
        if score is None:
            continue
        if best_score is None or score > best_score:
            best_col, best_score = col, score
    return best_col


def main() -> None:
    # Lecture ligne par ligne : CodinGame envoie les tours progressivement,
    # un read() global attendrait la fin du flux et provoquerait un timeout.
    def line() -> str:
        return sys.stdin.readline().strip()

    # Le jeu tourne en boucle : un tour = une entrée complète + une sortie.
    while True:
        # Le nombre de paires varie : le jeu envoie les paires restantes (8 au
        # début, puis de moins en moins). La ligne de score est celle qui ne
        # contient qu'un seul entier.
        pairs: list[tuple[str, str]] = []
        while True:
            parts = line().split()
            if not parts:
                return  # fin du flux (tests locaux)
            if len(parts) == 1:
                break  # score propre
            if len(parts) >= 2:
                pairs.append((parts[0], parts[1]))
        if not pairs:
            return
        grid = [list(line()) for _ in range(HEIGHT)]
        line()  # opponent score
        for _ in range(HEIGHT):  # opponent grid, unused in Wood
            line()

        print(choose_column(grid, pairs), flush=True)


if __name__ == "__main__":
    main()
