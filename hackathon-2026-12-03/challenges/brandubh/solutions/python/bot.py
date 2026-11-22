#!/usr/bin/env python3
"""Brandubh — alpha-beta à profondeur fixe avec évaluation d'évasion du roi."""
import sys
from collections import deque

WIN = 10**9
SIZE = 7
CORNERS = ((0, 0), (0, 6), (6, 0), (6, 6))
DIRS = ((1, 0), (-1, 0), (0, 1), (0, -1))
EMPTY, ATT, DEF, KING = 0, 1, 2, 3


def index(x, y):
    return y * SIZE + x


def is_corner(x, y):
    return (x, y) in CORNERS


def is_throne(x, y):
    return x == 3 and y == 3


def initial():
    g = [EMPTY] * 49
    # y=0 correspond a la premiere ligne d'entree (rangee 7).
    for x, y in ((3, 6), (3, 5), (0, 3), (1, 3), (6, 3), (5, 3), (3, 1), (3, 0)):
        g[index(x, y)] = ATT
    for x, y in ((3, 4), (2, 3), (4, 3), (3, 2)):
        g[index(x, y)] = DEF
    g[index(3, 3)] = KING
    return g


def hostile(g, x, y, capturing_type, capturing_king):
    """Referee.isHostileSquare : coins hostiles, trône conditionnel."""
    if x < 0 or x >= SIZE or y < 0 or y >= SIZE:
        return False
    if is_corner(x, y):
        return True
    target_is_attacker = capturing_type != ATT
    if is_throne(x, y):
        occupant = g[index(x, y)]
        if occupant == EMPTY:
            return not capturing_king
        if target_is_attacker:
            return True
        return False
    p = g[index(x, y)]
    if p != EMPTY:
        if target_is_attacker:
            return p != ATT
        return p == ATT
    return False


def apply_move(g, move):
    """move = (sx, sy, ex, ey). Retourne (nouveau_plateau, roi_capturé)."""
    sx, sy, ex, ey = move
    ng = g[:]
    piece = ng[index(sx, sy)]
    ng[index(sx, sy)] = EMPTY
    ng[index(ex, ey)] = piece
    for ux, uy in DIRS:
        ax, ay = ex + ux, ey + uy
        if not (0 <= ax < SIZE and 0 <= ay < SIZE):
            continue
        adj = ng[index(ax, ay)]
        if adj == EMPTY or (piece == ATT and adj == ATT) or (piece != ATT and adj != ATT):
            continue
        if adj == KING and ax == 3 and ay == 3:
            if all(hostile(ng, x, y, ATT, True) for x, y in
                   ((ax + 1, ay), (ax - 1, ay), (ax, ay + 1), (ax, ay - 1))):
                return ng, True
            continue
        bx, by = ax + ux, ay + uy
        if hostile(ng, bx, by, piece, adj == KING):
            ng[index(ax, ay)] = EMPTY
    return ng, False


def gen_moves(g, player):
    out = []
    for sy in range(SIZE):
        for sx in range(SIZE):
            p = g[index(sx, sy)]
            if p == EMPTY:
                continue
            is_att = p == ATT
            if p == KING:
                if player != 1:
                    continue
            elif is_att != (player == 0):
                continue
            for ux, uy in DIRS:
                x, y = sx, sy
                while True:
                    x += ux
                    y += uy
                    if x < 0 or x >= SIZE or y < 0 or y >= SIZE:
                        break
                    if g[index(x, y)] != EMPTY:
                        break
                    if (is_throne(x, y) or is_corner(x, y)) and p != KING:
                        continue
                    out.append((sx, sy, x, y))
    return out


def king_pos(g):
    for i, v in enumerate(g):
        if v == KING:
            return i % SIZE, i // SIZE
    return None


def escape_dist(g, kx, ky):
    """Distance BFS du roi à un coin ; toutes les pièces sont des murs."""
    q = deque([(kx, ky, 0)])
    seen = {(kx, ky)}
    while q:
        x, y, d = q.popleft()
        if is_corner(x, y):
            return d
        for ux, uy in DIRS:
            nx, ny = x + ux, y + uy
            if not (0 <= nx < SIZE and 0 <= ny < SIZE) or (nx, ny) in seen:
                continue
            if g[index(nx, ny)] == ATT:
                continue
            seen.add((nx, ny))
            q.append((nx, ny, d + 1))
    return 14


def king_mobility(g, kx, ky):
    n = 0
    for ux, uy in DIRS:
        x, y = kx, ky
        while True:
            x += ux
            y += uy
            if not (0 <= x < SIZE and 0 <= y < SIZE) or g[index(x, y)] != EMPTY:
                break
            n += 1
            if is_corner(x, y):
                break
    return n


def encircled(g):
    """Referee.checkEncirclement : bassin ouvert depuis n'importe quel bord."""
    seen = [[False] * SIZE for _ in range(SIZE)]
    q = deque()
    for x in range(SIZE):
        for y in range(SIZE):
            if x not in (0, SIZE - 1) and y not in (0, SIZE - 1):
                continue
            p = g[index(x, y)]
            if p in (KING, DEF):
                return False
            if p == EMPTY:
                seen[y][x] = True
                q.append((x, y))
    while q:
        x, y = q.popleft()
        for ux, uy in DIRS:
            nx, ny = x + ux, y + uy
            if 0 <= nx < SIZE and 0 <= ny < SIZE and not seen[ny][nx]:
                p = g[index(nx, ny)]
                if p in (KING, DEF):
                    return False
                if p != ATT:
                    seen[ny][nx] = True
                    q.append((nx, ny))
    return True


def evaluate(g, player):
    kp = king_pos(g)
    if kp is None:
        return WIN if player == 0 else -WIN
    kx, ky = kp
    if is_corner(kx, ky):
        return -WIN if player == 0 else WIN
    if encircled(g):
        return WIN if player == 0 else -WIN
    dist = escape_dist(g, kx, ky)
    mob = king_mobility(g, kx, ky)
    atts = g.count(ATT)
    defs = g.count(DEF)
    adj = 0
    for ux, uy in DIRS:
        x, y = kx + ux, ky + uy
        if 0 <= x < SIZE and 0 <= y < SIZE and g[index(x, y)] == ATT:
            adj += 1
    if player == 0:
        return (-120 * dist - 8 * mob + 50 * adj + 300 * (atts - defs))
    return (120 * dist + 8 * mob - 50 * adj + 300 * (defs - atts))


def search(g, player, depth, alpha, beta):
    moves = gen_moves(g, player)
    if not moves:
        return -WIN if player == 0 else WIN
    if depth == 0:
        return evaluate(g, player)
    best = -2 * WIN if player == 0 else 2 * WIN
    for m in moves:
        ng, king_dead = apply_move(g, m)
        if king_dead:
            val = WIN if player == 0 else -WIN
        else:
            val = search(ng, 1 - player, depth - 1, alpha, beta)
        if player == 0:
            if val > best:
                best = val
            if best > alpha:
                alpha = best
        else:
            if val < best:
                best = val
            if best < beta:
                beta = best
        if beta <= alpha:
            break
    return best


def choose(g, role, depth=None):
    if depth is None:
        depth = 2 if role == 0 else 1
    moves = gen_moves(g, role)
    if not moves:
        return None
    best = None
    best_val = -2 * WIN
    alpha = -2 * WIN
    beta = 2 * WIN
    for m in moves:
        ng, king_dead = apply_move(g, m)
        val = WIN if king_dead else search(ng, 1 - role, depth - 1, alpha, beta)
        # Pas de departage sur valeur bornee : seul un gain strict est fiable.
        if val > best_val:
            best_val, best = val, m
            if best_val > alpha:
                alpha = best_val
    return best


def main():
    while True:
        try:
            line = input()
        except EOFError:
            break
        role = int(line.strip())
        _last = input()
        g = []
        for _ in range(SIZE):
            row = input().strip()
            for ch in row:
                g.append(ATT if ch == 'A' else DEF if ch == 'D' else KING if ch == 'K' else EMPTY)
        m = choose(g, role)
        if m is None:
            print('a1a1', flush=True)
            continue
        sx, sy, ex, ey = m
        print('%c%c%c%c' % (chr(ord('a') + sx), ord('7') - sy,
                            chr(ord('a') + ex), ord('7') - ey), flush=True)


if __name__ == '__main__':
    main()
