#!/usr/bin/env python3
"""Mad Knights (3-player knight isolation) — Python reference, v1.

Paranoid search: we maximise, both opponents minimise our score; alpha-beta,
iterative deepening, deterministic node budget (env CG_NODES).
Index = y * 8 + x, y = 0 is the first board line (row 8), x = 0 = column a.
Blocked squares: '#' and every knight square. A player without a move on
its turn is eliminated; the last player alive wins.
Turn order: r, g, b (skipping eliminated players).
"""
import os
import sys

COLORS = "rgb"
KN = ((1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2))
TARGETS = []
for _i in range(64):
    _x, _y = _i % 8, _i // 8
    TARGETS.append(tuple((_y + dy) * 8 + _x + dx for dx, dy in KN
                         if 0 <= _x + dx < 8 and 0 <= _y + dy < 8))


class State:
    __slots__ = ("blocked", "pos", "alive", "turn")

    def __init__(self, blocked, pos, alive, turn):
        self.blocked = blocked
        self.pos = pos
        self.alive = alive
        self.turn = turn


def moves_of(blocked, sq):
    return [t for t in TARGETS[sq] if not (blocked >> t) & 1]


def mob(blocked, sq):
    n = 0
    for t in TARGETS[sq]:
        if not (blocked >> t) & 1:
            n += 1
    return n


def next_alive(alive, turn):
    for k in range(1, 4):
        j = (turn + k) % 3
        if alive[j]:
            return j
    return turn


def apply_move(s, t):
    pos = s.pos[:]
    pos[s.turn] = t
    return State(s.blocked | (1 << t), pos, s.alive, next_alive(s.alive, s.turn))


def eliminate(s):
    alive = s.alive[:]
    alive[s.turn] = False
    return State(s.blocked, s.pos, alive, next_alive(alive, s.turn))


ME = 0


def evaluate(s):
    sc = 0
    b = s.blocked
    for t in moves_of(b, s.pos[ME]):
        sc += 10 + mob(b | (1 << t), t)
    for j in range(3):
        if j != ME and s.alive[j]:
            sc -= 6 * mob(b, s.pos[j])
    return sc


def order_key(s, t):
    """Warnsdorff-like for us, also for opponents (fewest onward moves last)."""
    return mob(s.blocked | (1 << t), t)


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "60000"))


class Aborted(Exception):
    pass


class Search:
    def __init__(self):
        self.nodes = 0
        self.depth = 0

    def minimax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        if not s.alive[ME]:
            return -(WIN - ply)
        if sum(1 for a in s.alive if a) == 1:
            return WIN - ply
        mv = moves_of(s.blocked, s.pos[s.turn])
        if not mv:
            return self.minimax(eliminate(s), depth, alpha, beta, ply)
        if depth == 0:
            return evaluate(s)
        mv.sort(key=lambda t: -order_key(s, t))
        if s.turn == ME:
            best = -INF
            for t in mv:
                v = self.minimax(apply_move(s, t), depth - 1, alpha, beta, ply + 1)
                if v > best:
                    best = v
                if v > alpha:
                    alpha = v
                if alpha >= beta:
                    break
            return best
        best = INF
        for t in mv:
            v = self.minimax(apply_move(s, t), depth - 1, alpha, beta, ply + 1)
            if v < best:
                best = v
            if v < beta:
                beta = v
            if alpha >= beta:
                break
        return best

    def run(self, root, mv):
        mv = list(mv)
        mv.sort(key=lambda t: -order_key(root, t))
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur, cur_idx = -INF, -INF, mv[0], 0
            try:
                for i, t in enumerate(mv):
                    v = self.minimax(apply_move(root, t), depth - 1, alpha, INF, 1)
                    if v > best_v:
                        best_v, cur, cur_idx = v, t, i
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
def sq_of(t):
    return (8 - (ord(t[1]) - 48)) * 8 + (ord(t[0]) - 97)


def name_of(i):
    return "%s%d" % (chr(97 + i % 8), 8 - i // 8)


def valid_sq(t):
    return len(t) >= 2 and "a" <= t[0] <= "h" and "1" <= t[1] <= "8"


def main():
    global ME
    line = sys.stdin.readline()
    if not line:
        return
    c = line.strip()[:1]
    ME = COLORS.index(c) if c in COLORS else 0
    while True:
        alive = [True, True, True]
        for _ in range(3):
            l = sys.stdin.readline()
            if not l:
                return
            parts = l.split()
            if len(parts) >= 2 and parts[0] in COLORS:
                alive[COLORS.index(parts[0])] = parts[1] == "1"
        blocked = 0
        pos = [0, 0, 0]
        for y in range(8):
            row = (sys.stdin.readline().rstrip("\r\n") + "########")[:8]
            for x in range(8):
                ch = row[x]
                if ch != ".":
                    blocked |= 1 << (y * 8 + x)
                if ch in COLORS:
                    pos[COLORS.index(ch)] = y * 8 + x
        n_line = sys.stdin.readline()
        if not n_line:
            return
        legal = []
        for _ in range(int(n_line)):
            t = sys.stdin.readline().split()
            if t and valid_sq(t[0]):
                legal.append(sq_of(t[0]))
        alive[ME] = True
        s = State(blocked, pos, alive, ME)
        if not legal:
            print("random", flush=True)
            continue
        print(name_of(Search().run(s, legal)), flush=True)


if __name__ == "__main__":
    main()
