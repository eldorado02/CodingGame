#!/usr/bin/env python3
"""Ataxx 7x7 — Python reference (template board-2p), v2 alpha-beta.

Bit index = y * 7 + x, y = 0 is the FIRST input line (row 7: verified in
test-game logs, the statement says the opposite), x = 0 is column a.
Spawn: new piece on an empty square adjacent to one of ours. Jump: move a
piece exactly 2 squares away (Chebyshev). Afterwards adjacent enemies flip.
A side without move passes; game over when nobody can move or a side is
empty. Eval (side to move): piece difference.
"""
import os
import sys

SIZE = 7
CELLS = SIZE * SIZE
ALL = (1 << CELLS) - 1
TURN_NODES = 120000
SPAWN_ORDER_BONUS = 1   # spawns add a piece: try them before equal jumps


def ring_masks(dist):
    masks = []
    for i in range(CELLS):
        x, y = i % SIZE, i // SIZE
        m = 0
        for j in range(CELLS):
            jx, jy = j % SIZE, j // SIZE
            if max(abs(jx - x), abs(jy - y)) == dist:
                m |= 1 << j
        masks.append(m)
    return masks


ADJ = ring_masks(1)
JUMP = ring_masks(2)


def popcount(x):
    return bin(x).count("1")


def bits(x):
    """Set bit indexes in ascending order."""
    out = []
    while x:
        low = x & -x
        out.append(low.bit_length() - 1)
        x ^= low
    return out


class State:
    __slots__ = ("mine", "theirs", "walls")

    def __init__(self, mine, theirs, walls):
        self.mine = mine        # pieces of the side to move
        self.theirs = theirs
        self.walls = walls

    def empty(self):
        return ALL & ~(self.mine | self.theirs | self.walls)


def gen_moves(s):
    """(src, dst); src = -1 for a spawn. Spawns by dst, then jumps by src, dst."""
    empty = s.empty()
    spawn_targets = 0
    for p in bits(s.mine):
        spawn_targets |= ADJ[p]
    out = [(-1, t) for t in bits(spawn_targets & empty)]
    for p in bits(s.mine):
        out.extend((p, t) for t in bits(JUMP[p] & empty))
    return out


def apply_move(s, m):
    src, dst = m
    mine = s.mine | (1 << dst)
    if src >= 0:
        mine &= ~(1 << src)
    flipped = ADJ[dst] & s.theirs
    mine |= flipped
    # the side to move switches: swap the point of view
    return State(s.theirs & ~flipped, mine, s.walls)


def order_key(s, m):
    flips = popcount(ADJ[m[1]] & s.theirs)
    return 2 * flips + (SPAWN_ORDER_BONUS if m[0] < 0 else 0)


def evaluate(s):
    return popcount(s.mine) - popcount(s.theirs)


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


def final_score(s, ply):
    diff = evaluate(s)
    if diff > 0:
        return WIN - ply
    if diff < 0:
        return -(WIN - ply)
    return 0


class Search:
    def __init__(self):
        self.nodes = 0
        self.depth = 0

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        if not s.mine or not s.theirs:
            return final_score(s, ply)
        if depth == 0:
            return evaluate(s)
        mv = gen_moves(s)
        if not mv:
            passed = State(s.theirs, s.mine, s.walls)
            if not gen_moves(passed):
                return final_score(s, ply)
            return -self.negamax(passed, depth - 1, -beta, -alpha, ply + 1)
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


# ------------------------------------------------------------- GAME: I/O
def square(txt):
    """'b7' -> index, None if malformed."""
    if len(txt) != 2 or not ("a" <= txt[0] <= "g" and "1" <= txt[1] <= "7"):
        return None
    return (7 - int(txt[1])) * SIZE + ord(txt[0]) - 97


def parse_action(txt):
    if len(txt) == 2:
        t = square(txt)
        return None if t is None else (-1, t)
    if len(txt) == 4:
        a, b = square(txt[:2]), square(txt[2:])
        return None if a is None or b is None else (a, b)
    return None


def name_of(i):
    return "%s%d" % (chr(97 + i % SIZE), 7 - i // SIZE)


def action_text(m):
    return name_of(m[1]) if m[0] < 0 else name_of(m[0]) + name_of(m[1])


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = line.strip()[:1] or "0"
    while True:
        mine = theirs = walls = 0
        for y in range(SIZE):
            row = sys.stdin.readline()
            if not row:
                return
            row = (row.rstrip("\r\n") + "#######")[:SIZE]
            for x, ch in enumerate(row):
                bit = 1 << (y * SIZE + x)
                if ch == "#":
                    walls |= bit
                elif ch == me:
                    mine |= bit
                elif ch in "01":
                    theirs |= bit
        if not sys.stdin.readline():
            return
        n_line = sys.stdin.readline()
        if not n_line:
            return
        legal = []
        for _ in range(int(n_line)):
            t = sys.stdin.readline().split()
            m = parse_action(t[0]) if t else None
            if m is not None:
                legal.append(m)
        if not legal:
            print("random", flush=True)
            continue
        best = Search().run(State(mine, theirs, walls), legal)
        print(action_text(best), flush=True)


if __name__ == "__main__":
    main()
