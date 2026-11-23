#!/usr/bin/env python3
"""Isola 9x9 — Python reference (template board-2p), v1.

A turn = move the pawn one king step to a free tile, then remove a free tile
(not under a pawn). A player who cannot move on his turn loses.
Search: negamax alpha-beta over (move, removal) pairs; removals restricted to
free tiles adjacent to the opponent pawn after our move (his liberties).
Eval (side to move): Voronoi (king-step BFS, ties neutral) + 3 * mobility diff.
Index = y * 9 + x.
"""
import os
import sys

W = 9
MOBILITY_WEIGHT = 3
TURN_NODES = 10000
NB = []
for _i in range(81):
    _x, _y = _i % W, _i // W
    NB.append(tuple((_y + dy) * W + _x + dx
                    for dy in (-1, 0, 1) for dx in (-1, 0, 1)
                    if (dx or dy) and 0 <= _x + dx < W and 0 <= _y + dy < W))


class State:
    __slots__ = ("gone", "pos", "side")

    def __init__(self, gone, pos, side):
        self.gone = gone      # list of 81 bools (removed tiles)
        self.pos = pos        # [pawn of side 0, pawn of side 1]
        self.side = side      # side to move


def free(s, t):
    return not s.gone[t] and t != s.pos[0] and t != s.pos[1]


def moves_of(s, p):
    return [t for t in NB[s.pos[p]] if free(s, t)]


def gen_actions(s):
    """(move, removal) pairs in fixed order."""
    me, op = s.side, 1 - s.side
    out = []
    for t in NB[s.pos[me]]:
        if not free(s, t):
            continue
        old = s.pos[me]
        s.pos[me] = t
        rem = [r for r in NB[s.pos[op]] if free(s, r)]
        if not rem:
            rem = [next(r for r in range(81) if free(s, r))]
        s.pos[me] = old
        for r in rem:
            out.append((t, r))
    return out


def apply_action(s, a):
    gone = s.gone[:]
    pos = s.pos[:]
    pos[s.side] = a[0]
    gone[a[1]] = True
    return State(gone, pos, 1 - s.side)


def voronoi(s):
    """Tiles reached strictly first by side to move minus by the other."""
    dist = [[-1] * 81, [-1] * 81]
    for p in (0, 1):
        d = dist[p]
        d[s.pos[p]] = 0
        q = [s.pos[p]]
        h = 0
        while h < len(q):
            i = q[h]
            h += 1
            for t in NB[i]:
                if d[t] < 0 and free(s, t):
                    d[t] = d[i] + 1
                    q.append(t)
    me, op = s.side, 1 - s.side
    sc = 0
    for i in range(81):
        a, b = dist[me][i], dist[op][i]
        if a > 0 and (b < 0 or a < b):
            sc += 1
        elif b > 0 and (a < 0 or b < a):
            sc -= 1
    return sc


def evaluate(s):
    me, op = s.side, 1 - s.side
    return voronoi(s) + MOBILITY_WEIGHT * (len(moves_of(s, me)) - len(moves_of(s, op)))


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
        if not moves_of(s, s.side):
            return -(WIN - ply)
        if depth == 0:
            return evaluate(s)
        best = -INF
        for a in gen_actions(s):
            v = -self.negamax(apply_action(s, a), depth - 1, -beta, -alpha, ply + 1)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, root):
        mv = gen_actions(root)
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur, cur_idx = -INF, -INF, mv[0], 0
            try:
                for i, a in enumerate(mv):
                    v = -self.negamax(apply_action(root, a), depth - 1, -INF, -alpha, 1)
                    if v > best_v:
                        best_v, cur, cur_idx = v, a, i
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
def read_int():
    line = sys.stdin.readline()
    if not line:
        raise EOFError
    return int(line.split()[0])


def main():
    try:
        mx, my = read_int(), read_int()
    except (EOFError, ValueError, IndexError):
        return
    gone = [False] * 81
    me_pos = my * W + mx
    me = 0 if mx == 0 else 1
    while True:
        try:
            ox, oy, rx, ry = read_int(), read_int(), read_int(), read_int()
        except (EOFError, ValueError, IndexError):
            return
        if 0 <= rx < W and 0 <= ry < W:
            gone[ry * W + rx] = True
        pos = [0, 0]
        pos[me] = me_pos
        pos[1 - me] = oy * W + ox
        s = State(gone, pos, me)
        if not moves_of(s, me):
            print("RANDOM", flush=True)
            continue
        a = Search().run(s)
        me_pos = a[0]
        gone[a[1]] = True
        print("%d %d %d %d" % (a[0] % W, a[0] // W, a[1] % W, a[1] // W), flush=True)


if __name__ == "__main__":
    main()
