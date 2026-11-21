#!/usr/bin/env python3
"""Ataxx bot — deterministic 1-ply greedy over the referee-provided actions.

The referee supplies the exact list of legal actions each turn; this bot only
has to choose one. For every offered action we simulate it on a copy of the
board and score the result with an integer evaluation (material + center
control + local pressure, inspired by GeckoX94/Ataxx heuristics v2+v3).
The best score wins; ties are broken by the lexicographically smallest ASCII
action string. No floats, no RNG: identical behaviour in C/C++/Rust.
"""
import sys

# Center-control weights (row 0..6 from bottom; matrix is doubly symmetric).
WEIGHTS = [
    1, 2, 2, 3, 2, 2, 1,
    2, 3, 4, 4, 4, 3, 2,
    2, 4, 5, 5, 5, 4, 2,
    3, 4, 5, 6, 5, 4, 3,
    2, 4, 5, 5, 5, 4, 2,
    2, 3, 4, 4, 4, 3, 2,
    1, 2, 2, 3, 2, 2, 1,
]

# 8-neighbour index lists for each of the 49 cells.
NEIGHBORS = []
for _y in range(7):
    for _x in range(7):
        _nb = []
        for _dy in (-1, 0, 1):
            for _dx in (-1, 0, 1):
                if _dx == 0 and _dy == 0:
                    continue
                _nx, _ny = _x + _dx, _y + _dy
                if 0 <= _nx < 7 and 0 <= _ny < 7:
                    _nb.append(_ny * 7 + _nx)
        NEIGHBORS.append(_nb)

WIN_SCORE = 1_000_000
PIECE_WEIGHT = 20


def char_to_cell(ch: str) -> int:
    """'.'→0, '0'→1, '1'→2, '#'→3; defensive default 0."""
    if ch == "1":
        return 2
    if ch == "0":
        return 1
    if ch == "#":
        return 3
    return 0


def parse_action(action: str):
    """Return (is_jump, x0, y0, x1, y1) with indices clamped to [0, 6]."""
    s = action.strip()
    if len(s) < 2:
        return False, 0, 0, 0, 0
    x0 = max(0, min(6, ord(s[0]) - ord("a")))
    y0 = max(0, min(6, ord(s[1]) - ord("1")))
    if len(s) >= 4:
        x1 = max(0, min(6, ord(s[2]) - ord("a")))
        y1 = max(0, min(6, ord(s[3]) - ord("1")))
        return True, x0, y0, x1, y1
    return False, x0, y0, x0, y0


def simulate(cells, action: str, me: int, opp: int):
    """Apply one action to a copy of the board and return the new board."""
    board = cells[:]
    is_jump, x0, y0, x1, y1 = parse_action(action)
    dst = y1 * 7 + x1
    if is_jump:
        board[y0 * 7 + x0] = 0  # source vacated by a jump
    board[dst] = me
    for nb in NEIGHBORS[dst]:
        if board[nb] == opp:
            board[nb] = me
    return board


def evaluate(board, me: int, opp: int) -> int:
    """Integer evaluation of a board from `me`'s perspective."""
    my_count = 0
    opp_count = 0
    score = 0
    for i in range(49):
        cell = board[i]
        if cell == me:
            my_count += 1
            s = WEIGHTS[i]
            for nb in NEIGHBORS[i]:
                if board[nb] == opp:
                    s += 1
            score += s
        elif cell == opp:
            opp_count += 1
            s = WEIGHTS[i]
            for nb in NEIGHBORS[i]:
                if board[nb] == me:
                    s += 1
            score -= s
    if my_count == 0:
        return -WIN_SCORE
    if opp_count == 0:
        return WIN_SCORE
    return score + PIECE_WEIGHT * (my_count - opp_count)


def choose(cells, me: int, actions) -> str:
    """Pick the best action: max eval, then smallest ASCII action string."""
    opp = 3 - me
    best = None
    best_eval = None
    for action in actions:
        action = action.strip()
        if not action:
            continue
        val = evaluate(simulate(cells, action, me, opp), me, opp)
        if best_eval is None or val > best_eval or (val == best_eval and action < best):
            best_eval = val
            best = action
    return best if best is not None else "random"


def main() -> None:
    first = sys.stdin.readline()
    if not first:
        return
    try:
        me = int(first.strip()) + 1
    except ValueError:
        me = 1
    if me not in (1, 2):
        me = 1

    while True:
        cells = []
        complete = True
        for _ in range(7):
            line = sys.stdin.readline()
            if not line:
                complete = False
                break
            row = line.strip()
            cells.extend(char_to_cell(row[j]) if j < len(row) else 0
                         for j in range(7))
        if not complete:
            break
        # lastAction line — consumed, not used.
        if sys.stdin.readline() == "":
            break
        count_line = sys.stdin.readline()
        if count_line == "":
            break
        try:
            count = max(0, int(count_line.strip()))
        except ValueError:
            count = 0
        actions = []
        for _ in range(count):
            action = sys.stdin.readline()
            if action == "":
                break
            actions.append(action.strip())
        print(choose(cells, me, actions), flush=True)


if __name__ == "__main__":
    main()
