#!/usr/bin/env python3
"""Dice Shogi 5x5 — Python reference, v1 greedy over the referee's legal actions.

Square (col, row) of an action maps to board[row - 1][5 - col] (col 1 = right
file, row 1 = top row; verified by a probe game). Player 0 = uppercase,
moves up; player 1 = lowercase, moves down. Promoted pieces are "+X" tokens.
Score = 10 * value(captured) + 4 if promotion - 10 * value(mover) if the
destination is attacked by the enemy (attack map of the current board)
- 1 for king moves. Ties: first action of the list.
DICE = True variant (dice-shogi) reads one extra line (rolled dice).
"""
import sys

DICE = True
VAL = {"K": 0, "R": 9, "B": 8, "G": 6, "S": 5, "P": 1, "+R": 11, "+B": 10, "+S": 6, "+P": 6}
ORTH = ((0, -1), (0, 1), (-1, 0), (1, 0))
DIAG = ((-1, -1), (1, -1), (-1, 1), (1, 1))
GOLD = ((0, -1), (-1, -1), (1, -1), (-1, 0), (1, 0), (0, 1))
SILVER = ((0, -1), (-1, -1), (1, -1), (-1, 1), (1, 1))
KING = ORTH + DIAG


def parse_row(s):
    out = []
    i = 0
    while i < len(s) and len(out) < 5:
        if s[i] == "+" and i + 1 < len(s):
            out.append(s[i:i + 2])
            i += 2
        else:
            out.append(s[i])
            i += 1
    while len(out) < 5:
        out.append(".")
    return out


def owner(tok):
    c = tok[-1]
    if c == ".":
        return -1
    return 0 if c.isupper() else 1


def kind(tok):
    return tok.upper() if tok != "." else "."


def attacks(board, side):
    """Set of (x, y) squares attacked by `side`."""
    att = set()
    fwd = -1 if side == 0 else 1
    for y in range(5):
        for x in range(5):
            t = board[y][x]
            if owner(t) != side:
                continue
            k = kind(t)
            steps, slides = (), ()
            if k == "K":
                steps = KING
            elif k in ("G", "+S", "+P"):
                steps = GOLD
            elif k == "S":
                steps = SILVER
            elif k == "P":
                steps = ((0, -1),)
            elif k == "R":
                slides = ORTH
            elif k == "B":
                slides = DIAG
            elif k == "+R":
                slides, steps = ORTH, DIAG
            elif k == "+B":
                slides, steps = DIAG, ORTH
            for dx, dy in steps:
                nx, ny = x + dx, y + dy * fwd
                if 0 <= nx < 5 and 0 <= ny < 5:
                    att.add((nx, ny))
            for dx, dy in slides:
                nx, ny = x + dx, y + dy
                while 0 <= nx < 5 and 0 <= ny < 5:
                    att.add((nx, ny))
                    if board[ny][nx] != ".":
                        break
                    nx += dx
                    ny += dy
    return att


def sq(c, r):
    """Action digits -> (x, y) or None."""
    if not ("1" <= c <= "5" and "1" <= r <= "5"):
        return None
    return (5 - int(c), int(r) - 1)


def score(board, me, enemy_att, act):
    a = act.strip()
    if len(a) >= 4 and a[1] == "*":
        dst = sq(a[2], a[3])
        if dst is None:
            return None
        v = VAL.get(a[0].upper(), 0)
        return -10 * v if dst in enemy_att else 0
    if len(a) < 4:
        return None
    src, dst = sq(a[0], a[1]), sq(a[2], a[3])
    if src is None or dst is None:
        return None
    mover = board[src[1]][src[0]]
    target = board[dst[1]][dst[0]]
    sc = 0
    if target != "." and owner(target) != me:
        sc += 10 * VAL.get(kind(target), 0)
    if a.endswith("+"):
        sc += 4
    k = kind(mover)
    if k == "K":
        sc -= 1
    elif dst in enemy_att:
        sc -= 10 * VAL.get(k, 0)
    return sc


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = 1 if line.strip() == "1" else 0
    size_line = sys.stdin.readline()
    n = int(size_line) if size_line.strip().isdigit() else 5
    while True:
        board = []
        for _ in range(n):
            r = sys.stdin.readline()
            if not r:
                return
            board.append(parse_row(r.strip()))
        for _ in range(3 + (1 if DICE else 0)):
            if not sys.stdin.readline():
                return
        c_line = sys.stdin.readline()
        if not c_line:
            return
        enemy_att = attacks(board, 1 - me)
        best, best_v = None, None
        for _ in range(int(c_line)):
            t = sys.stdin.readline().split()
            if not t:
                continue
            v = score(board, me, enemy_att, t[0])
            if v is not None and (best_v is None or v > best_v):
                best, best_v = t[0], v
        print(best if best else "random", flush=True)


if __name__ == "__main__":
    main()
