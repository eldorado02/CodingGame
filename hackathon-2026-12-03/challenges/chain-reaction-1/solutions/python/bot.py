#!/usr/bin/env python3
"""Chain Reaction 6x6 — Python reference (template board-2p), v1.

Index = y * 6 + x, y = 0 is the first input row (row 6), x = 0 is column a.
Owner: 0 none, 1 red, 2 blue. A move adds one orb to an empty or own cell;
a cell reaching its critical mass (number of neighbours) explodes: it loses
that many orbs and each neighbour gets one orb and the mover's colour.
Explosions are resolved in rounds (all critical cells of a round, index
order) until stable or the opponent has no cell left.
Orbs are conserved, so total orbs = plies played: a side with no cell loses
once total >= 2 (both players have moved).
"""
import os
import sys

N = 36
NB = []
for _i in range(N):
    _x, _y = _i % 6, _i // 6
    _l = []
    for _dx, _dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        if 0 <= _x + _dx < 6 and 0 <= _y + _dy < 6:
            _l.append((_y + _dy) * 6 + _x + _dx)
    NB.append(tuple(_l))
CRIT = tuple(len(n) for n in NB)
MAX_ROUNDS = 200


class State:
    __slots__ = ("own", "orb", "side")

    def __init__(self, own, orb, side):
        self.own = own
        self.orb = orb
        self.side = side


def gen_moves(s):
    return [i for i in range(N) if s.own[i] == 0 or s.own[i] == s.side]


def count(s, p):
    return sum(1 for o in s.own if o == p)


def apply_move(s, i):
    own = s.own[:]
    orb = s.orb[:]
    p = s.side
    o = 3 - p
    own[i] = p
    orb[i] += 1
    rounds = 0
    while rounds < MAX_ROUNDS:
        crit = [j for j in range(N) if orb[j] >= CRIT[j]]
        if not crit:
            break
        for j in crit:
            orb[j] -= CRIT[j]
            if orb[j] == 0:
                own[j] = 0
            for k in NB[j]:
                orb[k] += 1
                own[k] = p
        rounds += 1
        if o not in own:
            break
    return State(own, orb, o)


def winner(s):
    """0 none, else winning colour."""
    total = sum(s.orb)
    if total < 2:
        return 0
    r = b = 0
    for o in s.own:
        if o == 1:
            r += 1
        elif o == 2:
            b += 1
    if r == 0 and b > 0:
        return 2
    if b == 0 and r > 0:
        return 1
    return 0


def side_score(s, p):
    o = 3 - p
    sc = 0
    for i in range(N):
        if s.own[i] != p:
            continue
        sc += s.orb[i]
        vulnerable = False
        for k in NB[i]:
            if s.own[k] == o and s.orb[k] == CRIT[k] - 1:
                vulnerable = True
                sc -= 5 - CRIT[i]
        if not vulnerable:
            if CRIT[i] == 2:
                sc += 3
            elif CRIT[i] == 3:
                sc += 2
            if s.orb[i] == CRIT[i] - 1:
                sc += 2
    return sc


def evaluate(s):
    return side_score(s, s.side) - side_score(s, 3 - s.side)


def order_key(s, i):
    """Critical own cells first (they explode), then corners/edges."""
    k = 0
    if s.own[i] == s.side and s.orb[i] == CRIT[i] - 1:
        k += 4
    k += 4 - CRIT[i]
    return k


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "20000"))


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
        w = winner(s)
        if w:
            return WIN - ply if w == s.side else -(WIN - ply)
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
            self.depth = depth
            mv.insert(0, mv.pop(cur_idx))
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
        return best


# ------------------------------------------------------------- GAME: I/O
def name_of(i):
    return "%s%d" % (chr(97 + i % 6), 6 - i // 6)


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = 2 if line.strip().lower().startswith("b") else 1
    while True:
        if not sys.stdin.readline():
            return
        own = [0] * N
        orb = [0] * N
        for y in range(6):
            row = sys.stdin.readline()
            if not row:
                return
            row = row.rstrip("\r\n")
            for x in range(6):
                tok = row[2 * x:2 * x + 2]
                if len(tok) == 2 and tok[0] in "rb" and tok[1] in "123":
                    own[y * 6 + x] = 1 if tok[0] == "r" else 2
                    orb[y * 6 + x] = int(tok[1])
        s = State(own, orb, me)
        mv = gen_moves(s)
        if not mv:
            print("random", flush=True)
            continue
        print(name_of(Search().run(s, mv)), flush=True)


if __name__ == "__main__":
    main()
