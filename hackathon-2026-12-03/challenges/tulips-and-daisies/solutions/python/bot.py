#!/usr/bin/env python3
"""Tulips and Daisies — Python reference of the differential fuzz, v3 window + gap greedy.

Planting costs by tile (soil / grass / rocks / enemy flower, init line; never
on our own flower). A planted flower harvests every run of >= 4 same flowers
through it; runs of different directions only share the planted cell, so N =
1 + sum(len - 1) and the gold is the sum of the first N Fibonacci numbers.
Move score, all evaluated with the planted cell overridden (no grid copy):
  NET_WEIGHT * (harvest - cost)
  - THREAT_WEIGHT * best opponent harvest - cost on the cells where they
    harvested before our move (our move cannot create their lines)
  + every 4-cell window through the cell: ATTACK[our flowers] when it holds
    no enemy flower, DEFENCE[their flowers] when it held none of ours
  + GAP_WEIGHT * best harvest - cost reachable by filling one cell next to
    our new run (runs of 3 + gap + 3 pay 33 instead of 7: the boss does it).
Gold: keep RESERVE_TURNS grass plantings (a cost-free move needs soil, and
running out of gold loses); when no move keeps the reserve, the cheapest
affordable move. Ties: row-major order.
"""
import sys

DIRECTIONS = ((0, 1), (1, 0), (1, 1), (1, -1))
ROW = 4                         # flowers in a line to harvest
NET_WEIGHT = 10
THREAT_WEIGHT = 10
ATTACK = (0, 1, 4, 16, 64)      # by our flowers in a clean window
DEFENCE = (0, 0, 2, 12, 48)     # by their flowers in a window we block
RESERVE_TURNS = 10
GAP_WEIGHT = 5                  # a gap harvest is worth half of a sure one
SOIL, GRASS, ROCKS = "S", "G", "R"


def fib_sum(n):
    total, a, b = 0, 1, 1
    for _ in range(n):
        total += a
        a, b = b, a + b
    return total


class Field:
    def __init__(self, width, height, costs, mine, theirs):
        self.width, self.height = width, height
        self.cost_soil, self.cost_grass, self.cost_rocks, self.cost_flower = costs
        self.mine, self.theirs = mine, theirs
        self.grid = []
        self.placed = None      # (row, col) holding our flower in the current hypothesis

    def inside(self, r, c):
        return 0 <= r < self.height and 0 <= c < self.width

    def at(self, r, c):
        return self.mine if (r, c) == self.placed else self.grid[r][c]

    def cost(self, tile, flower):
        """Planting cost of `flower` on `tile`, None when it is not allowed."""
        if tile == flower:
            return None
        if tile == SOIL:
            return self.cost_soil
        if tile == GRASS:
            return self.cost_grass
        if tile == ROCKS:
            return self.cost_rocks
        return self.cost_flower

    def harvest(self, r, c, flower):
        """Gold earned by planting `flower` at (r, c)."""
        count, harvested = 1, False
        for dr, dc in DIRECTIONS:
            length = 1
            for sign in (1, -1):
                nr, nc = r + sign * dr, c + sign * dc
                while self.inside(nr, nc) and self.at(nr, nc) == flower:
                    length += 1
                    nr, nc = nr + sign * dr, nc + sign * dc
            if length >= ROW:
                count += length - 1
                harvested = True
        return fib_sum(count) if harvested else 0

    def run_end(self, r, c, dr, dc):
        """Our flowers after (r, c) along (dr, dc) and the first other cell."""
        count = 0
        r, c = r + dr, c + dc
        while self.inside(r, c) and self.at(r, c) == self.mine:
            count += 1
            r, c = r + dr, c + dc
        return count, r, c

    def gap_potential(self, r, c):
        """Best harvest - cost of filling one cell next to our run through (r, c)."""
        best = 0
        for dr, dc in DIRECTIONS:
            forward, fr, fc = self.run_end(r, c, dr, dc)
            backward, br, bc = self.run_end(r, c, -dr, -dc)
            length = 1 + forward + backward
            for er, ec, sr, sc in ((fr, fc, dr, dc), (br, bc, -dr, -dc)):
                if not self.inside(er, ec):
                    continue
                beyond, _, _ = self.run_end(er, ec, sr, sc)
                total = length + beyond + 1
                if total >= ROW:
                    best = max(best, fib_sum(total) - self.cost(self.at(er, ec), self.mine))
        return best

    def reply(self, threats, gold):
        """Best opponent harvest - cost among the threat cells (0 when none pays)."""
        best = 0
        for r, c in threats:
            cost = self.cost(self.at(r, c), self.theirs)
            if cost is None or cost > gold:
                continue
            earned = self.harvest(r, c, self.theirs)
            if earned and earned - cost > best:
                best = earned - cost
        return best

    def windows(self, r, c):
        score = 0
        for dr, dc in DIRECTIONS:
            for shift in range(ROW):
                cells = [(r + (k - shift) * dr, c + (k - shift) * dc) for k in range(ROW)]
                if not all(self.inside(cr, cc) for cr, cc in cells):
                    continue
                ours = sum(1 for cr, cc in cells if self.at(cr, cc) == self.mine)
                theirs = sum(1 for cr, cc in cells if self.at(cr, cc) == self.theirs)
                before_ours = sum(1 for cr, cc in cells if (cr, cc) != (r, c) and self.grid[cr][cc] == self.mine)
                if theirs == 0:
                    score += ATTACK[ours]
                if before_ours == 0:
                    score += DEFENCE[sum(1 for cr, cc in cells if self.grid[cr][cc] == self.theirs)]
        return score

    def threats(self, gold):
        self.placed = None
        out = []
        for r in range(self.height):
            for c in range(self.width):
                cost = self.cost(self.grid[r][c], self.theirs)
                if cost is not None and cost <= gold and self.harvest(r, c, self.theirs):
                    out.append((r, c))
        return out

    def choose(self, grid, gold, opponent_gold):
        self.grid = grid
        threats = self.threats(opponent_gold)
        reserve = min(gold, RESERVE_TURNS * self.cost_grass)
        best = cheapest = None
        for r in range(self.height):
            for c in range(self.width):
                cost = self.cost(grid[r][c], self.mine)
                if cost is None or cost > gold:
                    continue
                self.placed = None
                earned = self.harvest(r, c, self.mine)
                self.placed = (r, c)
                score = (NET_WEIGHT * (earned - cost) - THREAT_WEIGHT * self.reply(threats, opponent_gold)
                         + self.windows(r, c) + (0 if earned else GAP_WEIGHT * self.gap_potential(r, c)))
                key = (score, (r, c))
                if cost == 0 or gold - cost + earned >= reserve:
                    if best is None or score > best[0]:
                        best = key
                if cheapest is None or (cost, -score) < cheapest[0]:
                    cheapest = ((cost, -score), (r, c))
        if best is not None:
            return best[1]
        return cheapest[1] if cheapest else (0, 0)


def main():
    read = sys.stdin.readline
    first = read()
    if not first:
        return
    width, height = (int(v) for v in first.split()[:2])
    costs = tuple(int(v) for v in read().split()[:4])
    mine, theirs = (name[0].upper() for name in read().split()[:2])
    field = Field(width, height, costs, mine, theirs)
    while True:
        if not read().strip():
            return
        gold, opponent_gold = (int(v) for v in read().split()[:2])
        grid = [list(read().strip().ljust(width, SOIL)[:width]) for _ in range(height)]
        r, c = field.choose(grid, gold, opponent_gold)
        print("%d %d" % (r, c), flush=True)


if __name__ == "__main__":
    main()
