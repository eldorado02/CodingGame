#!/usr/bin/env python3
"""Othello 8x8 - deterministic bitboard engine (fixed depth 6).

Strict port of solutions/cpp/bot.cpp: same bit layout (bit 0 = a1, bit 7 = h1,
bit 8 = a2, bit 63 = h8), same move generation, same evaluation, same move
ordering and the same alpha-beta windows, so every chosen move matches.
"""
import sys

MASK = (1 << 64) - 1
FA = 0x0101010101010101  # file a
FH = 0x8080808080808080  # file h
NFA = MASK ^ FA
NFH = MASK ^ FH

POS = (
     120, -20,  20,   5,   5,  20, -20, 120,
     -20, -40,  -5,  -5,  -5,  -5, -40, -20,
      20,  -5,  15,   3,   3,  15,  -5,  20,
       5,  -5,   3,   3,   3,   3,  -5,   5,
       5,  -5,   3,   3,   3,   3,  -5,   5,
      20,  -5,  15,   3,   3,  15,  -5,  20,
     -20, -40,  -5,  -5,  -5,  -5, -40, -20,
     120, -20,  20,   5,   5,  20, -20, 120,
)

WIN = 1_000_000_000
INF = 2_000_000_000
DEPTH = 6


def shE(x):  return (x << 1) & MASK & NFA
def shO(x):  return (x >> 1) & MASK & NFH
def shN(x):  return (x << 8) & MASK
def shS(x):  return (x >> 8) & MASK
def shNE(x): return (x << 9) & MASK & NFA
def shNO(x): return (x << 7) & MASK & NFH
def shSE(x): return (x >> 7) & MASK & NFA
def shSO(x): return (x >> 9) & MASK & NFH


SHIFTS = (shE, shO, shN, shS, shNE, shNO, shSE, shSO)


def moves(P, O):
    """Legal destination squares for P (six dilation steps per direction)."""
    m = 0
    empty = MASK ^ (P | O)
    for sh in SHIFTS:
        t = sh(P) & O
        t |= sh(t) & O
        t |= sh(t) & O
        t |= sh(t) & O
        t |= sh(t) & O
        t |= sh(t) & O
        m |= sh(t) & empty
    return m


def flips(P, O, mv):
    """All discs flipped by placing a single stone at mv."""
    f = 0
    for sh in SHIFTS:
        s = sh(mv)
        t = 0
        while s & O:
            t |= s
            s = sh(s)
        if s & P:
            f |= t
    return f


def apply_move(P, O, mv):
    f = flips(P, O, mv)
    return P | mv | f, O & ~f


def neighbors(x):
    return shE(x) | shO(x) | shN(x) | shS(x) | shNE(x) | shNO(x) | shSE(x) | shSO(x)


def evaluate(P, O):
    pos = 0
    x = P
    while x:
        lb = x & -x
        pos += POS[lb.bit_length() - 1]
        x ^= lb
    x = O
    while x:
        lb = x & -x
        pos -= POS[lb.bit_length() - 1]
        x ^= lb
    empty = MASK ^ (P | O)
    nb = neighbors(empty)
    score = (pos
             + 20 * (moves(P, O).bit_count() - moves(O, P).bit_count())
             + 8 * ((O & nb).bit_count() - (P & nb).bit_count()))
    if empty.bit_count() <= 12:
        score += 8 * (P.bit_count() - O.bit_count())
    return score


def terminal(P, O):
    d = P.bit_count() - O.bit_count()
    return WIN if d > 0 else (-WIN if d < 0 else 0)


def ordered_bits(m):
    """Bits of m ascending, then stable insertion sort by POS descending."""
    out = []
    while m:
        lb = m & -m
        out.append(lb.bit_length() - 1)
        m ^= lb
    for i in range(1, len(out)):
        v = out[i]
        j = i - 1
        while j >= 0 and POS[out[j]] < POS[v]:
            out[j + 1] = out[j]
            j -= 1
        out[j + 1] = v
    return out


def search(P, O, depth, alpha, beta, passed):
    m = moves(P, O)
    if not m:
        if passed:
            return terminal(P, O)
        return -search(O, P, depth, -beta, -alpha, True)
    if depth <= 0:
        return evaluate(P, O)
    best = -INF
    for b in ordered_bits(m):
        nP, nO = apply_move(P, O, 1 << b)
        val = -search(nO, nP, depth - 1, -beta, -alpha, False)
        if val > best:
            best = val
        if best > alpha:
            alpha = best
        if alpha >= beta:
            break
    return best


def choose(P, O):
    m = moves(P, O)
    if not m:
        return -1
    bits = ordered_bits(m)
    best = bits[0]
    best_val = -INF
    for depth in range(1, DEPTH + 1):
        local_best = -INF
        local_move = best
        alpha = -INF
        for b in bits:
            nP, nO = apply_move(P, O, 1 << b)
            val = -search(nO, nP, depth - 1, -INF, -alpha, False)
            if val > local_best:
                local_best = val
                local_move = b
            if val > alpha:
                alpha = val
        best = local_move
        best_val = local_best
        if best_val >= WIN // 2:
            break
    return best


def parse_int(s):
    """Matches the C++ parse_int: optional surrounding blanks, digits only."""
    i = 0
    n = len(s)
    while i < n and s[i] == ' ':
        i += 1
    if i == n or not ('0' <= s[i] <= '9'):
        return None
    x = 0
    while i < n and '0' <= s[i] <= '9':
        x = x * 10 + (ord(s[i]) - 48)
        if x > 1000000:
            return None
        i += 1
    while i < n and s[i] == ' ':
        i += 1
    if i != n:
        return None
    return x


def is_board_row(s):
    if len(s) < 8:
        return False
    for c in range(8):
        if s[c] != '.' and s[c] != '0' and s[c] != '1':
            return False
    return True


def main():
    lines = iter(sys.stdin.read().split('\n'))

    # id / boardSize are only sent on the first three turns by the referee.
    line = next(lines, None)
    if line is None:
        return
    v = parse_int(line)
    ident = 0 if v is None else v
    line = next(lines, None)
    if line is None:
        return
    v = parse_int(line)
    size = 8 if v is None else v

    while True:
        P = 0
        O = 0
        rows = 0
        while rows < 8:
            line = next(lines, None)
            if line is None:
                return
            if not is_board_row(line):
                continue
            for c in range(8):
                bit = 1 << (rows * 8 + c)
                if line[c] == '0':
                    P |= bit
                elif line[c] == '1':
                    O |= bit
            rows += 1
        if ident == 1:
            P, O = O, P

        n = -1
        while True:
            line = next(lines, None)
            if line is None:
                return
            v = parse_int(line)
            if v is not None:
                n = v
                break
        if n < 0:
            break

        actions = []
        for _ in range(n):
            line = next(lines, None)
            if line is None:
                return
            k = 0
            while k < len(line) and line[k] == ' ':
                k += 1
            if k + 1 >= len(line):
                continue
            actions.append((ord(line[k]) - 97) + (ord(line[k + 1]) - 49) * 8)

        mv = choose(P, O)
        if not any(a == mv for a in actions) and actions:
            mv = actions[0]
        if mv < 0:
            out = 'PASS'
        else:
            out = chr(97 + mv % 8) + chr(49 + mv // 8)
        sys.stdout.write(out + '\n')
        sys.stdout.flush()


if __name__ == '__main__':
    main()
