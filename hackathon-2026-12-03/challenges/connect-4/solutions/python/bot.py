#!/usr/bin/env python3
"""Connect 4 (7x9): alpha-beta depth 6, fenetre de 4 cases, STEAL."""
import sys

R, C = 7, 9
WIN = 1000000
DR = (0, 1, 1, 1)
DC = (1, 0, 1, -1)
MASK = (1 << 64) - 1
CONST = 0x9E3779B97F4A7C15


def wins_at(cell, r, c, p):
    for d in range(4):
        cnt = 1
        for s in range(1, 4):
            nr, nc = r + DR[d] * s, c + DC[d] * s
            if not (0 <= nr < R and 0 <= nc < C) or cell[nr * C + nc] != p:
                break
            cnt += 1
        for s in range(1, 4):
            nr, nc = r - DR[d] * s, c - DC[d] * s
            if not (0 <= nr < R and 0 <= nc < C) or cell[nr * C + nc] != p:
                break
            cnt += 1
        if cnt >= 4:
            return True
    return False


def play(cell, h, col, p):
    r = R - 1 - h[col]
    cell[r * C + col] = p
    h[col] += 1
    return wins_at(cell, r, col, p)


def evaluate(cell, p):
    opp = 3 - p
    score = 0
    for r in range(R):
        for c in range(C):
            for d in range(4):
                ar, ac = r - DR[d] * 3, c - DC[d] * 3
                if ar < 0 or ac < 0 or ac >= C:
                    continue
                mine = theirs = 0
                ok = True
                for s in range(4):
                    rr, cc = ar + DR[d] * s, ac + DC[d] * s
                    if rr >= R or cc >= C:
                        ok = False
                        break
                    v = cell[rr * C + cc]
                    if v == p:
                        mine += 1
                    elif v == opp:
                        theirs += 1
                if not ok:
                    continue
                if mine and not theirs:
                    score += 1 << (mine - 1)
                if theirs and not mine:
                    score -= 1 << (theirs - 1)
    top_center = cell[4]
    score += 2 * (top_center == p) - 2 * (top_center == opp)
    return score


def key(cell, p):
    h = 1469598103934665603
    for v in cell:
        h = (h ^ ((v + CONST + (h << 6) + (h >> 2)) & MASK)) & MASK
    return h ^ p


TT = {}


def search(cell, h, p, depth, alpha, beta, ply):
    if depth <= 0:
        return evaluate(cell, p)
    best = -2 * WIN
    best_move = -1
    k = key(cell, p)
    ent = TT.get(k)
    tt_move = -1
    if ent is not None and ent[0] >= depth:
        d, v, flag, mv = ent
        if flag == 0:
            return v
        if flag == 1 and v >= beta:
            return v
        if flag == 2 and v <= alpha:
            return v
        tt_move = mv
    moves = []
    if tt_move >= 0 and h[tt_move] < R:
        moves.append(tt_move)
    c, k2 = 4, 0
    while k2 < C:
        if h[c] < R and c not in moves:
            moves.append(c)
        k2 += 1
        c = (c + (k2 if k2 % 2 else -k2) + C) % C
    full = True
    for c in moves:
        ncell = cell[:]
        nh = h[:]
        win = play(ncell, nh, c, p)
        if win:
            val = WIN - ply
        elif all(x >= R for x in nh):
            val = 0
        else:
            val = -search(ncell, nh, 3 - p, depth - 1, -beta, -alpha, ply + 1)
        if val > best:
            best, best_move = val, c
        if best > alpha:
            alpha = best
        if alpha >= beta:
            full = False
            break
    TT[k] = (depth, best, 0 if full else (1 if best >= beta else 2), best_move)
    return best


def choose(cell, h, valid, p):
    best_col = valid[0] if valid else 4
    best_val = -2 * WIN
    for depth in range(1, 7):
        alpha = -2 * WIN
        local_best, local_val = -1, -2 * WIN
        for c in valid:
            ncell = cell[:]
            nh = h[:]
            win = play(ncell, nh, c, p)
            if win:
                val = WIN
            elif depth == 1:
                val = evaluate(ncell, p)
            else:
                val = -search(ncell, nh, 3 - p, depth - 1, -2 * WIN, -alpha, 1)
            if val > local_val:
                local_val, local_best = val, c
            if val > alpha:
                alpha = val
        best_col, best_val = local_best, local_val
        if best_val >= WIN // 2:
            break
    return best_col


def main():
    init = sys.stdin.readline().split()
    if len(init) < 2:
        return
    me = int(init[0]) + 1
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        if not line.strip():
            continue
        int(line)  # turnIndex
        cell = [0] * (R * C)
        for r in range(R):
            row = sys.stdin.readline().strip()
            for c in range(C):
                cell[r * C + c] = 0 if row[c] == '.' else int(row[c]) + 1
        h = [sum(1 for r in range(R) if cell[r * C + c]) for c in range(C)]
        n = int(sys.stdin.readline())
        valid = [int(sys.stdin.readline()) for _ in range(n)]
        sys.stdin.readline()  # oppPreviousAction
        if -2 in valid:
            print('STEAL', flush=True)
        else:
            print(choose(cell, h, valid, me), flush=True)


if __name__ == '__main__':
    main()
