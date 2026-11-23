#!/usr/bin/env python3
"""Amazons 8x8 — Python reference, v1 one-ply territory search.

An action = move an amazon like a queen, then shoot a wall like a queen
from its landing square. No legal action = loss. The legal list is NOT
given (only its count), so actions are generated here.
Eval (after our action): queen-distance territory — cells reached in fewer
queen moves by us minus by them (ties neutral) — plus MOBILITY_WEIGHT *
(our queen moves - theirs). The search is two-staged in generation
order: all queen moves are ranked without arrow, the TOP_MOVES best get
every arrow tried (deterministic, ~300 evaluations per turn).
Index = y * n + x, y = 0 is the first board line (top row = rank n).
"""
import os
import sys

TOP_MOVES = 10          # moves kept for the arrow stage (env CG_NODES overrides)
MOBILITY_WEIGHT = 1
DIRS = ((-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (0, 1), (1, 1))
EMPTY, WALL = ".", "-"


class Board:
    def __init__(self, n):
        self.n = n
        # rays[i][d] = cells along direction d from i (excluding i)
        self.rays = []
        for i in range(n * n):
            x, y = i % n, i // n
            per_dir = []
            for dx, dy in DIRS:
                ray, cx, cy = [], x + dx, y + dy
                while 0 <= cx < n and 0 <= cy < n:
                    ray.append(cy * n + cx)
                    cx += dx
                    cy += dy
                per_dir.append(ray)
            self.rays.append(per_dir)

    def reach(self, cells, start):
        """Free cells reachable by one queen move from start."""
        out = []
        for ray in self.rays[start]:
            for t in ray:
                if cells[t] != EMPTY:
                    break
                out.append(t)
        return out

    def queen_distance(self, cells, sources):
        dist = [-1] * (self.n * self.n)
        frontier = list(sources)
        for s in sources:
            dist[s] = 0
        d = 0
        while frontier:
            d += 1
            nxt = []
            for c in frontier:
                for t in self.reach(cells, c):
                    if dist[t] < 0:
                        dist[t] = d
                        nxt.append(t)
            frontier = nxt
        return dist


def pieces(cells, color):
    return [i for i, v in enumerate(cells) if v == color]


def evaluate(board, cells, me, op):
    mine, theirs = pieces(cells, me), pieces(cells, op)
    dm = board.queen_distance(cells, mine)
    dt = board.queen_distance(cells, theirs)
    territory = 0
    for i, v in enumerate(cells):
        if v != EMPTY:
            continue
        a, b = dm[i], dt[i]
        if a >= 0 and (b < 0 or a < b):
            territory += 1
        elif b >= 0 and (a < 0 or b < a):
            territory -= 1
    mobility = sum(len(board.reach(cells, p)) for p in mine) - sum(len(board.reach(cells, p)) for p in theirs)
    return territory + MOBILITY_WEIGHT * mobility


def gen_actions(board, cells, color):
    """(src, dst, arrow) by piece index, move ray order, arrow ray order."""
    out = []
    for src in pieces(cells, color):
        for dst in board.reach(cells, src):
            cells[src], cells[dst] = EMPTY, color
            for arrow in board.reach(cells, dst):
                out.append((src, dst, arrow))
            cells[src], cells[dst] = color, EMPTY
    return out


def best_action(board, cells, me, op, top_moves):
    """Two stages: rank every queen move without arrow, keep the `top_moves`
    best (stable), then score every arrow of those moves."""
    ranked = []
    for src in pieces(cells, me):
        for dst in board.reach(cells, src):
            cells[src], cells[dst] = EMPTY, me
            ranked.append((evaluate(board, cells, me, op), src, dst))
            cells[src], cells[dst] = me, EMPTY
    ranked.sort(key=lambda t: -t[0])
    best, best_v = None, None
    for _, src, dst in ranked[:top_moves]:
        cells[src], cells[dst] = EMPTY, me
        for arrow in board.reach(cells, dst):
            cells[arrow] = WALL
            v = evaluate(board, cells, me, op)
            cells[arrow] = EMPTY
            if best_v is None or v > best_v:
                best, best_v = (src, dst, arrow), v
        cells[src], cells[dst] = me, EMPTY
    return best


def square_name(n, i):
    return "%s%d" % (chr(97 + i % n), n - i // n)


def main():
    first = sys.stdin.readline()
    if not first:
        return
    n = int(first)
    board = Board(n)
    top_moves = int(os.environ.get("CG_NODES", str(TOP_MOVES)))
    while True:
        color_line = sys.stdin.readline()
        if not color_line:
            return
        me = color_line.strip()[:1] or "w"
        op = "b" if me == "w" else "w"
        cells = []
        for _ in range(n):
            row = sys.stdin.readline().rstrip("\r\n")
            cells.extend((row + WALL * n)[:n])
        if not sys.stdin.readline() or not sys.stdin.readline():
            return
        a = best_action(board, cells, me, op, top_moves)
        if a is None:
            print("random", flush=True)
            continue
        print("".join(square_name(n, i) for i in a), flush=True)


if __name__ == "__main__":
    main()
