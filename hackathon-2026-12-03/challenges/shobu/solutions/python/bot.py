#!/usr/bin/env python3
"""Shobu — minimax déterministe à budget d'évaluations (référence de fuzz)."""
import sys

WIN = 1_000_000_000
ROOT_BEAM = 24
NODE_BEAM = 16
WORK_LIMIT = 20000
DIRS = ((0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1))


def board_of(x, y):
    return 2 * (y // 4) + x // 4


def is_dark(b):
    return b % 2 == 0


def is_home(b, color):
    return b >= 2 if color == 0 else b < 2


def same_board(x, y, ux, uy, k):
    nx, ny = x + ux * k, y + uy * k
    return 0 <= nx < 8 and 0 <= ny < 8 and nx // 4 == x // 4 and ny // 4 == y // 4


def gen_moves(c, me):
    out = []
    op = 1 - me
    for py in range(8):
        for px in range(8):
            if c[py * 8 + px] != me or not is_home(board_of(px, py), me):
                continue
            passive_dark = is_dark(board_of(px, py))
            for ux, uy in DIRS:
                for k in (1, 2):
                    if not same_board(px, py, ux, uy, k) or c[(py + uy * k) * 8 + px + ux * k] != -1:
                        break
                    for ay in range(8):
                        for ax in range(8):
                            if c[ay * 8 + ax] != me or is_dark(board_of(ax, ay)) == passive_dark:
                                continue
                            if not same_board(ax, ay, ux, uy, k):
                                continue
                            n = 0
                            ok = True
                            for j in range(1, k + 1):
                                v = c[(ay + uy * j) * 8 + ax + ux * j]
                                if v == me:
                                    ok = False
                                    break
                                if v == op:
                                    n += 1
                            if not ok or n > 1:
                                continue
                            if n == 1 and same_board(ax, ay, ux, uy, k + 1):
                                if c[(ay + uy * (k + 1)) * 8 + ax + ux * (k + 1)] != -1:
                                    continue
                            out.append((px, py, ax, ay, ux * k, uy * k))
    return out


def apply_step(cells, me, m):
    px, py, ax, ay, dx, dy = m
    g = cells[:]
    op = 1 - me
    k = max(abs(dx), abs(dy))
    ux = (dx > 0) - (dx < 0)
    uy = (dy > 0) - (dy < 0)
    g[py * 8 + px] = -1
    g[(py + dy) * 8 + px + dx] = me
    for j in range(1, k + 1):
        x, y = ax + ux * j, ay + uy * j
        if g[y * 8 + x] == op:
            g[y * 8 + x] = -1
            tx, ty = ax + ux * (k + 1), ay + uy * (k + 1)
            if same_board(ax, ay, ux, uy, k + 1):
                g[ty * 8 + tx] = op
            break
    g[ay * 8 + ax] = -1
    g[(ay + dy) * 8 + ax + dx] = me
    win = False
    for b in range(4):
        bx, by = (b % 2) * 4, (b // 2) * 4
        n = 0
        for y in range(by, by + 4):
            for x in range(bx, bx + 4):
                if g[y * 8 + x] == op:
                    n += 1
        if n == 0:
            win = True
            break
    return g, win


def apply_move(cells, me, m):
    """Alias historique utilisé par le générateur de fuzz."""
    return apply_step(cells, me, m)[0]


def stones(g, b, color):
    bx, by = (b % 2) * 4, (b // 2) * 4
    return sum(1 for y in range(by, by + 4) for x in range(bx, bx + 4) if g[y * 8 + x] == color)


def evaluate(g, me):
    op = 1 - me
    om = [stones(g, b, op) for b in range(4)]
    mm = [stones(g, b, me) for b in range(4)]
    mn_o, mn_m = min(om), min(mm)
    if mn_o == 0:
        return WIN
    if mn_m == 0:
        return -WIN
    score = 35 * (sum(mm) - sum(om))
    score += 420 * (4 - mn_o) * (4 - mn_o)
    score -= 320 * (4 - mn_m) * (4 - mn_m)
    for b in range(4):
        score += 18 * (mm[b] - om[b])
    our_edge = op_edge = our_corner = op_corner = 0
    for y in range(8):
        for x in range(8):
            c = g[y * 8 + x]
            if c == -1:
                continue
            lx, ly = x % 4, y % 4
            edge = lx == 0 or lx == 3 or ly == 0 or ly == 3
            corner = (lx == 0 or lx == 3) and (ly == 0 or ly == 3)
            if c == me:
                our_edge += edge
                our_corner += corner
            else:
                op_edge += edge
                op_corner += corner
    score += 30 * (op_edge - our_edge) + 45 * (op_corner - our_corner)
    return score


class Budget:
    __slots__ = ('work', 'aborted')

    def __init__(self):
        self.work = 0
        self.aborted = False


def ordered_moves(g, me, beam, budget):
    scored = []
    for m in gen_moves(g, me):
        if budget.work >= WORK_LIMIT:
            budget.aborted = True
            break
        budget.work += 1
        ng, win = apply_step(g, me, m)
        scored.append(((WIN if win else evaluate(ng, me)), m, ng, win))
    scored.sort(key=lambda x: (-x[0], x[1]))
    return scored[:beam]


def search(g, me, depth, alpha, beta, budget):
    if budget.aborted:
        return evaluate(g, me)
    if depth <= 0:
        return evaluate(g, me)
    moves = ordered_moves(g, me, NODE_BEAM, budget)
    if not moves:
        return -WIN
    best = -2 * WIN
    for score, m, ng, win in moves:
        val = WIN - depth if win else -search(ng, 1 - me, depth - 1, -beta, -alpha, budget)
        if val > best:
            best = val
        if best > alpha:
            alpha = best
        if alpha >= beta or budget.aborted:
            break
    return best


def choose(g, me):
    all_moves = gen_moves(g, me)
    if not all_moves:
        return (0, 0, 0, 0, 0, 0)
    best = all_moves[0]
    best_val = -2 * WIN
    budget = Budget()
    for depth in range(1, 5):
        moves = ordered_moves(g, me, ROOT_BEAM, budget)
        local_best, local_move = -2 * WIN, best
        aborted = budget.aborted
        for score, m, ng, win in moves:
            if budget.aborted:
                aborted = True
                break
            val = (WIN - depth if win else
                   -search(ng, 1 - me, depth - 1, -2 * WIN,
                           2 * WIN if local_best == -2 * WIN else -local_best, budget))
            if val > local_best:
                local_best, local_move = val, m
        if not aborted:
            best, best_val = local_move, local_best
        if best_val >= WIN // 2 or aborted:
            break
    return best if best in all_moves else all_moves[0]


def main():
    line = sys.stdin.readline()
    if not line:
        return
    round_no, my_color = map(int, line.split())
    out = []
    while True:
        cells = []
        for _ in range(8):
            row = sys.stdin.readline().strip()
            if not row:
                return
            cells.extend(0 if ch == 'b' else 1 if ch == 'w' else -1 for ch in row)
        sys.stdin.readline()  # action adverse (6 entiers, '-' ou 'SWAP')
        n = int(sys.stdin.readline())
        actions = []
        can_swap = False
        for _ in range(n):
            a = sys.stdin.readline().split()
            if a[0] == 'SWAP':
                can_swap = True
            else:
                actions.append(tuple(map(int, a)))
        if can_swap:
            out.append('SWAP')
        else:
            out.append(' '.join(map(str, choose(cells, my_color))))
        sys.stdout.write(out[-1] + '\n')
        sys.stdout.flush()
        out.clear()
        line = sys.stdin.readline()
        if not line:
            return
        round_no, my_color = map(int, line.split())


if __name__ == '__main__':
    main()
