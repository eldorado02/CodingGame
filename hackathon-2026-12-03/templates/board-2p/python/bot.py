#!/usr/bin/env python3
"""TEMPLATE board-2p — Python reference for the differential fuzz.

Same engine as cpp/bot.cpp (negamax + alpha-beta + iterative deepening,
deterministic node budget). Replace the "GAME:" blocks only. Python is the
REFERENCE of the fuzz: C, C++ and Rust must print byte-identical output.
"""
import os
import sys
import time

# ------------------------------------------------------------- GAME: state
LINES = ((0, 1, 2), (3, 4, 5), (6, 7, 8), (0, 3, 6), (1, 4, 7), (2, 5, 8),
         (0, 4, 8), (2, 4, 6))
WEIGHT = (3, 2, 3, 2, 4, 2, 3, 2, 3)


class State:
    __slots__ = ("cell", "to_move")

    def __init__(self, cell=None, to_move=1):
        self.cell = cell if cell is not None else [0] * 9
        self.to_move = to_move


def winner(s):
    """GAME: 0 none, 1/2 player, 3 draw."""
    for a, b, c in LINES:
        v = s.cell[a]
        if v and v == s.cell[b] and v == s.cell[c]:
            return v
    return 0 if 0 in s.cell else 3


def gen_moves(s):
    """GAME: legal moves in a fixed deterministic order."""
    return [(i // 3, i % 3) for i in range(9) if not s.cell[i]]


def apply_move(s, m):
    """GAME: copy-make apply."""
    cell = s.cell[:]
    cell[m[0] * 3 + m[1]] = s.to_move
    return State(cell, 3 - s.to_move)


def evaluate(s):
    """GAME: static eval from the point of view of s.to_move."""
    sc = 0
    for i, v in enumerate(s.cell):
        if v == s.to_move:
            sc += WEIGHT[i]
        elif v:
            sc -= WEIGHT[i]
    return sc


def order_key(_s, m):
    """GAME: higher searched first; sorted() is stable like std::stable_sort."""
    return 1 if m == (1, 1) else 0


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "200000"))


class Aborted(Exception):
    pass


class Search:
    def __init__(self):
        self.nodes = 0

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        w = winner(s)
        if w == 3:
            return 0
        if w:
            return WIN - ply if w == s.to_move else -(WIN - ply)
        if depth == 0:
            return evaluate(s)
        mv = gen_moves(s)
        if not mv:
            return evaluate(s)
        mv.sort(key=lambda m: -order_key(s, m))
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
        mv = gen_moves(root)
        mv.sort(key=lambda m: -order_key(root, m))
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur, cur_idx = -INF, -INF, mv[0], 0
            try:
                for i, m in enumerate(mv):
                    v = -self.negamax(apply_move(root, m), depth - 1, -INF, -alpha, 1)
                    if v > best_v:
                        best_v, cur, cur_idx = v, m, i
                    if v > alpha:
                        alpha = v
            except Aborted:
                break
            best = cur
            mv.insert(0, mv.pop(cur_idx))
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
            if depth >= 9:
                break
        return best


# ------------------------------------------------------------- GAME: I/O
def main():
    s = State()
    data = sys.stdin
    while True:
        line = data.readline()
        if not line:
            break
        parts = line.split()
        if len(parts) < 2:
            break
        r, c = int(parts[0]), int(parts[1])
        if 0 <= r < 3 and 0 <= c < 3:
            s.cell[r * 3 + c] = 2
        k = int(data.readline())
        for _ in range(k):
            data.readline()
        s.to_move = 1
        m = Search().run(s)
        s.cell[m[0] * 3 + m[1]] = 1
        print(m[0], m[1], flush=True)
    _ = time  # time guard unused in the reference (no time limit in fuzz)


if __name__ == "__main__":
    main()
