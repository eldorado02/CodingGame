#!/usr/bin/env python3
"""Onitama 5x5 — Python reference (template board-2p), v1 alpha-beta.

Coordinates (probe game): column A..E = x 0..4, row 1..5 with row 1 the
LAST input line; player 0 starts on row 1 and moves "up" (dy > 0).
Cards: the input gives each card in the frame of its next user (my cards
and the centre card for me, the opponent's cards for him). We store every
card in player 0's frame; player 1 uses the negated moves. Playing a card
swaps it with the centre card.
Win: capture the enemy master or put our master on the enemy shrine
(player 0 -> C5, player 1 -> C1). Eval: material + master distance to shrine.
"""
import os
import sys

SIZE = 5
TURN_NODES = 30000
STUDENT_VALUE = 100
SHRINE_DISTANCE_WEIGHT = 4
SHRINE = (2 + 4 * SIZE, 2)          # cell index (row r -> (r-1)*5 + x): P0 targets C5, P1 targets C1
EMPTY, MASTER, STUDENT = 0, 1, 2


def cell_index(x, row):
    return (row - 1) * SIZE + x


class State:
    __slots__ = ("kind", "owner", "hands", "center", "side")

    def __init__(self, kind, owner, hands, center, side):
        self.kind = kind        # 25 cells: EMPTY / MASTER / STUDENT
        self.owner = owner      # 25 cells: 0 / 1 (meaningless when empty)
        self.hands = hands      # [[c, c], [c, c]] card ids per player
        self.center = center
        self.side = side


CARDS = {}   # card id -> tuple of (dx, dy) in player 0's frame


def gen_moves(s):
    """(card, src, dst) in hand order, then cell order, then card move order.
    If no move exists, one PASS per card: (card, -1, -1)."""
    out = []
    sign = 1 if s.side == 0 else -1
    for card in s.hands[s.side]:
        for src in range(SIZE * SIZE):
            if s.kind[src] == EMPTY or s.owner[src] != s.side:
                continue
            x, row = src % SIZE, src // SIZE + 1
            for dx, dy in CARDS[card]:
                nx, nrow = x + sign * dx, row + sign * dy
                if not (0 <= nx < SIZE and 1 <= nrow <= SIZE):
                    continue
                dst = cell_index(nx, nrow)
                if s.kind[dst] != EMPTY and s.owner[dst] == s.side:
                    continue
                out.append((card, src, dst))
    if not out:
        out = [(card, -1, -1) for card in s.hands[s.side]]
    return out


def apply_move(s, m):
    card, src, dst = m
    kind, owner = s.kind[:], s.owner[:]
    if src >= 0:
        kind[dst], owner[dst] = kind[src], owner[src]
        kind[src] = EMPTY
    hands = [s.hands[0][:], s.hands[1][:]]
    hands[s.side][hands[s.side].index(card)] = s.center
    return State(kind, owner, hands, card, 1 - s.side)


def master_of(s, p):
    for i in range(SIZE * SIZE):
        if s.kind[i] == MASTER and s.owner[i] == p:
            return i
    return -1


def winner(s):
    """Winning player after the last move, or -1."""
    for p in (0, 1):
        m = master_of(s, p)
        if m < 0:
            return 1 - p
        if m == SHRINE[p]:
            return p
    return -1


def distance(a, b):
    return max(abs(a % SIZE - b % SIZE), abs(a // SIZE - b // SIZE))


def side_score(s, p):
    students = sum(1 for i in range(SIZE * SIZE) if s.kind[i] == STUDENT and s.owner[i] == p)
    return STUDENT_VALUE * students - SHRINE_DISTANCE_WEIGHT * distance(master_of(s, p), SHRINE[p])


def evaluate(s):
    return side_score(s, s.side) - side_score(s, 1 - s.side)


def order_key(s, m):
    """Captures first."""
    _, src, dst = m
    return 1 if src >= 0 and s.kind[dst] != EMPTY else 0


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
        w = winner(s)
        if w >= 0:
            return WIN - ply if w == s.side else -(WIN - ply)
        if depth == 0:
            return evaluate(s)
        mv = gen_moves(s)
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
def square(txt):
    if len(txt) != 2 or not ("A" <= txt[0] <= "E" and "1" <= txt[1] <= "5"):
        return None
    return cell_index(ord(txt[0]) - 65, int(txt[1]))


def name_of(i):
    return "%s%d" % (chr(65 + i % SIZE), i // SIZE + 1)


def parse_action(parts):
    if len(parts) < 2 or not parts[0].lstrip("-").isdigit():
        return None
    card = int(parts[0])
    if parts[1] == "PASS":
        return (card, -1, -1)
    a, b = square(parts[1][:2]), square(parts[1][2:4])
    return None if a is None or b is None else (card, a, b)


def action_text(m):
    card, src, dst = m
    return "%d %s" % (card, "PASS" if src < 0 else name_of(src) + name_of(dst))


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = int(line.split()[0])
    while True:
        rows = []
        for _ in range(SIZE):
            r = sys.stdin.readline()
            if not r:
                return
            rows.append((r.rstrip("\r\n") + ".....")[:SIZE])
        kind, owner = [EMPTY] * 25, [0] * 25
        for li, r in enumerate(rows):
            row = SIZE - li
            for x, ch in enumerate(r):
                i = cell_index(x, row)
                if ch in "Ww":
                    kind[i], owner[i] = (MASTER if ch == "W" else STUDENT), 0
                elif ch in "Bb":
                    kind[i], owner[i] = (MASTER if ch == "B" else STUDENT), 1
        hands, center = [[], []], -1
        for _ in range(5):
            v = [int(t) for t in sys.stdin.readline().split()]
            card_owner, card_id, moves = v[0], v[1], v[2:10]
            user = me if card_owner == -1 else card_owner   # the centre card's next user is us
            sign = 1 if user == 0 else -1
            CARDS[card_id] = tuple((sign * moves[k], sign * moves[k + 1]) for k in range(0, 8, 2)
                                   if moves[k] or moves[k + 1])
            if card_owner == -1:
                center = card_id
            else:
                hands[card_owner].append(card_id)
        n = int(sys.stdin.readline())
        legal = []
        for _ in range(n):
            m = parse_action(sys.stdin.readline().split())
            if m is not None:
                legal.append(m)
        if not legal:
            print("%d PASS" % hands[me][0], flush=True)
            continue
        s = State(kind, owner, hands, center, me)
        print(action_text(Search().run(s, legal)), flush=True)


if __name__ == "__main__":
    main()
