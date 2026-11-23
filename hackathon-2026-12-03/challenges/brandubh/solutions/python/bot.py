#!/usr/bin/env python3
"""Brandubh 7x7 — Python reference (template board-2p), v9.

Rules follow the referee (github.com/n3l-ctrl/brandubh-codingame Referee.java):
  * rook moves; only the King may stop on the throne (d4) or a corner; others
    may pass over the empty throne;
  * only the moved piece captures (custodial, 2 opposite sides); corners are
    hostile to all; the empty throne is hostile except to the King; the
    occupied throne is hostile to attackers only; the King captures too;
  * King on the throne needs 4 hostile sides; elsewhere 2 (empty throne not
    hostile to him);
  * after a move: King gone -> attackers win; King on a corner -> defenders
    win; defenders unreachable from the border -> attackers win;
  * the mover loses on a 3rd repetition of (board, side to move).
Index = y * 7 + x, y = 0 is row "7" (first input line), x = 0 is column "a".
"""
import os
import sys

EMPTY, ATT, DEF, KING = 0, 1, 2, 3
SIDE_A, SIDE_D = 0, 1
THRONE = 24
CORNERS = (0, 6, 42, 48)
CORNER_DIAG = (8, 12, 36, 40)  # b6 f6 b2 f2
DIRS = ((1, 0), (-1, 0), (0, 1), (0, -1))
CHAR = {".": EMPTY, "A": ATT, "D": DEF, "K": KING}
KDIST = (0, -600, -250, -80, 0)


def is_corner(i):
    return i == 0 or i == 6 or i == 42 or i == 48


class State:
    __slots__ = ("cell", "side")

    def __init__(self, cell, side):
        self.cell = cell
        self.side = side


def side_of(v):
    return SIDE_A if v == ATT else SIDE_D


def gen_moves(s):
    """(from, to) by ascending square, then DIRS order, then distance."""
    out = []
    c = s.cell
    for i in range(49):
        v = c[i]
        if v == EMPTY or side_of(v) != s.side:
            continue
        x0, y0 = i % 7, i // 7
        for dx, dy in DIRS:
            x, y = x0 + dx, y0 + dy
            while 0 <= x < 7 and 0 <= y < 7:
                j = y * 7 + x
                if c[j] != EMPTY:
                    break
                if v == KING or not (j == THRONE or is_corner(j)):
                    out.append((i, j))
                x += dx
                y += dy
    return out


def hostile(c, x, y, cap_type, cap_king):
    if x < 0 or x > 6 or y < 0 or y > 6:
        return False
    j = y * 7 + x
    if is_corner(j):
        return True
    target_att = cap_type != ATT
    if j == THRONE:
        if c[THRONE] == EMPTY:
            return not cap_king
        return target_att
    p = c[j]
    if p != EMPTY:
        return p != ATT if target_att else p == ATT
    return False


def is_enemy(a, b):
    return b != ATT if a == ATT else b == ATT


def apply_move(s, m):
    c = s.cell[:]
    v = c[m[0]]
    c[m[0]] = EMPTY
    c[m[1]] = v
    ex, ey = m[1] % 7, m[1] // 7
    for dx, dy in DIRS:
        ax, ay = ex + dx, ey + dy
        if ax < 0 or ax > 6 or ay < 0 or ay > 6:
            continue
        a = ay * 7 + ax
        if c[a] == EMPTY or not is_enemy(v, c[a]):
            continue
        if c[a] == KING and a == THRONE:
            if all(hostile(c, 3 + ddx, 3 + ddy, ATT, True) for ddx, ddy in DIRS):
                c[a] = EMPTY
        elif hostile(c, ax + dx, ay + dy, v, c[a] == KING):
            c[a] = EMPTY
    return State(c, 1 - s.side)


def encircled(c):
    seen = [False] * 49
    queue = []
    for i in range(49):
        x, y = i % 7, i // 7
        if x == 0 or x == 6 or y == 0 or y == 6:
            if c[i] == KING or c[i] == DEF:
                return False
            if c[i] != ATT:
                seen[i] = True
                queue.append(i)
    h = 0
    while h < len(queue):
        i = queue[h]
        h += 1
        if c[i] == KING or c[i] == DEF:
            return False
        x, y = i % 7, i // 7
        for dx, dy in DIRS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < 7 and 0 <= ny < 7:
                j = ny * 7 + nx
                if not seen[j] and c[j] != ATT:
                    seen[j] = True
                    queue.append(j)
    return True


def king_pos(c):
    for i in range(49):
        if c[i] == KING:
            return i
    return -1


def winner(s):
    """Winning side after the last move, or -1."""
    k = king_pos(s.cell)
    if k < 0:
        return SIDE_A
    if is_corner(k):
        return SIDE_D
    if encircled(s.cell):
        return SIDE_A
    return -1


def king_lines(c, k):
    """Corners the King reaches in one move."""
    n = 0
    x0, y0 = k % 7, k // 7
    for dx, dy in DIRS:
        x, y = x0 + dx, y0 + dy
        while 0 <= x < 7 and 0 <= y < 7:
            j = y * 7 + x
            if c[j] != EMPTY:
                break
            if is_corner(j):
                n += 1
            x += dx
            y += dy
    return n


def king_dist(c, k):
    """Rook-move distance of the King to the nearest corner, capped at 4."""
    dist = [9] * 49
    dist[k] = 0
    queue = [k]
    h = 0
    while h < len(queue):
        i = queue[h]
        h += 1
        if is_corner(i):
            return min(dist[i], 4)
        if dist[i] >= 3:
            continue
        x0, y0 = i % 7, i // 7
        for dx, dy in DIRS:
            x, y = x0 + dx, y0 + dy
            while 0 <= x < 7 and 0 <= y < 7:
                j = y * 7 + x
                if c[j] != EMPTY:
                    break
                if dist[j] == 9:
                    dist[j] = dist[i] + 1
                    queue.append(j)
                x += dx
                y += dy
    return 4


def evaluate(s):
    c = s.cell
    k = king_pos(c)
    a = d = 0
    for v in c:
        if v == ATT:
            a += 1
        elif v == DEF:
            d += 1
    sc = 100 * a - 150 * d
    kx, ky = k % 7, k // 7
    for dx, dy in DIRS:
        x, y = kx + dx, ky + dy
        if 0 <= x < 7 and 0 <= y < 7 and c[y * 7 + x] == ATT:
            sc += 60
    for i in CORNER_DIAG:
        if c[i] == ATT:
            sc += 60
    sc += KDIST[king_dist(c, k)]
    if s.side == SIDE_A and king_lines(c, k) >= 2:
        sc -= 3000
    return sc if s.side == SIDE_A else -sc


def order_key(s, m):
    v = s.cell[m[0]]
    if v == KING:
        return 10 if is_corner(m[1]) else 1
    if v == ATT:
        k = king_pos(s.cell)
        dx = abs(m[1] % 7 - k % 7)
        dy = abs(m[1] // 7 - k // 7)
        return 2 if dx + dy == 1 else 0
    return 0


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", "12000"))


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
        if s.side == SIDE_D and king_lines(s.cell, king_pos(s.cell)):
            return WIN - (ply + 1)
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
def key_of(s):
    return "".join(".ADK"[v] for v in s.cell) + str(s.side)


def name_of(m):
    a, b = m
    return "%s%s%s%s" % (chr(97 + a % 7), chr(55 - a // 7), chr(97 + b % 7), chr(55 - b // 7))


def main():
    history = {}
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        try:
            role = int(line.split()[0])
        except (ValueError, IndexError):
            break
        sys.stdin.readline()  # last action: the board is given anyway
        cell = []
        for _ in range(7):
            row = sys.stdin.readline().rstrip("\r\n")
            row = (row + ".......")[:7]
            cell.extend(CHAR.get(ch, EMPTY) for ch in row)
        s = State(cell, SIDE_A if role == 0 else SIDE_D)
        k = key_of(s)
        history[k] = history.get(k, 0) + 1
        moves = gen_moves(s)
        # never create a 3rd repetition (the mover loses)
        safe = [m for m in moves if history.get(key_of(apply_move(s, m)), 0) < 2]
        pool = safe if safe else moves
        if not pool:
            print("a1a1", flush=True)
            continue
        m = Search().run(s, pool)
        nk = key_of(apply_move(s, m))
        history[nk] = history.get(nk, 0) + 1
        print(name_of(m), flush=True)


if __name__ == "__main__":
    main()
