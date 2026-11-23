#!/usr/bin/env python3
"""Nine Men's Morris — Python reference (template board-2p), v1 alpha-beta.

Fields and adjacency come from the init input ("A1:A4;D1"). Mills are the
aligned triples (same letter or same digit) a-b-c with b adjacent to a and
c. Phases: placing (stones in hand), moving (adjacent), flying (3 stones).
Forming a mill takes an enemy stone outside mills (any stone if all are in
mills). A side loses with fewer than 3 stones (nothing in hand) or no move.
Stones in hand = 9 - PLACE commands seen for that side (not in the input).
Eval (side to move): STONE_VALUE * stones + MILL_VALUE * mills + mobility.
"""
import os
import sys

STONES = 9
TURN_NODES = 15000
STONE_VALUE = 100
MILL_VALUE = 10
EMPTY = -1


class Board:
    def __init__(self, names, adjacency):
        self.names = names
        self.index = {n: i for i, n in enumerate(names)}
        self.adj = [sorted(self.index[m] for m in adjacency[n] if m in self.index) for n in names]
        mills = set()
        for b in range(len(names)):
            for a in self.adj[b]:
                for c in self.adj[b]:
                    if a < c and self.aligned(a, b, c):
                        mills.add((a, b, c))
        self.mills = sorted(mills)
        self.mills_of = [[m for m in self.mills if i in m] for i in range(len(names))]

    def aligned(self, a, b, c):
        na, nb, nc = self.names[a], self.names[b], self.names[c]
        return (na[0] == nb[0] == nc[0]) or (na[1:] == nb[1:] == nc[1:])


class State:
    __slots__ = ("owner", "hand", "side")

    def __init__(self, owner, hand, side):
        self.owner = owner    # per field: EMPTY, 0, 1
        self.hand = hand      # stones still to place per player
        self.side = side


def in_mill(board, owner, i):
    p = owner[i]
    return any(owner[a] == p and owner[b] == p and owner[c] == p for a, b, c in board.mills_of[i])


def take_targets(board, owner, enemy):
    stones = [i for i, o in enumerate(owner) if o == enemy]
    free = [i for i in stones if not in_mill(board, owner, i)]
    return free if free else stones


def gen_moves(board, s):
    """(src, dst, take); src = -1 for a placement, take = -1 without mill."""
    p, owner = s.side, s.owner
    if s.hand[p] > 0:
        steps = [(-1, d) for d in range(len(owner)) if owner[d] == EMPTY]
    else:
        mine = [i for i, o in enumerate(owner) if o == p]
        flying = len(mine) == 3
        steps = []
        for src in mine:
            targets = range(len(owner)) if flying else board.adj[src]
            steps.extend((src, d) for d in targets if owner[d] == EMPTY)
    out = []
    for src, dst in steps:
        after = owner[:]
        if src >= 0:
            after[src] = EMPTY
        after[dst] = p
        if in_mill(board, after, dst):
            out.extend((src, dst, t) for t in take_targets(board, after, 1 - p))
        else:
            out.append((src, dst, -1))
    return out


def apply_move(s, m):
    src, dst, take = m
    owner = s.owner[:]
    hand = s.hand[:]
    if src >= 0:
        owner[src] = EMPTY
    else:
        hand[s.side] -= 1
    owner[dst] = s.side
    if take >= 0:
        owner[take] = EMPTY
    return State(owner, hand, 1 - s.side)


def material(s, p):
    return sum(1 for o in s.owner if o == p) + s.hand[p]


def evaluate(board, s):
    def side(p):
        mills = sum(1 for a, b, c in board.mills if s.owner[a] == s.owner[b] == s.owner[c] == p)
        mobility = sum(1 for i, o in enumerate(s.owner) if o == p for d in board.adj[i] if s.owner[d] == EMPTY)
        return STONE_VALUE * material(s, p) + MILL_VALUE * mills + mobility
    return side(s.side) - side(1 - s.side)


def order_key(m):
    return 1 if m[2] >= 0 else 0


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


class Search:
    def __init__(self, board):
        self.board = board
        self.nodes = 0
        self.depth = 0

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        if material(s, s.side) < 3:
            return -(WIN - ply)
        mv = gen_moves(self.board, s)
        if not mv:
            return -(WIN - ply)
        if depth == 0:
            return evaluate(self.board, s)
        mv.sort(key=lambda m: -order_key(m))
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
        mv = sorted(mv, key=lambda m: -order_key(m))
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
def parse_command(board, text):
    """'PLACE;A1' / 'PLACE&TAKE;A1;D1' / 'MOVE;A1;D1' / 'MOVE&TAKE;A1;D1;D2'."""
    parts = text.strip().split(";")
    fields = [board.index.get(p.strip()) for p in parts[1:]]
    if None in fields or not fields:
        return None
    kind = parts[0].strip()
    if kind == "PLACE" and len(fields) == 1:
        return (-1, fields[0], -1)
    if kind == "PLACE&TAKE" and len(fields) == 2:
        return (-1, fields[0], fields[1])
    if kind == "MOVE" and len(fields) == 2:
        return (fields[0], fields[1], -1)
    if kind == "MOVE&TAKE" and len(fields) == 3:
        return (fields[0], fields[1], fields[2])
    return None


def main():
    read = sys.stdin.readline
    first = read()
    if not first:
        return
    me = int(first.split()[0])
    names, adjacency = [], {}
    for _ in range(int(read())):
        line = read().strip()
        name, _, rest = line.partition(":")
        names.append(name.strip())
        adjacency[name.strip()] = [x.strip() for x in rest.split(";") if x.strip()]
    board = Board(names, adjacency)
    placed = [0, 0]
    while True:
        op_move = read()
        if not op_move:
            return
        if op_move.strip().startswith("PLACE"):
            placed[1 - me] += 1
        owner = [EMPTY] * len(names)
        for item in read().strip().split(";"):
            name, _, state = item.partition(":")
            if name.strip() in board.index and state.strip() in ("0", "1"):
                owner[board.index[name.strip()]] = int(state)
        commands = [read().strip() for _ in range(int(read()))]
        legal = []
        texts = {}
        for c in commands:
            m = parse_command(board, c)
            if m is not None and m not in texts:
                legal.append(m)
                texts[m] = c
        if not legal:
            print(commands[0] if commands else "PLACE;A1", flush=True)
            continue
        s = State(owner, [STONES - placed[0], STONES - placed[1]], me)
        m = Search(board).run(s, legal)
        if m[0] < 0:
            placed[me] += 1
        print(texts[m], flush=True)


if __name__ == "__main__":
    main()
