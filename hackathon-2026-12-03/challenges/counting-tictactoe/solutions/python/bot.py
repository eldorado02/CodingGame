#!/usr/bin/env python3
"""Counting TicTacToe 10x10 — Python reference (template board-2p), v1.

Score = number of windows of 3 consecutive same-colour cells (rows, columns,
both diagonals; sliding, so a line of 4 scores 2) once the board is full
(referee: github.com/RezaSi/counting-tic-tac-toe TicTacToeGrid.countScore).
A game is two matches with colours swapped; the valid-action list is the set
of empty cells. Occupied = not in the list; ours = our own record, reset when
one of our cells becomes empty again (new match).
Eval (side to move): window patterns, see WEIGHTS.
"""
import os
import sys

N = 10
CELLS = N * N
TURN_NODES = 2500
# inner nodes only search the BEAM best moves by immediate gain (root: all)
BEAM = 10
# value of a window by number of own stones when the other side has none
WEIGHTS = (0, 1, 10, 100)


def build_windows():
    out = []
    for r in range(N):
        for c in range(N):
            for dr, dc in ((1, 0), (0, 1), (1, 1), (1, -1)):
                r2, c2 = r + 2 * dr, c + 2 * dc
                if 0 <= r2 < N and 0 <= c2 < N:
                    out.append((r * N + c, (r + dr) * N + c + dc, r2 * N + c2))
    return out


WINDOWS = build_windows()
WINDOWS_OF = [[w for w in WINDOWS if i in w] for i in range(CELLS)]


def window_value(c, w):
    """Value of window w for player 1 (negative = good for player 2)."""
    ones = (c[w[0]] == 1) + (c[w[1]] == 1) + (c[w[2]] == 1)
    twos = (c[w[0]] == 2) + (c[w[1]] == 2) + (c[w[2]] == 2)
    if twos == 0:
        return WEIGHTS[ones]
    if ones == 0:
        return -WEIGHTS[twos]
    return 0


class State:
    __slots__ = ("cell", "me", "score")

    def __init__(self, cell, me, score=None):
        self.cell = cell     # 0 empty, 1 side A, 2 side B
        self.me = me         # side to move (1 or 2)
        # pattern score for player 1, updated incrementally
        self.score = sum(window_value(cell, w) for w in WINDOWS) if score is None else score


def gen_moves(s):
    return [i for i in range(CELLS) if s.cell[i] == 0]


def apply_move(s, i):
    """Only the windows through cell i change: update the score by delta."""
    cell = s.cell[:]
    before = sum(window_value(cell, w) for w in WINDOWS_OF[i])
    cell[i] = s.me
    after = sum(window_value(cell, w) for w in WINDOWS_OF[i])
    return State(cell, 3 - s.me, s.score + after - before)


def evaluate(s):
    return s.score if s.me == 1 else -s.score


def ordered_children(s, limit):
    """Children sorted by the mover's immediate gain (stable), at most `limit`."""
    children = []
    for m in gen_moves(s):
        child = apply_move(s, m)
        gain = -evaluate(child)   # child is seen from the opponent's side
        children.append((gain, m, child))
    children.sort(key=lambda t: -t[0])
    return [(m, child) for _, m, child in children[:limit]]


def final_triples(s):
    me, op = s.me, 3 - s.me
    c = s.cell
    diff = 0
    for a, b, d in WINDOWS:
        if c[a] == c[b] == c[d] != 0:
            diff += 1 if c[a] == me else -1
    return diff


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


class Search:
    def __init__(self):
        self.nodes = 0
        self.depth = 0

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        mv = gen_moves(s)
        if not mv:
            diff = final_triples(s)
            return (WIN - ply) if diff > 0 else (-(WIN - ply) if diff < 0 else 0)
        if depth == 0:
            return evaluate(s)
        best = -INF
        for _, child in ordered_children(s, BEAM):
            v = -self.negamax(child, depth - 1, -beta, -alpha, ply + 1)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, root):
        mv = [m for m, _ in ordered_children(root, CELLS)]
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
MINE, THEIRS = 1, 2


def main():
    my_cells = set()
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        k_line = sys.stdin.readline()
        if not k_line:
            return
        empty = set()
        for _ in range(int(k_line)):
            parts = sys.stdin.readline().split()
            if len(parts) >= 2:
                r, c = int(parts[0]), int(parts[1])
                if 0 <= r < N and 0 <= c < N:
                    empty.add(r * N + c)
        if my_cells & empty:
            my_cells = set()   # a new match started: the board was cleared
        cell = [0] * CELLS
        for i in range(CELLS):
            if i not in empty:
                cell[i] = MINE if i in my_cells else THEIRS
        s = State(cell, MINE)
        if not empty:
            print("0 0", flush=True)
            continue
        m = Search().run(s)
        my_cells.add(m)
        print(m // N, m % N, flush=True)


if __name__ == "__main__":
    main()
