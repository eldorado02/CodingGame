#!/usr/bin/env python3
"""Clash of Bots — Python reference of the differential fuzz, v1 local rules.

Facts from the referee (stivens/Clash-of-Bots): 15x15 torus, 100 turns, 10 hp,
attack 2, collision 1 (both robots), explosion 4 on the 3x3 square, guard
halves damage; moves resolve before attacks; chains of moves are legal, a
contested or swapped cell is a collision. A robot only sees its 5x5 minimap
(row 0 = up) and the input order is shuffled, so every robot decides alone.

Rules, first match wins:
1. explode when it kills at least KILLS_TO_EXPLODE enemies and hits no ally;
2. attack the weakest enemy on a side cell;
3. attack the side cell that most enemies at distance 2 can step into;
4. step towards the nearest visible enemy;
5. no enemy in sight: drift in DRIFT direction (the whole army keeps its
   shape), guard when an ally stands in the way.
"""
import sys

SIZE = 5
CENTER = 2
EXPLOSION_DAMAGE = 4
KILLS_TO_EXPLODE = 2
# (name, row delta, column delta), in tie-break order
DIRECTIONS = (("UP", -1, 0), ("DOWN", 1, 0), ("LEFT", 0, -1), ("RIGHT", 0, 1))
DRIFT = DIRECTIONS[0]


def cells(grid, predicate):
    """(row, col, value) of every cell but the center matching predicate(value)."""
    return [(r, c, grid[r][c]) for r in range(SIZE) for c in range(SIZE)
            if (r, c) != (CENTER, CENTER) and predicate(grid[r][c])]


def manhattan(r, c):
    return abs(r - CENTER) + abs(c - CENTER)


def explosion_pays(grid):
    near = [(r, c, v) for r, c, v in cells(grid, lambda v: v != 0)
            if abs(r - CENTER) <= 1 and abs(c - CENTER) <= 1]
    if any(v > 0 for _, _, v in near):
        return False
    kills = sum(1 for _, _, v in near if -v <= EXPLOSION_DAMAGE)
    return kills >= KILLS_TO_EXPLODE


def adjacent_attack(grid):
    best, best_hp = None, None
    for name, dr, dc in DIRECTIONS:
        v = grid[CENTER + dr][CENTER + dc]
        if v < 0 and (best_hp is None or -v < best_hp):
            best, best_hp = name, -v
    return best


def ambush_attack(grid):
    """Side cell (empty) that the most enemies at distance 2 can move into."""
    enemies = [(r, c) for r, c, _ in cells(grid, lambda v: v < 0) if manhattan(r, c) == 2]
    best, best_count = None, 0
    for name, dr, dc in DIRECTIONS:
        r, c = CENTER + dr, CENTER + dc
        if grid[r][c] != 0:
            continue
        count = sum(1 for er, ec in enemies if abs(er - r) + abs(ec - c) == 1)
        if count > best_count:
            best, best_count = name, count
    return best


def approach(grid):
    """First free side cell that brings us closer to the nearest enemy."""
    enemies = sorted((manhattan(r, c), r, c) for r, c, _ in cells(grid, lambda v: v < 0))
    if not enemies:
        return None
    _, er, ec = enemies[0]
    for name, dr, dc in DIRECTIONS:
        r, c = CENTER + dr, CENTER + dc
        if grid[r][c] == 0 and abs(er - r) + abs(ec - c) < abs(er - CENTER) + abs(ec - CENTER):
            return name
    return None


def decide(grid):
    if explosion_pays(grid):
        return "SELFDESTRUCTION"
    target = adjacent_attack(grid) or ambush_attack(grid)
    if target:
        return "ATTACK " + target
    step = approach(grid)
    if step:
        return "MOVE " + step
    name, dr, dc = DRIFT
    if not cells(grid, lambda v: v < 0) and grid[CENTER + dr][CENTER + dc] == 0:
        return "MOVE " + name
    return "GUARD"


def main():
    read = sys.stdin.readline
    while True:
        line = read()
        if not line.strip():
            return
        commands = []
        for _ in range(int(line)):
            grid = [[int(v) for v in read().split()[:SIZE]] for _ in range(SIZE)]
            commands.append(decide(grid))
        sys.stdout.write("".join(cmd + "\n" for cmd in commands))
        sys.stdout.flush()


if __name__ == "__main__":
    main()
