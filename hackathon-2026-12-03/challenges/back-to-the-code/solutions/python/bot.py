#!/usr/bin/env python3
"""Back to the Code — Python reference of the differential fuzz, v1 rectangle loops.

A loop of our cells around neutral cells claims them. Each turn we keep (or
pick) a rectangle that has no opponent cell, contains us on its border and
maximises claimed cells / border cells still to walk (2D prefix sums make
each candidate O(1)); an opponent closer to it than half of that walk makes
it too risky. We walk its border clockwise, one corner after the other (the
referee moves one cell towards the target, along the row or column we share
with it). No rectangle: nearest neutral cell. BACK is never used.
"""
import sys

WIDTH, HEIGHT = 35, 20
MAX_SIDE = 12           # bigger loops get cut by the opponents
NEUTRAL, MINE = ".", "0"


class Prefix:
    """Count of cells matching a predicate in any rectangle, O(1)."""

    def __init__(self, grid, predicate):
        self.sum = [[0] * (WIDTH + 1) for _ in range(HEIGHT + 1)]
        for y in range(HEIGHT):
            for x in range(WIDTH):
                self.sum[y + 1][x + 1] = (self.sum[y][x + 1] + self.sum[y + 1][x] - self.sum[y][x]
                                          + (1 if predicate(grid[y][x]) else 0))

    def count(self, x0, y0, x1, y1):
        if x0 > x1 or y0 > y1:
            return 0
        s = self.sum
        return s[y1 + 1][x1 + 1] - s[y0][x1 + 1] - s[y1 + 1][x0] + s[y0][x0]


def distance_to_rect(px, py, rect):
    x0, y0, x1, y1 = rect
    return max(x0 - px, 0, px - x1) + max(y0 - py, 0, py - y1)


def rect_value(rect, neutral, mine, foe, opponents):
    """(gain, cost) of claiming this rectangle, or None when it is not worth it."""
    x0, y0, x1, y1 = rect
    if foe.count(x0, y0, x1, y1):
        return None
    gain = neutral.count(x0, y0, x1, y1)
    border = (x1 - x0 + 1) * (y1 - y0 + 1) - (x1 - x0 - 1) * (y1 - y0 - 1) if x1 > x0 and y1 > y0 \
        else (x1 - x0 + 1) * (y1 - y0 + 1)
    inner_mine = mine.count(x0 + 1, y0 + 1, x1 - 1, y1 - 1)
    cost = border - (mine.count(x0, y0, x1, y1) - inner_mine)
    if gain == 0 or cost == 0:
        return None
    for ox, oy in opponents:
        if 2 * distance_to_rect(ox, oy, rect) < cost:   # they could cut the loop
            return None
    return gain, cost


def best_rect(me, neutral, mine, foe, opponents):
    """Rectangle with us on its border maximising gain / cost; ties: bigger gain."""
    mx, my = me
    best, best_gain, best_cost = None, 0, 1
    for y0 in range(max(0, my - MAX_SIDE + 1), my + 1):
        for y1 in range(my, min(HEIGHT, y0 + MAX_SIDE)):
            for x0 in range(max(0, mx - MAX_SIDE + 1), mx + 1):
                for x1 in range(mx, min(WIDTH, x0 + MAX_SIDE)):
                    if mx not in (x0, x1) and my not in (y0, y1):
                        continue
                    rect = (x0, y0, x1, y1)
                    value = rect_value(rect, neutral, mine, foe, opponents)
                    if value is None:
                        continue
                    gain, cost = value
                    if best is None or gain * best_cost > best_gain * cost or \
                            (gain * best_cost == best_gain * cost and gain > best_gain):
                        best, best_gain, best_cost = rect, gain, cost
    return best


def next_corner(me, rect):
    """Clockwise corner to aim at from our cell on the border."""
    x, y = me
    x0, y0, x1, y1 = rect
    if y == y0 and x < x1:
        return x1, y0
    if x == x1 and y < y1:
        return x1, y1
    if y == y1 and x > x0:
        return x0, y1
    return x0, y0


def on_border(me, rect):
    x, y = me
    x0, y0, x1, y1 = rect
    return x0 <= x <= x1 and y0 <= y <= y1 and (x in (x0, x1) or y in (y0, y1))


def nearest_neutral(me, grid):
    best, best_d = me, None
    for y in range(HEIGHT):
        for x in range(WIDTH):
            d = abs(x - me[0]) + abs(y - me[1])
            if grid[y][x] == NEUTRAL and (best_d is None or d < best_d):
                best, best_d = (x, y), d
    return best


def decide(me, opponents, grid, plan):
    """Target cell; plan[0] holds the rectangle followed across turns."""
    neutral = Prefix(grid, lambda ch: ch == NEUTRAL)
    mine = Prefix(grid, lambda ch: ch == MINE)
    foe = Prefix(grid, lambda ch: ch not in (NEUTRAL, MINE))
    rect = plan[0]
    if rect is None or not on_border(me, rect) or rect_value(rect, neutral, mine, foe, opponents) is None:
        rect = best_rect(me, neutral, mine, foe, opponents)
    plan[0] = rect
    if rect is None:
        return nearest_neutral(me, grid)
    return next_corner(me, rect)


def main():
    read = sys.stdin.readline
    first = read()
    if not first:
        return
    opponent_count = int(first)
    plan = [None]
    while True:
        if not read():
            return
        x, y = (int(v) for v in read().split()[:2])
        opponents = []
        for _ in range(opponent_count):
            ox, oy = (int(v) for v in read().split()[:2])
            if ox >= 0:
                opponents.append((ox, oy))
        grid = [read().strip().ljust(WIDTH, NEUTRAL)[:WIDTH] for _ in range(HEIGHT)]
        tx, ty = decide((x, y), opponents, grid, plan)
        print("%d %d" % (tx, ty), flush=True)


if __name__ == "__main__":
    main()
