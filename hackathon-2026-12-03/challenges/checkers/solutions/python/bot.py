#!/usr/bin/env python3
"""Checkers (English draughts) — Python reference (template board-2p), v1.

Board index = y * 8 + x, y = 0 is the first input line (row 8), x = 0 is
column A. Black ('b') starts at the bottom and moves up (y - 1); red ('r')
moves down (y + 1). Kings ('B', 'R') move one step in the 4 diagonals.
Captures are mandatory and multi-jumps must be completed; a man reaching
the crown row stops jumping (English rules). At the root we only play moves
from the referee's list; our generator is used deeper in the search.
"""
import os
import sys

EMPTY = 0
BM, BK, RM, RK = 1, 2, 3, 4   # black man/king, red man/king
SIDE_B, SIDE_R = 0, 1
CHAR = {".": EMPTY, "b": BM, "B": BK, "r": RM, "R": RK}
MAN_V, KING_V = 100, 160
DIAG = ((-1, -1), (1, -1), (-1, 1), (1, 1))   # (dx, dy)


def side_of(v):
    return SIDE_B if v in (BM, BK) else SIDE_R


def is_king(v):
    return v == BK or v == RK


class State:
    __slots__ = ("cell", "side")

    def __init__(self, cell, side):
        self.cell = cell
        self.side = side


def dirs_for(v):
    if is_king(v):
        return DIAG
    return DIAG[:2] if side_of(v) == SIDE_B else DIAG[2:]


def crown_row(v):
    return 0 if side_of(v) == SIDE_B else 7


def jumps_from(c, sq, v, path, out):
    """DFS of jump sequences; c is mutated and restored."""
    x0, y0 = sq % 8, sq // 8
    found = False
    for dx, dy in dirs_for(v):
        mx, my = x0 + dx, y0 + dy
        tx, ty = x0 + 2 * dx, y0 + 2 * dy
        if not (0 <= tx < 8 and 0 <= ty < 8):
            continue
        mid, to = my * 8 + mx, ty * 8 + tx
        mv = c[mid]
        if mv == EMPTY or side_of(mv) == side_of(v) or c[to] != EMPTY:
            continue
        found = True
        c[mid] = EMPTY
        if not is_king(v) and ty == crown_row(v):
            out.append(path + [to])
        else:
            c[to] = v
            jumps_from(c, to, v, path + [to], out)
            c[to] = EMPTY
        c[mid] = mv
    if not found and len(path) > 1:
        out.append(path)


def gen_moves(s):
    """List of square paths; jumps only if any jump exists."""
    c = s.cell
    jumps = []
    for sq in range(64):
        v = c[sq]
        if v == EMPTY or side_of(v) != s.side:
            continue
        c[sq] = EMPTY
        jumps_from(c, sq, v, [sq], jumps)
        c[sq] = v
    if jumps:
        return jumps
    out = []
    for sq in range(64):
        v = c[sq]
        if v == EMPTY or side_of(v) != s.side:
            continue
        x0, y0 = sq % 8, sq // 8
        for dx, dy in dirs_for(v):
            x, y = x0 + dx, y0 + dy
            if 0 <= x < 8 and 0 <= y < 8 and c[y * 8 + x] == EMPTY:
                out.append([sq, y * 8 + x])
    return out


def apply_move(s, path):
    c = s.cell[:]
    v = c[path[0]]
    c[path[0]] = EMPTY
    for a, b in zip(path, path[1:]):
        ax, ay, bx, by = a % 8, a // 8, b % 8, b // 8
        if abs(bx - ax) == 2:
            c[((ay + by) // 2) * 8 + (ax + bx) // 2] = EMPTY
    end = path[-1]
    if not is_king(v) and end // 8 == crown_row(v):
        v = BK if v == BM else RK
    c[end] = v
    return State(c, 1 - s.side)


def evaluate(s):
    sc = 0
    for sq in range(64):
        v = s.cell[sq]
        if v == EMPTY:
            continue
        y = sq // 8
        if is_king(v):
            val = KING_V
        else:
            adv = (7 - y) if side_of(v) == SIDE_B else y
            val = MAN_V + 2 * adv
            if adv == 0:
                val += 6   # back-rank guard
        sc += val if side_of(v) == SIDE_B else -val
    return sc if s.side == SIDE_B else -sc


def order_key(_s, path):
    return len(path)


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "25000"))


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
            return -(WIN - ply)
        if depth == 0:
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
            self.depth = depth
            mv.insert(0, mv.pop(cur_idx))
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
        return best


# ------------------------------------------------------------- GAME: I/O
def parse_path(txt):
    """'A3C5A7' -> [squares]; None if malformed."""
    t = txt.strip().upper()
    if len(t) < 4 or len(t) % 2:
        return None
    out = []
    for i in range(0, len(t), 2):
        col, row = t[i], t[i + 1]
        if not ("A" <= col <= "H" and "1" <= row <= "8"):
            return None
        out.append((8 - int(row)) * 8 + (ord(col) - 65))
    return out


def name_of(path):
    return "".join("%s%d" % (chr(65 + sq % 8), 8 - sq // 8) for sq in path)


def main():
    line = sys.stdin.readline()
    if not line:
        return
    side = SIDE_B if line.strip().lower().startswith("b") else SIDE_R
    while True:
        rows = []
        for _ in range(8):
            r = sys.stdin.readline()
            if not r:
                return
            rows.append((r.rstrip("\r\n") + "........")[:8])
        n_line = sys.stdin.readline()
        if not n_line:
            return
        n = int(n_line)
        given = []
        for _ in range(n):
            t = sys.stdin.readline().split()
            if t:
                p = parse_path(t[0])
                if p:
                    given.append(p)
        cell = [CHAR.get(ch, EMPTY) for row in rows for ch in row]
        s = State(cell, side)
        if not given:
            print("A1B2", flush=True)
            continue
        best = Search().run(s, given)
        print(name_of(best), flush=True)


if __name__ == "__main__":
    main()
