#!/usr/bin/env python3
"""Clobber — Python reference (template board-2p), v1 alpha-beta.

A move takes one of our stones onto an orthogonally adjacent enemy stone
(the enemy stone is removed). No move on your turn = loss (no draws).
Bit index = y * 8 + x, y = 0 is the first input line (top row, rank n),
x = 0 is column a. Boards up to 8x8.
Eval (side to move): our move count - their move count (mobility, the
classic Clobber evaluation; cf. TidyMaze/clobber minimax variant).
"""
import os
import sys

STRIDE = 8
TURN_NODES = 60000
# (dx, dy) in a fixed order: the move order of every port depends on it
DIRS = ((0, -1), (0, 1), (-1, 0), (1, 0))


class Board:
    def __init__(self, n):
        self.n = n
        self.cells = [y * STRIDE + x for y in range(n) for x in range(n)]
        self.mask = sum(1 << i for i in self.cells)
        self.nb = {}
        for i in self.cells:
            x, y = i % STRIDE, i // STRIDE
            self.nb[i] = tuple((y + dy) * STRIDE + x + dx for dx, dy in DIRS
                               if 0 <= x + dx < n and 0 <= y + dy < n)


class State:
    __slots__ = ("mine", "theirs")

    def __init__(self, mine, theirs):
        self.mine = mine      # stones of the side to move
        self.theirs = theirs


def gen_moves(board, s):
    out = []
    for i in board.cells:
        if not (s.mine >> i) & 1:
            continue
        for t in board.nb[i]:
            if (s.theirs >> t) & 1:
                out.append((i, t))
    return out


COL0 = 0x0101010101010101
COL7 = 0x8080808080808080


def popcount(x):
    return bin(x).count("1")


def count_moves(board, own, other):
    """Moves of `own` onto `other`: one shift per direction, then popcount.

    Column masks remove the wrap-around between rows."""
    m = board.mask
    up = own >> STRIDE
    down = (own << STRIDE) & m
    left = (own >> 1) & ~COL7 & m
    right = (own << 1) & ~COL0 & m
    return popcount(up & other) + popcount(down & other) + popcount(left & other) + popcount(right & other)


def apply_move(s, m):
    src, dst = m
    mine = (s.mine & ~(1 << src)) | (1 << dst)
    return State(s.theirs & ~(1 << dst), mine)   # side to move switches


def evaluate(board, s):
    return count_moves(board, s.mine, s.theirs) - count_moves(board, s.theirs, s.mine)


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

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        mv = gen_moves(self.board, s)
        if not mv:
            return -(WIN - ply)
        if depth == 0:
            return evaluate(self.board, s)
        best = -INF
        for m in mv:
            v = -self.negamax(apply_move(s, m), depth - 1, -beta, -alpha, ply + 1)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, root):
        mv = gen_moves(self.board, root)
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur_idx = -INF, -INF, 0
            try:
                for i, m in enumerate(mv):
                    v = -self.negamax(apply_move(root, m), depth - 1, -INF, -alpha, 1)
                    if v > best_v:
                        best_v, cur_idx = v, i
                    if v > alpha:
                        alpha = v
            except Aborted:
                break
            best = mv[cur_idx]
            self.depth = depth
            mv.insert(0, mv.pop(cur_idx))   # PV first next iteration
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
        return best


# ------------------------------------------------------------- I/O
def square_name(board, i):
    return "%s%d" % (chr(97 + i % STRIDE), board.n - i // STRIDE)


def main():
    first = sys.stdin.readline()
    if not first:
        return
    n = min(int(first), STRIDE)
    me = (sys.stdin.readline().strip() or "w")[0]
    board = Board(n)
    while True:
        mine = theirs = 0
        for y in range(n):
            row = sys.stdin.readline()
            if not row:
                return
            for x, ch in enumerate(row.rstrip("\r\n")[:n]):
                bit = 1 << (y * STRIDE + x)
                if ch == me:
                    mine |= bit
                elif ch in "wb":
                    theirs |= bit
        if not sys.stdin.readline() or not sys.stdin.readline():
            return
        s = State(mine, theirs)
        if not gen_moves(board, s):
            print("random", flush=True)
            continue
        src, dst = Search(board).run(s)
        print(square_name(board, src) + square_name(board, dst), flush=True)


if __name__ == "__main__":
    main()
