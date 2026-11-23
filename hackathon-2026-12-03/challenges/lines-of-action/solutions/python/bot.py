#!/usr/bin/env python3
"""Lines of Action 8x8 — Python reference (template board-2p), v1.

A checker moves exactly as many squares as there are checkers on its line,
may jump friends but not enemies, and captures by landing on an enemy.
Connecting all own checkers (8-neighbourhood) wins; if a move connects both
sides, the mover wins. 150 moves -> draw. Black starts.
Index = y * 8 + x, y = 0 is the first input line (rank 8).
Eval per side: -(sum of Chebyshev distances to its centre of mass)
- GROUP_WEIGHT * (groups - 1); eval = ours - theirs.
"""
import os
import sys

N = 8
TURN_NODES = 20000
GROUP_WEIGHT = 10
DIRS = ((0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (1, 1), (-1, 1), (1, -1))
EMPTY = "."


class State:
    __slots__ = ("cell", "me", "op")

    def __init__(self, cell, me, op):
        self.cell = cell      # list of 64 chars
        self.me = me          # colour to move
        self.op = op


def line_count(c, x, y, dx, dy):
    """Checkers on the whole line through (x, y) along (dx, dy)."""
    n = 0
    for sgn in (1, -1):
        cx, cy = x + sgn * dx, y + sgn * dy
        while 0 <= cx < N and 0 <= cy < N:
            if c[cy * N + cx] != EMPTY:
                n += 1
            cx += sgn * dx
            cy += sgn * dy
    return n + 1   # the moving checker itself


def gen_moves(s):
    c, me, op = s.cell, s.me, s.op
    out = []
    for i in range(N * N):
        if c[i] != me:
            continue
        x, y = i % N, i // N
        for dx, dy in DIRS:
            k = line_count(c, x, y, dx, dy)
            tx, ty = x + k * dx, y + k * dy
            if not (0 <= tx < N and 0 <= ty < N):
                continue
            t = ty * N + tx
            if c[t] == me:
                continue
            blocked = any(c[(y + j * dy) * N + x + j * dx] == op for j in range(1, k))
            if not blocked:
                out.append((i, t))
    return out


def apply_move(s, m):
    c = s.cell[:]
    c[m[1]] = c[m[0]]
    c[m[0]] = EMPTY
    return State(c, s.op, s.me)


def groups(c, color):
    seen = [False] * (N * N)
    count = 0
    for i in range(N * N):
        if c[i] != color or seen[i]:
            continue
        count += 1
        stack = [i]
        seen[i] = True
        while stack:
            j = stack.pop()
            x, y = j % N, j // N
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < N and 0 <= ny < N:
                        k = ny * N + nx
                        if not seen[k] and c[k] == color:
                            seen[k] = True
                            stack.append(k)
    return count


def winner_after_move(s):
    """s is the position after a move by s.op. Returns the winning colour or None."""
    mover, other = s.op, s.me
    if groups(s.cell, mover) <= 1:
        return mover
    if groups(s.cell, other) <= 1:
        return other
    return None


def side_score(c, color):
    cells = [i for i in range(N * N) if c[i] == color]
    if not cells:
        return 0
    cx = sum(i % N for i in cells) // len(cells)
    cy = sum(i // N for i in cells) // len(cells)
    spread = sum(max(abs(i % N - cx), abs(i // N - cy)) for i in cells)
    return -spread - GROUP_WEIGHT * (groups(c, color) - 1)


def evaluate(s):
    return side_score(s.cell, s.me) - side_score(s.cell, s.op)


def order_key(s, m):
    """Captures first."""
    return 1 if s.cell[m[1]] == s.op else 0


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
        w = winner_after_move(s)
        if w is not None:
            return WIN - ply if w == s.me else -(WIN - ply)
        if depth == 0:
            return evaluate(s)
        mv = gen_moves(s)
        if not mv:
            return -self.negamax(State(s.cell, s.op, s.me), depth - 1, -beta, -alpha, ply + 1)
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
        mv = sorted(mv, key=lambda m: -order_key(root, m))
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
def square(t):
    if len(t) != 2 or not ("a" <= t[0] <= "h" and "1" <= t[1] <= "8"):
        return None
    return (N - int(t[1])) * N + ord(t[0]) - 97


def name_of(i):
    return "%s%d" % (chr(97 + i % N), N - i // N)


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = line.strip()[:1] or "b"
    op = "w" if me == "b" else "b"
    while True:
        cells = []
        for _ in range(N):
            row = sys.stdin.readline()
            if not row:
                return
            cells.extend((row.rstrip("\r\n") + EMPTY * N)[:N])
        if not sys.stdin.readline():
            return
        n_line = sys.stdin.readline()
        if not n_line:
            return
        legal = []
        for _ in range(int(n_line)):
            t = sys.stdin.readline().split()
            if t and len(t[0]) == 4:
                a, b = square(t[0][:2]), square(t[0][2:])
                if a is not None and b is not None:
                    legal.append((a, b))
        if not legal:
            print("pass", flush=True)
            continue
        src, dst = Search().run(State(cells, me, op), legal)
        print(name_of(src) + name_of(dst), flush=True)


if __name__ == "__main__":
    main()
