#!/usr/bin/env python3
"""Penguins (Hey, That's my fish!) — Python reference, v2 greedy + mobility.

POS = letter (column) + number (row, row 1 = first board line). Rows 1, 3,
... are the narrow (shifted) rows. Hex neighbours (probe-verified on the
legal-move lists): from a narrow row, "right" diagonals keep x+1 and "left"
keep x; from a wide row, right keeps x and left x-1 (up and down alike).
Leaving a block eats its fish. Action value = SOURCE_WEIGHT * fish(source)
+ fish(destination) + MOBILITY_WEIGHT * moves available from the
destination after the move. Placements (no source) skip the source term.
"""
import sys

SOURCE_WEIGHT = 10     # fish eaten now outweigh fish reachable later
MOBILITY_WEIGHT = 1    # a penguin with few rays left gets trapped


def step(r, x, d):
    """Neighbour of (r, x) in direction d (0 E, 1 W, 2 NE, 3 NW, 4 SE, 5 SW)."""
    narrow = r % 2 == 0
    if d == 0:
        return r, x + 1
    if d == 1:
        return r, x - 1
    dr = 1 if d in (2, 3) else -1
    right = d in (2, 4)
    if narrow:
        return r + dr, x + 1 if right else x
    return r + dr, x if right else x - 1


def is_ice(board, blocked, r, x):
    return 0 <= r < len(board) and 0 <= x < len(board[r]) and board[r][x] not in "0" \
        and board[r][x].isdigit() and (r, x) not in blocked


def mobility(board, blocked, r, x):
    n = 0
    for d in range(6):
        cr, cx = step(r, x, d)
        while is_ice(board, blocked, cr, cx):
            n += 1
            cr, cx = step(cr, cx, d)
    return n


def parse_pos(pos):
    if len(pos) < 2 or not pos[0].isalpha() or not pos[1:].isdigit():
        return None
    return int(pos[1:]) - 1, ord(pos[0].upper()) - 65


def fish_at(board, rx):
    if rx is None:
        return -1
    r, x = rx
    if 0 <= r < len(board) and 0 <= x < len(board[r]) and board[r][x].isdigit():
        return int(board[r][x])
    return -1


def score(board, mine, occupied, action):
    """mine: our penguin id -> cell; occupied: cells of every penguin."""
    parts = action.split()
    dst = parse_pos(parts[-1]) if parts else None
    if dst is None:
        return -1
    src = mine.get(parts[1]) if len(parts) >= 3 else None
    value = fish_at(board, dst)
    blocked = set(occupied) | {dst}
    if src is not None:
        value += SOURCE_WEIGHT * max(0, fish_at(board, src))
        blocked.discard(src)
        board = [row[:src[1]] + "0" + row[src[1] + 1:] if i == src[0] else row for i, row in enumerate(board)]
    return value + MOBILITY_WEIGHT * mobility(board, blocked - {dst}, dst[0], dst[1])


def main():
    read = sys.stdin.readline
    header = [read() for _ in range(3)]
    if not header[-1]:
        return
    n_players = int(header[0])
    me = header[1].strip()
    while True:
        line = read()
        if not line:
            return
        board = [read().strip() for _ in range(int(line))]
        for _ in range(n_players):
            read()
        mine, occupied = {}, []   # penguin ids are unique per player only
        for _ in range(int(read())):
            parts = read().split()
            if len(parts) >= 3:
                cell = parse_pos(parts[2])
                if cell is not None:
                    occupied.append(cell)
                    if parts[1] == me:
                        mine[parts[0]] = cell
        best, best_v = None, -2
        for _ in range(int(read())):
            action = read().strip()
            v = score(board, mine, occupied, action) if action else -2
            if v > best_v:
                best, best_v = action, v
        print(best if best else "MOVE 0 A1", flush=True)


if __name__ == "__main__":
    main()
