#!/usr/bin/env python3
"""Abalone — Python reference of the differential fuzz, v1 one-ply greedy on legal moves.

Board (probe 2026-11-26): 9 rows of 5,6,7,8,9,8,7,6,5 cells, `x` = index in
the row, `y` = row. Axial coordinates: r = y - 4, q = x + max(-4, -r - 4).
Directions (from the legal moves of the probe): 0 E (+1,0), 1 NE (+1,-1),
2 NW (0,-1), 3 W (-1,0), 4 SW (-1,+1), 5 SE (0,+1).
A move "fx fy lx ly d" moves the line first..last by d: in line (d along the
line or a single marble) the marbles in front are pushed (Sumito), the last
one may fall off; otherwise every marble side-steps.
Score = CAPTURE_WEIGHT * marbles pushed off - CENTER_WEIGHT * our distances
to the centre + EDGE_WEIGHT * their distances + COHESION_WEIGHT * our
neighbouring pairs. Ties: first legal move.
"""
import sys

RADIUS = 4
ROWS = 2 * RADIUS + 1
DIRECTIONS = ((1, 0), (1, -1), (0, -1), (-1, 0), (-1, 1), (0, 1))
CAPTURE_WEIGHT = 10000
CENTER_WEIGHT = 10
EDGE_WEIGHT = 5
COHESION_WEIGHT = 2
EMPTY = "0"


def q_min(r):
    return max(-RADIUS, -r - RADIUS)


def to_axial(x, y):
    r = y - RADIUS
    return x + q_min(r), r


def on_board(q, r):
    return -RADIUS <= r <= RADIUS and q_min(r) <= q <= min(RADIUS, -r + RADIUS)


def distance(q, r):
    return (abs(q) + abs(r) + abs(q + r)) // 2


def sign(v):
    return (v > 0) - (v < 0)


def play(board, me, move):
    """Board after our move and the number of enemy marbles pushed off."""
    fx, fy, lx, ly, d = move
    first, last = to_axial(fx, fy), to_axial(lx, ly)
    steps = distance(last[0] - first[0], last[1] - first[1])
    axis = (sign(last[0] - first[0]), sign(last[1] - first[1]))
    line = [c for c in ((first[0] + k * axis[0], first[1] + k * axis[1]) for k in range(steps + 1)) if on_board(*c)]
    dq, dr = DIRECTIONS[d % len(DIRECTIONS)]
    after = dict(board)
    captured = 0
    inline = steps == 0 or (dq, dr) in (axis, (-axis[0], -axis[1]))
    if not line:
        return after, 0
    if inline:
        head = max(line, key=lambda c: c[0] * dq + c[1] * dr)
        chain = []
        cursor = (head[0] + dq, head[1] + dr)
        while on_board(*cursor) and board[cursor] not in (EMPTY, me):
            chain.append(cursor)
            cursor = (cursor[0] + dq, cursor[1] + dr)
        for cell in reversed(chain):
            target = (cell[0] + dq, cell[1] + dr)
            if on_board(*target):
                after[target] = board[cell]
            else:
                captured += 1
            after[cell] = EMPTY
    for cell in line:
        after[cell] = EMPTY
    for cell in line:
        target = (cell[0] + dq, cell[1] + dr)
        if on_board(*target):
            after[target] = me
    return after, captured


def evaluate(board, me):
    score = 0
    for (q, r), v in board.items():
        if v == me:
            score -= CENTER_WEIGHT * distance(q, r)
            for dq, dr in DIRECTIONS:
                if board.get((q + dq, r + dr)) == me:
                    score += COHESION_WEIGHT
        elif v != EMPTY:
            score += EDGE_WEIGHT * distance(q, r)
    return score


def choose(board, me, moves):
    best, best_score = None, None
    for move in moves:
        after, captured = play(board, me, move)
        score = CAPTURE_WEIGHT * captured + evaluate(after, me)
        if best_score is None or score > best_score:
            best, best_score = move, score
    return best


def read_board(rows):
    board = {}
    for y in range(ROWS):
        r = y - RADIUS
        width = min(RADIUS, -r + RADIUS) - q_min(r) + 1
        text = rows[y] if y < len(rows) else ""
        for x in range(width):
            board[to_axial(x, y)] = text[x] if x < len(text) else EMPTY
    return board


def main():
    read = sys.stdin.readline
    first = read()
    if not first:
        return
    me = first.strip()
    while True:
        if not read():
            return
        rows = [read().strip() for _ in range(ROWS)]
        read()      # opponent's last move
        moves = [tuple(int(v) for v in read().split()[:5]) for _ in range(int(read()))]
        best = choose(read_board(rows), me, moves)
        print(" ".join(map(str, best)) if best else "0 0 0 0 0", flush=True)


if __name__ == "__main__":
    main()
