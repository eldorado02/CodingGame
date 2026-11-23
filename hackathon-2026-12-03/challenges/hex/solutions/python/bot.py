#!/usr/bin/env python3
"""Hex 11x11 (CodinGame) — Python reference of the differential fuzz.

Cell index = row * 11 + col ("d4" = col 'd', row 4 -> index 3*11+3).
Neighbours (checked with a test game, see research.md):
(-1,0) (-1,1) (0,-1) (0,1) (1,-1) (1,0).
Red (1) connects row 0 to row 10, blue (2) connects col 0 to col 10.

Algorithm (v1): immediate win / forced block, then MCTS + RAVE
(no exploration term, beta = an / (an + n + RAVE_B*an*n)), random-fill
playouts, winner of the full board by one BFS. Budget = iterations
(env CG_NODES), so C / C++ / Rust give byte-identical output.
"""
import os
import sys

N = 11
NC = N * N
RED, BLUE = 1, 2
DIRS = ((-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0))
NB = []
for _c in range(NC):
    _r, _q = divmod(_c, N)
    _l = []
    for dr, dc in DIRS:
        a, b = _r + dr, _q + dc
        if 0 <= a < N and 0 <= b < N:
            _l.append(a * N + b)
    NB.append(tuple(_l))

RAVE_B = 0.04
MAX_NODES = 2000000
EXPAND_N = 3   # expand a leaf after this many visits
MASK = (1 << 64) - 1
OPENING = 1 * N + 2      # red first move "c2"


def connected(board, p):
    """True if colour p links its two sides on board."""
    if p == RED:
        stack = [c for c in range(N) if board[c] == RED]
    else:
        stack = [r * N for r in range(N) if board[r * N] == BLUE]
    seen = bytearray(NC)
    for c in stack:
        seen[c] = 1
    while stack:
        c = stack.pop()
        if (p == RED and c // N == N - 1) or (p == BLUE and c % N == N - 1):
            return True
        for d in NB[c]:
            if not seen[d] and board[d] == p:
                seen[d] = 1
                stack.append(d)
    return False


class Rng:
    def __init__(self, seed):
        self.s = seed & MASK

    def next(self):
        x = self.s
        x ^= (x << 13) & MASK
        x ^= x >> 7
        x ^= (x << 17) & MASK
        self.s = x
        return x


class Mcts:
    def __init__(self, board, me, iters):
        self.root_board = board
        self.me = me
        self.iters = iters
        self.cell = [-1]
        self.first = [-1]
        self.nch = [0]
        self.n = [0]
        self.w = [0]
        self.an = [0]
        self.aw = [0]
        self.rng = Rng(0x9E3779B97F4A7C15)
        self.done = 0

    def expand(self, node, board):
        if len(self.cell) + NC > MAX_NODES:
            return
        self.first[node] = len(self.cell)
        k = 0
        for c in range(NC):
            if board[c] == 0:
                self.cell.append(c)
                self.first.append(-1)
                self.nch.append(0)
                self.n.append(0)
                self.w.append(0)
                self.an.append(0)
                self.aw.append(0)
                k += 1
        self.nch[node] = k

    def select(self, node):
        best, bestv = -1, -1.0
        f = self.first[node]
        n, w, an, aw = self.n, self.w, self.an, self.aw
        for i in range(f, f + self.nch[node]):
            ni, ai = n[i], an[i]
            if ai == 0:
                v = (w[i] / ni) if ni > 0 else 0.5
            elif ni == 0:
                v = aw[i] / ai
            else:
                beta = ai / (ai + ni + RAVE_B * ai * ni)
                v = (1.0 - beta) * (w[i] / ni) + beta * (aw[i] / ai)
            if v > bestv:
                bestv, best = v, i
        return best

    def run(self):
        me = self.me
        self.expand(0, self.root_board)
        for _ in range(self.iters):
            board = list(self.root_board)
            node, p = 0, me
            path = [0]
            while self.first[node] >= 0 and self.nch[node] > 0:
                node = self.select(node)
                board[self.cell[node]] = p
                p = 3 - p
                path.append(node)
            if self.first[node] < 0 and self.n[node] >= EXPAND_N:
                self.expand(node, board)
                if self.first[node] >= 0 and self.nch[node] > 0:
                    node = self.first[node]
                    board[self.cell[node]] = p
                    p = 3 - p
                    path.append(node)
            # random fill playout
            empt = [c for c in range(NC) if board[c] == 0]
            k = len(empt)
            for i in range(k - 1, 0, -1):
                j = ((self.rng.next() >> 32) * (i + 1)) >> 32
                empt[i], empt[j] = empt[j], empt[i]
            q = p
            for c in empt:
                board[c] = q
                q = 3 - q
            winner = RED if connected(board, RED) else BLUE
            # backprop: node at depth d was entered by player me if d odd
            mover = me
            pl = me  # player to move at path[d]
            for d, nd in enumerate(path):
                self.n[nd] += 1
                if d > 0:
                    if winner == mover:
                        self.w[nd] += 1
                    mover = 3 - mover
                f = self.first[nd]
                if f >= 0:
                    win = 1 if winner == pl else 0
                    for i in range(f, f + self.nch[nd]):
                        if board[self.cell[i]] == pl:
                            self.an[i] += 1
                            self.aw[i] += win
                pl = 3 - pl
            self.done += 1
        f = self.first[0]
        best, bn = -1, -1
        for i in range(f, f + self.nch[0]):
            if self.n[i] > bn:
                bn, best = self.n[i], i
        return self.cell[best]


def name_of(c):
    return "%s%d" % (chr(97 + c % N), c // N + 1)


def choose(board, me, last, iters):
    """Return (move text, info)."""
    reds = board.count(RED)
    blues = board.count(BLUE)
    if reds == 0 and blues == 0:
        return name_of(OPENING), "opening"
    if me == BLUE and reds == 1 and blues == 0 and last != "empty":
        c = board.index(RED)
        r, q = divmod(c, N)
        if 1 <= r <= N - 2 and 1 <= q <= N - 2:
            return "swap-pieces", "swap"
    opp = 3 - me
    for c in range(NC):
        if board[c] == 0:
            board[c] = me
            ok = connected(board, me)
            board[c] = 0
            if ok:
                return name_of(c), "win"
    for c in range(NC):
        if board[c] == 0:
            board[c] = opp
            ok = connected(board, opp)
            board[c] = 0
            if ok:
                return name_of(c), "block"
    m = Mcts(board, me, iters)
    c = m.run()
    return name_of(c), "mcts it=%d nodes=%d" % (m.done, len(m.cell))


def main():
    first = sys.stdin.readline().split()
    if not first:
        return
    me = RED if first[1] == "red" else BLUE
    env = os.environ.get("CG_NODES")
    turn = 0
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        last = line.split()[0] if line.split() else "empty"
        board = []
        for _ in range(N):
            row = sys.stdin.readline().replace(" ", "").strip()
            for ch in row[:N]:
                board.append(RED if ch == "r" else BLUE if ch == "b" else 0)
            board.extend([0] * (N - min(N, len(row))))
        iters = int(env) if env else (30000 if turn == 0 else 2600)
        mv, info = choose(board, me, last, iters)
        sys.stderr.write(info + "\n")
        sys.stdout.write(mv + "\n")
        sys.stdout.flush()
        turn += 1


if __name__ == "__main__":
    main()
