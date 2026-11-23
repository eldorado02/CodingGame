#!/usr/bin/env python3
"""Breakthrough 8x8 — Python reference (template board-2p).

Board: index = rank * 8 + file, rank 0 = "1", file 0 = "a".
Cell: 0 empty, 1 White (moves to rank 7), 2 Black (moves to rank 0).
The bot tracks the board from the opponent's move and its own moves, and only
prints a move present in the legal-move list given by the referee.
"""
import os
import sys

# ------------------------------------------------------------- GAME: state
WHITE, BLACK = 1, 2
DIR = (0, 1, -1)          # rank step per colour
GOAL = (0, 7, 0)          # rank to reach per colour
# advancement bonus by progress (0 = own home row, 7 = goal row)
ADV = (0, 4, 10, 20, 36, 60, 110, 0)
PIECE = 1000
DEFENDED = 12
HOME_GUARD = 18


class State:
    __slots__ = ("cell", "to_move")

    def __init__(self, cell, to_move):
        self.cell = cell
        self.to_move = to_move


def initial_state():
    cell = [0] * 64
    for i in range(16):
        cell[i] = WHITE
        cell[63 - i] = BLACK
    return State(cell, WHITE)


def winner(s):
    """0 none, else winning colour (goal row reached or no piece left)."""
    w = b = 0
    for i in range(8):
        if s.cell[56 + i] == WHITE:
            return WHITE
        if s.cell[i] == BLACK:
            return BLACK
    for v in s.cell:
        if v == WHITE:
            w += 1
        elif v == BLACK:
            b += 1
    if w == 0:
        return BLACK
    if b == 0:
        return WHITE
    return 0


def gen_moves(s):
    """Moves (from, to) in square order, then targets file-1, file, file+1."""
    p = s.to_move
    d = DIR[p]
    out = []
    for sq in range(64):
        if s.cell[sq] != p:
            continue
        r, f = sq >> 3, sq & 7
        nr = r + d
        if nr < 0 or nr > 7:
            continue
        for df in (-1, 0, 1):
            nf = f + df
            if nf < 0 or nf > 7:
                continue
            t = nr * 8 + nf
            v = s.cell[t]
            if v == 0 or (df != 0 and v != p):
                out.append((sq, t))
    return out


def apply_move(s, m):
    cell = s.cell[:]
    cell[m[1]] = cell[m[0]]
    cell[m[0]] = 0
    return State(cell, 3 - s.to_move)


def progress(p, sq):
    r = sq >> 3
    return r if p == WHITE else 7 - r


def side_score(s, p):
    d = DIR[p]
    sc = 0
    for sq in range(64):
        if s.cell[sq] != p:
            continue
        pr = progress(p, sq)
        sc += PIECE + ADV[pr]
        if pr == 0:
            sc += HOME_GUARD
        r, f = sq >> 3, sq & 7
        br = r - d
        if 0 <= br <= 7:
            if (f > 0 and s.cell[br * 8 + f - 1] == p) or (f < 7 and s.cell[br * 8 + f + 1] == p):
                sc += DEFENDED
    return sc


def evaluate(s):
    p = s.to_move
    return side_score(s, p) - side_score(s, 3 - p)


def order_key(s, m):
    k = 0
    if (m[1] >> 3) == GOAL[s.to_move]:
        k += 4
    if s.cell[m[1]] != 0:
        k += 2
    return k


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "120000"))


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
        if w:
            return WIN - ply if w == s.to_move else -(WIN - ply)
        if depth == 0:
            return evaluate(s)
        mv = gen_moves(s)
        if not mv:
            return -(WIN - ply)
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

    def run(self, root, mv):
        mv = list(mv)
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
        return best


# ------------------------------------------------------------- GAME: I/O
def sq_of(txt):
    return (ord(txt[1]) - ord("1")) * 8 + (ord(txt[0]) - ord("a"))


def move_of(txt):
    return (sq_of(txt[0:2]), sq_of(txt[2:4]))


def name_of(m):
    a, b = m
    return "%s%d%s%d" % (chr(97 + (a & 7)), (a >> 3) + 1, chr(97 + (b & 7)), (b >> 3) + 1)


def valid_move_text(t):
    return (len(t) >= 4 and "a" <= t[0] <= "h" and "1" <= t[1] <= "8"
            and "a" <= t[2] <= "h" and "1" <= t[3] <= "8")


def main():
    s = initial_state()
    me = 0
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        opp = line.split()[0] if line.split() else "None"
        n_line = sys.stdin.readline()
        if not n_line:
            break
        n = int(n_line)
        legal = []
        for _ in range(n):
            t = sys.stdin.readline().split()
            if t and valid_move_text(t[0]):
                legal.append(t[0][:4])
        if me == 0:
            me = WHITE if opp == "None" else BLACK
        if valid_move_text(opp):
            s = apply_move(State(s.cell, 3 - me), move_of(opp))
        s.to_move = me
        legal_set = set(legal)
        mine = [m for m in gen_moves(s) if name_of(m) in legal_set]
        if not mine:
            out = legal[0] if legal else "a1a2"
            if legal:
                s = apply_move(s, move_of(out))
        else:
            m = Search().run(s, mine)
            out = name_of(m)
            s = apply_move(s, m)
        print(out, flush=True)


if __name__ == "__main__":
    main()
