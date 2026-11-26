#!/usr/bin/env python3
"""Vindinium — Python reference of the differential fuzz, v2 mines, beer and fights.

Grid: board rows (`grid[row][col]`). Whether an entity's `x` is the column or
the row is decided on the first turn: our hero stands on its spawn digit.
Walkable: '.', spawn digits, minus cells holding another hero. Mines and
taverns are entered from a neighbour (the hero stays in place).
Cells next to an enemy with at least our life are avoided (it hits 20 at the
end of its turn and takes our mines when we die).
Turn:
1. kill: a weak enemy (life <= KILL_LIFE) owning mines, a step from us;
2. beer: life <= LOW_LIFE, or a tavern next to us and life <= TOP_UP (2 gold);
3. else the closest mine we do not own if life - path - MINE_FIGHT > SAFETY;
4. else the closest tavern; nothing reachable: WAIT.
Paths by BFS (first steps), ties: smaller target (row, col), then N, E, S, W.
"""
import sys
from collections import deque

LOW_LIFE = 50
KILL_LIFE = 20             # one hit kills
KILL_REACH = 2            # moves to stand next to the prey
TOP_UP = 80
MINE_FIGHT = 20
SAFETY = 15
BEER_PRICE = 2
WALKABLE = ".0123"
TAVERN, MINE = "T", "M"
MOVES = (("NORTH", -1, 0), ("EAST", 0, 1), ("SOUTH", 1, 0), ("WEST", 0, -1))


class Game:
    def __init__(self, grid, me):
        self.grid = grid
        self.size = len(grid)
        self.me = me
        self.x_is_column = None

    def inside(self, r, c):
        return 0 <= r < self.size and 0 <= c < self.size

    def cell(self, x, y):
        """(row, col) of an entity position."""
        return (y, x) if self.x_is_column else (x, y)

    def detect_axes(self, x, y):
        if self.x_is_column is not None:
            return
        spawn = str(self.me)
        self.x_is_column = not (self.inside(x, y) and self.grid[x][y] == spawn and
                                not (self.inside(y, x) and self.grid[y][x] == spawn))

    def bfs(self, start, blocked):
        """Distance and first move of every walkable cell reachable from start."""
        if not self.inside(*start):
            return {}
        reach = {start: (0, None)}
        queue = deque([start])
        while queue:
            r, c = queue.popleft()
            dist, first = reach[(r, c)]
            for name, dr, dc in MOVES:
                n = (r + dr, c + dc)
                if n in reach or not self.inside(*n) or self.grid[n[0]][n[1]] not in WALKABLE or n in blocked:
                    continue
                reach[n] = (dist + 1, first or name)
                queue.append(n)
        return reach

    def closest(self, reach, targets):
        """(cost, move) to step into the best target cell, or None."""
        best = None
        for t in sorted(targets):
            for name, dr, dc in MOVES:
                n = (t[0] - dr, t[1] - dc)          # we step from n into t with `name`
                if n not in reach:
                    continue
                dist, first = reach[n]
                key = (dist + 1, t)
                if best is None or key < best[0]:
                    best = (key, first or name)
        return (best[0][0], best[1]) if best else None

    def danger_zone(self, heroes, hero, pos):
        """Cells next to an enemy at least as healthy as us (never our own cell)."""
        zone = set()
        for i, h in heroes.items():
            if i == self.me or h["life"] < hero["life"]:
                continue
            r, c = self.cell(h["x"], h["y"])
            for _, dr, dc in MOVES:
                zone.add((r + dr, c + dc))
        zone.discard(pos)
        return zone

    def prey_move(self, heroes, mines, reach):
        """First move towards a cell next to a weak enemy owning mines, or None."""
        owned = {m["owner"] for m in mines}
        best = None
        for i in sorted(heroes):
            h = heroes[i]
            if i == self.me or h["life"] > KILL_LIFE or i not in owned:
                continue
            r, c = self.cell(h["x"], h["y"])
            for name, dr, dc in MOVES:
                n = (r + dr, c + dc)
                if n in reach and reach[n][0] <= KILL_REACH and (best is None or reach[n][0] < best[0]):
                    best = (reach[n][0], reach[n][1] or "WAIT")
        return best[1] if best else None

    def decide(self, heroes, mines):
        hero = heroes[self.me]
        pos = self.cell(hero["x"], hero["y"])
        blocked = {self.cell(h["x"], h["y"]) for i, h in heroes.items() if i != self.me}
        prey = self.prey_move(heroes, mines, self.bfs(pos, blocked))
        if prey:
            return prey
        reach = self.bfs(pos, blocked | self.danger_zone(heroes, hero, pos))
        taverns = [(r, c) for r in range(self.size) for c in range(self.size) if self.grid[r][c] == TAVERN]
        to_tavern = self.closest(reach, taverns)
        can_pay = hero["gold"] >= BEER_PRICE
        if to_tavern and can_pay and (hero["life"] <= LOW_LIFE or (to_tavern[0] == 1 and hero["life"] <= TOP_UP)):
            return to_tavern[1]
        targets = [self.cell(m["x"], m["y"]) for m in mines if m["owner"] != self.me]
        to_mine = self.closest(reach, targets)
        if to_mine and hero["life"] - to_mine[0] - MINE_FIGHT > SAFETY:
            return to_mine[1]
        if to_tavern and can_pay:
            return to_tavern[1]
        return to_mine[1] if to_mine else "WAIT"


def main():
    read = sys.stdin.readline
    first = read()
    if not first.strip():
        return
    size = int(first)
    grid = [read().strip().ljust(size, "#")[:size] for _ in range(size)]
    game = Game(grid, int(read()))
    while True:
        line = read()
        if not line.strip():
            return
        heroes, mines = {}, []
        for _ in range(int(line)):
            kind, ident, x, y, life, gold = read().split()[:6]
            entity = {"owner": int(ident), "x": int(x), "y": int(y), "life": int(life), "gold": int(gold)}
            if kind == "HERO":
                heroes[int(ident)] = entity
            else:
                mines.append(entity)
        if game.me not in heroes:
            print("WAIT", flush=True)
            continue
        game.detect_axes(heroes[game.me]["x"], heroes[game.me]["y"])
        print(game.decide(heroes, mines), flush=True)


if __name__ == "__main__":
    main()
