#!/usr/bin/env python3
"""Impasse — Python reference, v1 greedy over the referee's legal moves.

Board index = y * 8 + x, y = 0 is row 8 (first input line), x = 0 = column a.
White singles slide up (towards row 8 = furthest row, where they get crowned),
white doubles slide down (towards row 1 = nearest row, where they bear off).
Black is mirrored. Each legal move string (1, 2 or 3 coordinates) is scored:
  impasse removal 50 | double reaching nearest row 25 | transpose 12 (+20 if
  the receiving single is in the nearest row) | double slide 3/row |
  single slide 3/row (+8 if it reaches the furthest row) | crown coord +2.
Ties: first move of the list (strict >).
"""
import sys


def parse_coords(txt):
    out = []
    if len(txt) % 2:
        return None
    for i in range(0, len(txt), 2):
        c, r = txt[i], txt[i + 1]
        if not ("a" <= c <= "h" and "1" <= r <= "8"):
            return None
        out.append((ord(c) - 97, ord(r) - 48))  # (col, row 1..8)
    return out or None


def score(board, me, coords):
    if len(coords) == 1:
        return 50
    up = me == "w"
    near = 1 if up else 8
    far = 8 if up else 1
    (c1, r1), (c2, r2) = coords[0], coords[1]
    src = board[8 - r1][c1]
    dst = board[8 - r2][c2]
    sc = 0
    rows = abs(r2 - r1)
    if src == me.upper():
        if dst == me:
            sc += 12 + (20 if r2 == near else 0)
        else:
            sc += 3 * rows + (25 if r2 == near else 0)
    else:
        sc += 3 * rows + (8 if r2 == far else 0)
    if len(coords) >= 3:
        sc += 2
    return sc


def main():
    line = sys.stdin.readline()
    if not line:
        return
    me = "b" if line.strip().lower().startswith("b") else "w"
    while True:
        board = []
        for _ in range(8):
            r = sys.stdin.readline()
            if not r:
                return
            board.append((r.rstrip("\r\n") + "........")[:8])
        if not sys.stdin.readline():
            return
        n_line = sys.stdin.readline()
        if not n_line:
            return
        best, best_v = None, -1
        for _ in range(int(n_line)):
            t = sys.stdin.readline().split()
            if not t:
                continue
            co = parse_coords(t[0])
            if co is None:
                continue
            v = score(board, me, co)
            if v > best_v:
                best, best_v = t[0], v
        print(best if best else "random", flush=True)


if __name__ == "__main__":
    main()
