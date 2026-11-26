#!/usr/bin/env python3
"""Volcanoes — Python reference of the differential fuzz, v1 connection distance.

Tiles and neighbours come from the init; each turn the levels of the 80
tiles (ours > 0, theirs < 0) and the valid moves (tile names). A move puts a
level-1 volcano on an empty tile or raises one of ours; at level 4 it erupts
(cascade, each tile once): empty neighbours become ours at level 1, our
neighbours gain a level, enemy neighbours are destroyed.
Connection distance of a side = min over k of the 0-1 BFS cost from N<k> to
S<k> (own volcano 0, empty 1, enemy blocked). Score = DIST_WEIGHT *
(their distance - ours) + total level of our volcanoes. Ties: first move.
"""
import sys
from collections import deque

MAX_LEVEL = 4
DIST_WEIGHT = 100
BLOCKED = 1000          # no chain possible


class Board:
    def __init__(self, names, neighbours):
        self.names = names
        self.index = {n: i for i, n in enumerate(names)}
        self.neighbours = neighbours
        self.pairs = []
        for i, name in enumerate(names):
            if name.startswith("N") and ("S" + name[1:]) in self.index:
                self.pairs.append((i, self.index["S" + name[1:]]))

    def play(self, levels, tile):
        """Levels after our move on `tile` (cascading eruptions)."""
        after = list(levels)
        after[tile] = after[tile] + 1 if after[tile] > 0 else 1
        erupted = set()
        queue = deque([tile] if after[tile] >= MAX_LEVEL else [])
        while queue:
            t = queue.popleft()
            if t in erupted:
                continue
            erupted.add(t)
            for n in self.neighbours[t]:
                if after[n] == 0:
                    after[n] = 1
                elif after[n] > 0 and n not in erupted:
                    after[n] = min(after[n] + 1, MAX_LEVEL)
                    if after[n] >= MAX_LEVEL:
                        queue.append(n)
                elif after[n] < 0:
                    after[n] = 0
        return after

    def distance(self, levels, sign):
        """Fewest empty tiles to fill for a N<k>-S<k> chain of `sign` volcanoes."""
        best = BLOCKED
        for north, south in self.pairs:
            cost = self.cost(levels, sign, north)
            if cost is None:
                continue
            dist = {north: cost}
            queue = deque([north])
            while queue:
                t = queue.popleft()
                for n in self.neighbours[t]:
                    step = self.cost(levels, sign, n)
                    if step is None or dist[t] + step >= dist.get(n, BLOCKED):
                        continue
                    dist[n] = dist[t] + step
                    if step == 0:
                        queue.appendleft(n)
                    else:
                        queue.append(n)
            best = min(best, dist.get(south, BLOCKED))
        return best

    @staticmethod
    def cost(levels, sign, tile):
        v = levels[tile] * sign
        if v > 0:
            return 0
        return 1 if v == 0 else None

    def choose(self, levels, moves):
        best, best_score = None, None
        for name in moves:
            tile = self.index.get(name)
            if tile is None:
                continue
            after = self.play(levels, tile)
            score = (DIST_WEIGHT * (self.distance(after, -1) - self.distance(after, 1))
                     + sum(v for v in after if v > 0))
            if best_score is None or score > best_score:
                best, best_score = name, score
        return best or (moves[0] if moves else "RANDOM")


def main():
    read = sys.stdin.readline
    first = read()
    if not first.strip():
        return
    names, neighbours = [], []
    for _ in range(int(first)):
        parts = read().split()
        names.append(parts[0])
        neighbours.append([int(v) for v in parts[1:4]])
    board = Board(names, neighbours)
    while True:
        position = read()
        if not position.strip():
            return
        levels = [int(v) for v in position.split()][:len(names)]
        levels += [0] * (len(names) - len(levels))
        moves = read().split()
        print(board.choose(levels, moves), flush=True)


if __name__ == "__main__":
    main()
