#!/usr/bin/env python3
"""Dots and Boxes — Python reference (template board-2p), v1 alpha-beta.

Board n x n boxes; box (x, y) with y = 0 the bottom row ("A1" = x 0, y 0).
Edges: horizontal h(x, line) = line * n + x (line 0..n, bottom to top),
vertical v(line, y) = H + y * (n + 1) + line (line 0..n, left to right).
Drawing an edge that closes one or two boxes scores them and the same
player moves again (no sign flip in the negamax).
Eval (side to move): box difference + CAPTURABLE_WEIGHT * boxes we can take
now (boxes with a single free side).
"""
import os
import sys

TURN_NODES = 20000
SIDE_LETTERS = "LTRB"
CAPTURABLE_WEIGHT = 1     # a box with one free side is ours to take


class Board:
    def __init__(self, n):
        self.n = n
        self.h_count = n * (n + 1)
        self.edges = 2 * n * (n + 1)
        # box -> its 4 edges in L, T, R, B order
        self.box_edges = []
        for y in range(n):
            for x in range(n):
                left = self.h_count + y * (n + 1) + x
                top = (y + 1) * n + x
                right = left + 1
                bottom = y * n + x
                self.box_edges.append((left, top, right, bottom))
        self.edge_boxes = [[] for _ in range(self.edges)]
        for b, es in enumerate(self.box_edges):
            for e in es:
                self.edge_boxes[e].append(b)


class State:
    __slots__ = ("drawn", "diff")

    def __init__(self, drawn, diff):
        self.drawn = drawn    # list of bools per edge
        self.diff = diff      # boxes of side to move - boxes of the other side


def free_sides(board, drawn, b):
    return sum(1 for e in board.box_edges[b] if not drawn[e])


def gen_moves(board, s):
    return [e for e in range(board.edges) if not s.drawn[e]]


def apply_move(board, s, e):
    """Returns (child, same_player)."""
    drawn = s.drawn[:]
    drawn[e] = True
    closed = sum(1 for b in board.edge_boxes[e] if free_sides(board, drawn, b) == 0)
    if closed:
        return State(drawn, s.diff + closed), True
    return State(drawn, -s.diff), False


def evaluate(board, s):
    capturable = sum(1 for b in range(board.n * board.n) if free_sides(board, s.drawn, b) == 1)
    return s.diff + CAPTURABLE_WEIGHT * capturable


def order_key(board, s, e):
    """2 = closes a box, 1 = safe (gives no third side), 0 = gift."""
    worst = 2
    closes = False
    for b in board.edge_boxes[e]:
        f = free_sides(board, s.drawn, b)
        if f == 1:
            closes = True
        elif f == 2:
            worst = 0
    if closes:
        return 2
    return 1 if worst else 0


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


class Search:
    def __init__(self, board):
        self.board = board
        self.nodes = 0
        self.depth = 0

    def value(self, s, depth, alpha, beta):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        mv = gen_moves(self.board, s)
        if not mv:
            return WIN + s.diff if s.diff > 0 else (-WIN + s.diff if s.diff < 0 else 0)
        if depth == 0:
            return evaluate(self.board, s)
        mv.sort(key=lambda e: -order_key(self.board, s, e))
        best = -INF
        for e in mv:
            child, again = apply_move(self.board, s, e)
            if again:
                v = self.value(child, depth - 1, alpha, beta)
            else:
                v = -self.value(child, depth - 1, -beta, -alpha)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, root):
        mv = sorted(gen_moves(self.board, root), key=lambda e: -order_key(self.board, root, e))
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur_idx = -INF, -INF, 0
            try:
                for i, e in enumerate(mv):
                    child, again = apply_move(self.board, root, e)
                    v = self.value(child, depth - 1, alpha, INF) if again \
                        else -self.value(child, depth - 1, -INF, -alpha)
                    if v > best_v:
                        best_v, cur_idx = v, i
                    if v > alpha:
                        alpha = v
            except Aborted:
                break
            best = mv[cur_idx]
            self.depth = depth
            mv.insert(0, mv.pop(cur_idx))   # PV first next iteration
            if depth >= len(mv) + 1:
                break
        return best


# ------------------------------------------------------------- I/O
def main():
    first = sys.stdin.readline()
    if not first:
        return
    n = int(first)
    board = Board(n)
    sys.stdin.readline()   # our id: the state is given in our point of view
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        mine, theirs = (int(v) for v in line.split()[:2])
        k = int(sys.stdin.readline())
        drawn = [True] * board.edges
        owner_box = {}   # edge -> first listed box name/side that can draw it
        for _ in range(k):
            parts = sys.stdin.readline().split()
            if len(parts) < 2 or len(parts[0]) < 2:
                continue
            name, sides = parts[0], parts[1]
            x, y = ord(name[0]) - 65, int(name[1:]) - 1
            if not (0 <= x < n and 0 <= y < n):
                continue
            edges = board.box_edges[y * n + x]
            for ch in sides:
                if ch in SIDE_LETTERS:
                    e = edges[SIDE_LETTERS.index(ch)]
                    drawn[e] = False
                    owner_box.setdefault(e, "%s %s" % (name, ch))
        s = State(drawn, mine - theirs)
        if not owner_box:
            print("A1 L", flush=True)
            continue
        e = Search(board).run(s)
        print(owner_box[e], flush=True)


if __name__ == "__main__":
    main()
