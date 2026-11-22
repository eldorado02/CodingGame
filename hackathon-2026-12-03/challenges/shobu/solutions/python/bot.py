#!/usr/bin/env python3
"""Shobu — évaluation 1 ply + meilleure réponse adverse (beam)."""
import sys

WIN = 10**9
DIRS = ((0, -1), (1, -1), (1, 0), (1, 1),
        (0, 1), (-1, 1), (-1, 0), (-1, -1))
BEAM = 24


def board_of(x, y):
    return 2 * (y // 4) + x // 4


def is_dark(b):
    return b % 2 == 0


def is_home(b, color):
    return b >= 2 if color == 0 else b < 2


def same_board(x, y, ux, uy, k):
    nx, ny = x + ux * k, y + uy * k
    return 0 <= nx < 8 and 0 <= ny < 8 and nx // 4 == x // 4 and ny // 4 == y // 4


def gen_moves(cells, me):
    """Identique à Board.legalMoves() du referee, ordre de génération compris."""
    out = []
    op = 1 - me
    for py in range(8):
        for px in range(8):
            if cells[py * 8 + px] != me or not is_home(board_of(px, py), me):
                continue
            passive_dark = is_dark(board_of(px, py))
            for ux, uy in DIRS:
                for k in (1, 2):
                    if not same_board(px, py, ux, uy, k) or cells[(py + uy * k) * 8 + px + ux * k] != -1:
                        break
                    for ay in range(8):
                        for ax in range(8):
                            if cells[ay * 8 + ax] != me or is_dark(board_of(ax, ay)) == passive_dark:
                                continue
                            if not same_board(ax, ay, ux, uy, k):
                                continue
                            n = 0
                            ok = True
                            for j in range(1, k + 1):
                                v = cells[(ay + uy * j) * 8 + ax + ux * j]
                                if v == me:
                                    ok = False
                                    break
                                if v == op:
                                    n += 1
                            if not ok or n > 1:
                                continue
                            if n == 1 and same_board(ax, ay, ux, uy, k + 1):
                                if cells[(ay + uy * (k + 1)) * 8 + ax + ux * (k + 1)] != -1:
                                    continue
                            out.append((px, py, ax, ay, ux * k, uy * k))
    return out


def apply_move(cells, me, m):
    px, py, ax, ay, dx, dy = m
    g = cells[:]
    op = 1 - me
    k = max(abs(dx), abs(dy))
    ux = (dx > 0) - (dx < 0)
    uy = (dy > 0) - (dy < 0)
    g[py * 8 + px] = -1
    g[(py + dy) * 8 + px + dx] = me
    removed = False
    for j in range(1, k + 1):
        x, y = ax + ux * j, ay + uy * j
        if g[y * 8 + x] == op:
            g[y * 8 + x] = -1
            tx, ty = ax + ux * (k + 1), ay + uy * (k + 1)
            if same_board(ax, ay, ux, uy, k + 1):
                g[ty * 8 + tx] = op
            else:
                removed = True
            break
    g[ay * 8 + ax] = -1
    g[(ay + dy) * 8 + ax + dx] = me
    return g


def stones(g, board, color):
    bx, by = (board % 2) * 4, (board // 2) * 4
    return sum(1 for y in range(by, by + 4) for x in range(bx, bx + 4)
               if g[y * 8 + x] == color)


def evaluate(g, me):
    op = 1 - me
    om = [stones(g, b, op) for b in range(4)]
    mm = [stones(g, b, me) for b in range(4)]
    if min(om) == 0:
        return WIN
    if min(mm) == 0:
        return -WIN
    score = 40 * (sum(mm) - sum(om))
    score += 300 * ((4 - min(om)) ** 2)
    score -= 250 * ((4 - min(mm)) ** 2)
    score += 15 * sum(mm[b] - om[b] for b in range(4))
    # pierres sur le bord d'un board : exposées pour nous, cibles pour l'adversaire
    our_edge = op_edge = 0
    for y in range(8):
        for x in range(8):
            c = g[y * 8 + x]
            if c == -1:
                continue
            lx, ly = x % 4, y % 4
            edge = (lx == 0 or lx == 3 or ly == 0 or ly == 3)
            if edge:
                if c == me:
                    our_edge += 1
                else:
                    op_edge += 1
    score += 25 * (op_edge - our_edge)
    return score


def choose(g, me):
    moves = gen_moves(g, me)
    if not moves:
        return None
    scored = []
    for m in moves:
        ng = apply_move(g, me, m)
        s = evaluate(ng, me)
        scored.append((s, m, ng))
    scored.sort(key=lambda x: (-x[0], x[1]))
    best = None
    best_score = -2 * WIN
    for s, m, ng in scored[:BEAM]:
        if s >= WIN:
            return m
        replies = gen_moves(ng, 1 - me)
        if not replies:
            return m
        worst = WIN
        for r in replies[:BEAM]:
            v = evaluate(apply_move(ng, 1 - me, r), me)
            if v < worst:
                worst = v
        if worst > best_score or (worst == best_score and m < best):
            best_score, best = worst, m
    return best


def main():
    out = []
    while True:
        try:
            header = input().split()
        except EOFError:
            break
        if not header:
            continue
        round_no, my_color = int(header[0]), int(header[1])
        cells = []
        for _ in range(8):
            for ch in input().strip():
                cells.append(0 if ch == 'b' else 1 if ch == 'w' else -1)
        last = input().split()
        n = int(input())
        actions = []
        can_swap = False
        for _ in range(n):
            a = input().split()
            if a[0] == 'SWAP':
                can_swap = True
            else:
                actions.append(tuple(map(int, a[:6])))
        can_swap = can_swap and my_color == 1
        if can_swap:
            out.append('SWAP')
        else:
            m = choose(cells, my_color)
            if m is None or m not in actions:
                m = actions[0] if actions else (0, 0, 1, 1, 0, 1)
            out.append(' '.join(map(str, m)))
        print(out[-1], flush=True)


if __name__ == '__main__':
    main()
