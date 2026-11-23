#!/usr/bin/env python3
"""Yavalath (CodinGame) — Python reference of the differential fuzz.

Board: 9 rows of 5,6,7,8,9,8,7,6,5 cells (referee MultiStruct/Yavalath).
Cell index = order of the input rows. Output "x y" = index in row, row.
Axial: Q = x - min(y, 4) + 4 ; directions (dQ, dy): E(1,0) W(-1,0)
DR(0,1) UL(0,-1) DL(-1,1) UR(1,-1). A move making a line >= 4 wins, else a
line of exactly 3 loses (longest line through the move, referee rule).

v1: negamax alpha-beta + iterative deepening, node budget (env CG_NODES).
Forced blocks (opponent's winning cells) are searched without depth cost.
Eval: 4-windows with 2 own stones and 2 empty, safe-move difference.
"""
import os
import sys

ROWS = [9 - abs(4 - y) for y in range(9)]
CELLS = []
for _y in range(9):
    for _x in range(ROWS[_y]):
        CELLS.append((_x, _y))
NC = len(CELLS)  # 61
IDX = {}
for _i, (_x, _y) in enumerate(CELLS):
    IDX[(_x - min(_y, 4) + 4, _y)] = _i
DIRS = ((1, 0), (-1, 0), (0, 1), (0, -1), (-1, 1), (1, -1))
RAY = []  # RAY[c][d] = up to 3 cells
for _i, (_x, _y) in enumerate(CELLS):
    q, r = _x - min(_y, 4) + 4, _y
    rays = []
    for dq, dr in DIRS:
        lst = []
        for k in range(1, 4):
            t = IDX.get((q + dq * k, r + dr * k))
            if t is None:
                break
            lst.append(t)
        rays.append(tuple(lst))
    RAY.append(tuple(rays))
WINDOWS = []
for _i in range(NC):
    for d in (0, 2, 4):
        if len(RAY[_i][d]) == 3:
            WINDOWS.append((_i,) + RAY[_i][d])
WIN_OF = [[] for _ in range(NC)]
for _w, win in enumerate(WINDOWS):
    for c in win:
        WIN_OF[c].append(_w)

WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
W_TWO, W_THREE, W_SAFE = 10, 40, 3
OPENING = 7  # (x=1, y=1)


def place_result(b, c, p):
    """2 = line >= 4 (win), 1 = line of exactly 3 (loss), 0 = neutral."""
    best = 1
    ray = RAY[c]
    for a in (0, 2, 4):
        n = 1
        for t in ray[a]:
            if b[t] != p:
                break
            n += 1
        for t in ray[a + 1]:
            if b[t] != p:
                break
            n += 1
        if n > best:
            best = n
    if best >= 4:
        return 2
    return 1 if best == 3 else 0


def evaluate(b, p, safe_p, safe_o):
    o = 3 - p
    s = 0
    for win in WINDOWS:
        a = n = 0
        for c in win:
            v = b[c]
            if v == p:
                a += 1
            elif v == o:
                n += 1
        if n == 0:
            if a == 2:
                s += W_TWO
            elif a == 3:
                s += W_THREE
        elif a == 0:
            if n == 2:
                s -= W_TWO
            elif n == 3:
                s -= W_THREE
    return s + W_SAFE * (safe_p - safe_o)


def order_key(b, c, p):
    o = 3 - p
    k = 0
    for w in WIN_OF[c]:
        a = n = 0
        for t in WINDOWS[w]:
            v = b[t]
            if v == p:
                a += 1
            elif v == o:
                n += 1
        if n == 0:
            k += (0, 1, 4, 8)[a]
        elif a == 0:
            k += (0, 0, 3, 6)[n]
    return k


def sort_moves(b, mv, p):
    keys = [order_key(b, c, p) for c in mv]
    for i in range(1, len(mv)):
        x, k, j = mv[i], keys[i], i - 1
        while j >= 0 and keys[j] < k:
            mv[j + 1], keys[j + 1] = mv[j], keys[j]
            j -= 1
        mv[j + 1], keys[j + 1] = x, k


def analyse(b, p):
    """Return (kind, moves, safe_p, safe_o): kind 'win', 'draw', 'forced',
    'free', 'lost'. For 'forced' moves are (cell, loses_now) pairs."""
    o = 3 - p
    forced = []
    free = []
    safe_o = 0
    empty = 0
    for c in range(NC):
        if b[c]:
            continue
        empty += 1
        rp = place_result(b, c, p)
        if rp == 2:
            return "win", [c], 0, 0
        ro = place_result(b, c, o)
        if ro == 2:
            forced.append((c, rp == 1))
        elif ro == 0:
            safe_o += 1
        if rp == 0:
            free.append(c)
    if empty == 0:
        return "draw", [], 0, 0
    if forced:
        return "forced", forced, len(free), safe_o
    if not free:
        return "lost", [], 0, safe_o
    return "free", free, len(free), safe_o


class Search:
    def __init__(self, limit):
        self.limit = limit
        self.nodes = 0
        self.aborted = False
        self.last_depth = 0

    def negamax(self, b, p, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > self.limit:
            self.aborted = True
            return 0
        kind, mv, sp, so = analyse(b, p)
        if kind == "win":
            return WIN - ply
        if kind == "draw":
            return 0
        if kind == "lost":
            return -(WIN - ply - 1)
        best = -INF
        if kind == "forced":
            for c, loses in mv:
                if loses:
                    v = -(WIN - ply - 1)
                else:
                    b[c] = p
                    v = -self.negamax(b, 3 - p, depth, -beta, -alpha, ply + 1)
                    b[c] = 0
                    if self.aborted:
                        return 0
                if v > best:
                    best = v
                if v > alpha:
                    alpha = v
                if alpha >= beta:
                    break
            return best
        if depth <= 0:
            return evaluate(b, p, sp, so)
        sort_moves(b, mv, p)
        for c in mv:
            b[c] = p
            v = -self.negamax(b, 3 - p, depth - 1, -beta, -alpha, ply + 1)
            b[c] = 0
            if self.aborted:
                return 0
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, b, p, mv):
        """mv: candidate root cells (non-empty list)."""
        mv = list(mv)
        sort_moves(b, mv, p)
        best = mv[0]
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur_i = -INF, -INF, 0
            for i, c in enumerate(mv):
                b[c] = p
                v = -self.negamax(b, 3 - p, depth - 1, -INF, -alpha, 1)
                b[c] = 0
                if self.aborted:
                    break
                if v > best_v:
                    best_v, cur_i = v, i
                if v > alpha:
                    alpha = v
            if self.aborted:
                break
            best = mv[cur_i]
            self.last_depth = depth
            mv.insert(0, mv.pop(cur_i))
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
        return best


def choose(b, my_id, first_turn, last, limit):
    """Return (cell or -1 for steal of `last`, info)."""
    stones = sum(1 for v in b if v)
    if stones == 0:
        return OPENING, "opening"
    if my_id == 2 and first_turn and stones == 1 and last >= 0:
        x, y = CELLS[last]
        q, r = x - min(y, 4) + 4, y
        dist = max(abs(q - 4), abs(r - 4), abs((q + r) - 8))
        if dist <= 2:
            return -1, "steal"
    kind, mv, _, _ = analyse(b, 1)
    if kind == "win":
        return mv[0], "win"
    if kind == "forced":
        safe = [c for c, loses in mv if not loses]
        if len(safe) == 1 or not safe:
            return (safe or [mv[0][0]])[0], "forced"
        cand = safe
    elif kind == "free":
        cand = mv
    else:
        cand = [c for c in range(NC) if b[c] == 0]
        return cand[0], "lost"
    s = Search(limit)
    c = s.run(b, 1, cand)
    return c, "d=%d n=%d" % (s.last_depth, s.nodes)


def main():
    env = os.environ.get("CG_NODES")
    my_id = 0
    turn = 0
    first_line = True
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        if not line.strip():
            continue
        if first_line:
            my_id = int(line.split()[0])
            first_line = False
            line = sys.stdin.readline()
        count = int(line.split()[0])
        b = []
        for _ in range(count):
            row = sys.stdin.readline().strip()
            for ch in row:
                b.append(1 if ch == "1" else 2 if ch == "2" else 0)
        b = (b + [0] * NC)[:NC]
        ox, oy = map(int, sys.stdin.readline().split()[:2])
        last = -1
        if 0 <= oy < 9 and 0 <= ox < ROWS[oy]:
            last = IDX[(ox - min(oy, 4) + 4, oy)]
        limit = int(env) if env else (140000 if turn == 0 else 14000)
        c, info = choose(b, my_id, turn == 0, last, limit)
        if c < 0:
            c = last
        sys.stderr.write(info + "\n")
        sys.stdout.write("%d %d\n" % CELLS[c])
        sys.stdout.flush()
        turn += 1


if __name__ == "__main__":
    main()
