#!/usr/bin/env python3
"""Yinsh — Python reference of the differential fuzz, v1 one-ply greedy on legal actions.

We answer "yes" to receive the legal actions. Board (probe 2026-11-26): one
input row per column a..k, cells by increasing number; columns start at
numbers 2,1,1,1,1,2,2,3,4,5,7. Lines run along the axes (0,1), (1,0) and
(1,1) in (letter, number) coordinates.
Each legal action ("c6", "f7-f2", "xd4-h4xf2;b3-c3", ...) is simulated:
move = our marker on the start cell, ring to the end, markers in between
flipped; "xA-BxC" = markers A..B and the ring C removed (a ring removed in
our action is ours). Score = RING_VALUE * rings removed + sum over every
5-cell window holding markers of one colour only (+m*m ours, -t*t theirs)
+ marker difference + ring mobility difference. STEAL is never played.
Ties: first action of the list.
"""
import sys

COLUMNS = 11
COLUMN_START = (2, 1, 1, 1, 1, 2, 2, 3, 4, 5, 7)
COLUMN_SIZE = (4, 7, 8, 9, 10, 9, 10, 9, 8, 7, 4)
EMPTY, MY_RING, MY_MARKER, THEIR_RING, THEIR_MARKER = ".", "R", "S", "r", "s"
AXES = ((0, 1), (1, 0), (1, 1))
DIRECTIONS = AXES + tuple((-a, -b) for a, b in AXES)
ROW = 5                 # markers in a winning row
RING_VALUE = 1000
STEAL = "STEAL"
MAX_CELL_TEXT = 3        # "k11"


def valid(col, num):
    return 0 <= col < COLUMNS and COLUMN_START[col] <= num < COLUMN_START[col] + COLUMN_SIZE[col]


def windows():
    """Every line of ROW consecutive valid cells."""
    out = []
    for col in range(COLUMNS):
        for num in range(1, 12):
            for dc, dn in AXES:
                cells = [(col + k * dc, num + k * dn) for k in range(ROW)]
                if all(valid(c, n) for c, n in cells):
                    out.append(cells)
    return out


WINDOWS = windows()


def parse_cell(text):
    """'f10' -> (5, 10); None when it is not a board cell."""
    if not 2 <= len(text) <= MAX_CELL_TEXT or not text[1:].isdigit():
        return None
    col, num = ord(text[0].lower()) - ord("a"), int(text[1:])
    return (col, num) if valid(col, num) else None


def sign(v):
    return (v > 0) - (v < 0)


def segment(a, b):
    """Cells from a to b inclusive when they share an axis, else None."""
    dc, dn = b[0] - a[0], b[1] - a[1]
    if not (dc == 0 or dn == 0 or dc == dn):
        return None
    sc, sn = sign(dc), sign(dn)
    steps = max(abs(dc), abs(dn))
    return [(a[0] + k * sc, a[1] + k * sn) for k in range(steps + 1)]


FLIP = {MY_MARKER: THEIR_MARKER, THEIR_MARKER: MY_MARKER}


def apply_move(board, src, dst):
    line = segment(src, dst)
    board[dst] = board[src]
    board[src] = MY_MARKER
    if line:
        for cell in line[1:-1]:
            board[cell] = FLIP.get(board[cell], board[cell])


def apply_removal(board, text):
    """'xA-BxC': clears the markers A..B and the ring C; returns 1 if a ring went."""
    parts = text[1:].split("x")
    ends = parts[0].split("-")
    if len(ends) == 2:
        a, b = parse_cell(ends[0]), parse_cell(ends[1])
        line = segment(a, b) if a and b else None
        for cell in line or ():
            if board[cell] in (MY_MARKER, THEIR_MARKER):
                board[cell] = EMPTY
    ring = parse_cell(parts[1]) if len(parts) > 1 else None
    if ring and board[ring] in (MY_RING, THEIR_RING):
        board[ring] = EMPTY
        return 1
    return 0


def simulate(board, action):
    """Board after our action and the rings it removed."""
    after = dict(board)
    removed = 0
    for step in action.split(";"):
        if step.startswith("x"):
            removed += apply_removal(after, step)
        elif "-" in step:
            src, dst = (parse_cell(t) for t in step.split("-", 1))
            if src and dst:
                apply_move(after, src, dst)
        else:
            cell = parse_cell(step)
            if cell and after[cell] == EMPTY:
                after[cell] = MY_RING
    return after, removed


def mobility(board, ring):
    """Destinations of every ring of this colour."""
    total = 0
    for cell, value in board.items():
        if value != ring:
            continue
        for dc, dn in DIRECTIONS:
            c, n = cell[0] + dc, cell[1] + dn
            jumped = False
            while valid(c, n) and board[(c, n)] not in (MY_RING, THEIR_RING):
                if board[(c, n)] == EMPTY:
                    total += 1
                    if jumped:
                        break
                else:
                    jumped = True
                c, n = c + dc, n + dn
    return total


def evaluate(board):
    score = 0
    for cells in WINDOWS:
        mine = sum(1 for c in cells if board[c] == MY_MARKER)
        theirs = sum(1 for c in cells if board[c] == THEIR_MARKER)
        if theirs == 0:
            score += mine * mine
        elif mine == 0:
            score -= theirs * theirs
    markers = sum(1 for v in board.values() if v == MY_MARKER) - sum(1 for v in board.values() if v == THEIR_MARKER)
    return score + markers + mobility(board, MY_RING) - mobility(board, THEIR_RING)


def choose(board, actions):
    best, best_score = None, None
    for action in actions:
        if action.upper() == STEAL:
            continue
        after, removed = simulate(board, action)
        score = RING_VALUE * removed + evaluate(after)
        if best_score is None or score > best_score:
            best, best_score = action, score
    return best if best is not None else (actions[0] if actions else STEAL)


def read_board(rows):
    board = {}
    for col in range(COLUMNS):
        text = rows[col] if col < len(rows) else ""
        for k in range(COLUMN_SIZE[col]):
            board[(col, COLUMN_START[col] + k)] = text[k] if k < len(text) else EMPTY
    return board


def main():
    read = sys.stdin.readline
    if not read():
        return
    print("yes", flush=True)
    while True:
        line = read()
        if not line.strip():
            return
        rows = [read().strip() for _ in range(int(line))]
        actions = [read().strip() for _ in range(int(read()))]
        print(choose(read_board(rows), actions), flush=True)


if __name__ == "__main__":
    main()
