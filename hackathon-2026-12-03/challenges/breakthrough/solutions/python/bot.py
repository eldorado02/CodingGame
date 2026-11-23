#!/usr/bin/env python3
"""Breakthrough 8x8 — Python reference (template board-2p), v3 bitboards.

Bit index = rank * 8 + file (rank 0 = "1", file 0 = "a").
White (1) moves to rank 7, Black (2) moves to rank 0.
v3: bitboards + exact runner rule at every node (see plan.md):
  * side to move owns a piece one step from goal -> wins next move;
  * opponent owns >=2 such runners, or 1 we cannot capture -> lost in 2 plies.
The bot tracks the board from the opponent's move and its own moves, and only
prints a move present in the legal-move list given by the referee.
"""
import os
import sys

WHITE, BLACK = 1, 2
M64 = (1 << 64) - 1
FILE_A = 0x0101010101010101
FILE_H = 0x8080808080808080
RANK = [0xFF << (8 * r) for r in range(8)]
ADV = (0, 4, 10, 20, 36, 60, 110, 0)   # by progress (0 = own home row)
PIECE = 1000
DEFENDED = 12
HOME_GUARD = 18


def popcount(x):
    return bin(x).count("1")


class State:
    __slots__ = ("w", "b", "to_move")

    def __init__(self, w, b, to_move):
        self.w = w
        self.b = b
        self.to_move = to_move

    def own(self, p):
        return self.w if p == WHITE else self.b


def initial_state():
    return State(0xFFFF, 0xFFFF << 48, WHITE)


def attacks(p, x):
    """Squares diagonally in front of the pieces x of colour p."""
    if p == WHITE:
        return (((x << 7) & ~FILE_H) | ((x << 9) & ~FILE_A)) & M64
    return ((x >> 9) & ~FILE_H) | ((x >> 7) & ~FILE_A)


def runners(p, x):
    """Pieces of colour p one step from their goal row."""
    return x & (RANK[6] if p == WHITE else RANK[1])


def winner(s):
    if s.w & RANK[7]:
        return WHITE
    if s.b & RANK[0]:
        return BLACK
    if not s.w:
        return BLACK
    if not s.b:
        return WHITE
    return 0


def gen_moves(s):
    """(from, to) by ascending square, then target file-1, file, file+1."""
    p = s.to_move
    mine = s.own(p)
    d = 1 if p == WHITE else -1
    out = []
    x = mine
    while x:
        low = x & -x
        sq = low.bit_length() - 1
        x ^= low
        r, f = sq >> 3, sq & 7
        nr = r + d
        if nr < 0 or nr > 7:
            continue
        for df in (-1, 0, 1):
            nf = f + df
            if nf < 0 or nf > 7:
                continue
            t = nr * 8 + nf
            tb = 1 << t
            if tb & mine:
                continue
            if df == 0 and tb & (s.w | s.b):
                continue
            out.append((sq, t))
    return out


def apply_move(s, m):
    fb, tb = 1 << m[0], 1 << m[1]
    if s.to_move == WHITE:
        return State((s.w ^ fb) | tb, s.b & ~tb, BLACK)
    return State(s.w & ~tb, (s.b ^ fb) | tb, WHITE)


def side_score(p, x):
    sc = PIECE * popcount(x)
    for r in range(8):
        c = popcount(x & RANK[r])
        if c:
            sc += c * ADV[r if p == WHITE else 7 - r]
    home = RANK[0] if p == WHITE else RANK[7]
    sc += HOME_GUARD * popcount(x & home)
    sc += DEFENDED * popcount(x & attacks(p, x))
    return sc


def evaluate(s):
    p = s.to_move
    o = 3 - p
    return side_score(p, s.own(p)) - side_score(o, s.own(o))


def order_key(s, m):
    k = 0
    goal = 7 if s.to_move == WHITE else 0
    if (m[1] >> 3) == goal:
        k += 4
    if (1 << m[1]) & (s.w | s.b):
        k += 2
    return k


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "60000"))


def runner_rule(s, ply):
    """Exact short-term outcome or 0. Called on non-terminal positions."""
    p = s.to_move
    o = 3 - p
    if runners(p, s.own(p)):
        return WIN - (ply + 1)
    r = runners(o, s.own(o))
    if r:
        if r & (r - 1) or not (r & attacks(p, s.own(p))):
            return -(WIN - (ply + 2))
    return 0


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
        q = runner_rule(s, ply)
        if q:
            return q
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
        self.depth = 0
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
            self.depth = depth
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
            s.to_move = 3 - me
            s = apply_move(s, move_of(opp))
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
