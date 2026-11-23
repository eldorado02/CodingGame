#!/usr/bin/env python3
"""Bandas 8x8 (CodinGame) — Python reference of the differential fuzz.

Rules ported from the referee (Oli8/CG-bandas, Referee.java), quirks included:
every pawn of the mover steps in the chosen direction, scanning in the
referee's order, recursively pushing any pawn in front; a pawn leaving the
grid or entering a hole ('x') disappears. After the move, empty border rows
then empty border columns become holes. A side with no pawn loses
(player 0 checked first, so 0 pawns each = player 1 wins).

Algorithm (v1): negamax alpha-beta, iterative deepening, node budget
(env CG_NODES). Branching is 4. Eval = pawn difference + edge safety.
"""
import os
import sys

EMPTY, PAWN0, PAWN1, HOLE = 0, 1, 2, 3
CHAR_CODE = {"-": EMPTY, "0": PAWN0, "1": PAWN1, "x": HOLE}
DIRECTIONS = ("UP", "RIGHT", "DOWN", "LEFT")
STEP = ((-1, 0), (0, 1), (1, 0), (0, -1))
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
PAWN_WEIGHT = 100        # material dominates
SAFETY_WEIGHT = 3        # per step between a pawn and the nearest drop
SAFETY_CAP = 3           # beyond this distance a pawn is considered safe
DEFAULT_NODES = 10000    # CodinGame budget: ~1.8 us/node local, ~40 ms on CG


class Grid:
    def __init__(self, height, width, cells):
        self.h = height
        self.w = width
        self.cells = cells  # flat list, row major

    def copy(self):
        return Grid(self.h, self.w, self.cells[:])

    def at(self, y, x):
        return self.cells[y * self.w + x]

    def put(self, y, x, value):
        self.cells[y * self.w + x] = value

    def step_pawn(self, pawn, y, x, dy, dx):
        """referee singleMove*: push what is in front, then move or fall."""
        ny, nx = y + dy, x + dx
        if not (0 <= ny < self.h and 0 <= nx < self.w):
            self.put(y, x, EMPTY)
            return
        front = self.at(ny, nx)
        if front in (PAWN0, PAWN1):
            self.step_pawn(front, ny, nx, dy, dx)
            self.step_pawn(pawn, y, x, dy, dx)
        elif front == HOLE:
            self.put(y, x, EMPTY)
        else:
            self.put(ny, nx, pawn)
            self.put(y, x, EMPTY)

    def scan_order(self, direction):
        """Cells in the order the referee scans them for this direction."""
        h, w = self.h, self.w
        if direction == 0:    # UP: column by column, top to bottom
            return [(y, x) for x in range(w) for y in range(h)]
        if direction == 2:    # DOWN: column by column, bottom to top
            return [(y, x) for x in range(w) for y in range(h - 1, -1, -1)]
        if direction == 1:    # RIGHT: row by row, right to left
            return [(y, x) for y in range(h) for x in range(w - 1, -1, -1)]
        return [(y, x) for y in range(h) for x in range(w)]  # LEFT

    def move(self, player, direction):
        pawn = PAWN0 + player
        dy, dx = STEP[direction]
        for y, x in self.scan_order(direction):
            if self.at(y, x) == pawn:
                self.step_pawn(pawn, y, x, dy, dx)

    def _line_has_pawn(self, cells):
        return any(self.cells[i] in (PAWN0, PAWN1) for i in cells)

    def _trim(self, lines):
        """Pawn-free lines become holes from one side until a line with a pawn."""
        for cells in lines:
            if self._line_has_pawn(cells):
                return
            for i in cells:
                self.cells[i] = HOLE

    def remove_empty_borders(self):
        rows = [[y * self.w + x for x in range(self.w)] for y in range(self.h)]
        cols = [[y * self.w + x for y in range(self.h)] for x in range(self.w)]
        self._trim(rows)
        self._trim(rows[::-1])
        self._trim(cols)
        self._trim(cols[::-1])

    def play(self, player, direction):
        self.move(player, direction)
        self.remove_empty_borders()

    def count(self, player):
        return self.cells.count(PAWN0 + player)

    def winner(self):
        """-1 while both sides have pawns (referee checkWinner order)."""
        if self.count(0) == 0:
            return 1
        if self.count(1) == 0:
            return 0
        return -1

    def drop_distance(self, y, x):
        """Steps before this pawn would fall, in its most exposed direction."""
        best = SAFETY_CAP
        for dy, dx in STEP:
            steps, ny, nx = 0, y + dy, x + dx
            while steps < best and 0 <= ny < self.h and 0 <= nx < self.w and self.at(ny, nx) != HOLE:
                steps += 1
                ny += dy
                nx += dx
            best = min(best, steps)
        return best


def evaluate(grid, player):
    score = 0
    for y in range(grid.h):
        for x in range(grid.w):
            v = grid.at(y, x)
            if v in (PAWN0, PAWN1):
                sign = 1 if v == PAWN0 + player else -1
                score += sign * (PAWN_WEIGHT + SAFETY_WEIGHT * grid.drop_distance(y, x))
    return score


class Search:
    def __init__(self, node_limit):
        self.node_limit = node_limit
        self.nodes = 0
        self.aborted = False
        self.last_depth = 0

    def move_value(self, grid, player, direction, depth, alpha, beta, ply):
        child = grid.copy()
        child.play(player, direction)
        winner = child.winner()
        if winner >= 0:
            return WIN - ply if winner == player else -(WIN - ply)
        return -self.negamax(child, 1 - player, depth - 1, -beta, -alpha, ply + 1)

    def negamax(self, grid, player, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > self.node_limit:
            self.aborted = True
            return 0
        if depth == 0:
            return evaluate(grid, player)
        best = -INF
        for direction in range(4):
            value = self.move_value(grid, player, direction, depth, alpha, beta, ply)
            if self.aborted:
                return 0
            if value > best:
                best = value
            if value > alpha:
                alpha = value
            if alpha >= beta:
                break
        return best

    def run(self, grid, player):
        order = list(range(4))
        best = order[0]
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_value, best_index = -INF, -INF, 0
            for i, direction in enumerate(order):
                value = self.move_value(grid, player, direction, depth, alpha, INF, 1)
                if self.aborted:
                    break
                if value > best_value:
                    best_value, best_index = value, i
                if value > alpha:
                    alpha = value
            if self.aborted:
                break
            best = order[best_index]
            self.last_depth = depth
            order.insert(0, order.pop(best_index))
            if best_value >= WIN - MAX_DEPTH or best_value <= -(WIN - MAX_DEPTH):
                break
        return best


def main():
    env = os.environ.get("CG_NODES")
    budget = int(env) if env else DEFAULT_NODES
    me = int(sys.stdin.readline())
    height = int(sys.stdin.readline())
    width = int(sys.stdin.readline())
    while True:
        rows = []
        for _ in range(height):
            line = sys.stdin.readline()
            if not line:
                return
            rows.append(line.split())
        cells = [CHAR_CODE.get(row[x], HOLE) if x < len(row) else HOLE
                 for row in rows for x in range(width)]
        grid = Grid(height, width, cells)
        search = Search(budget)
        direction = search.run(grid, me)
        sys.stderr.write("d=%d n=%d\n" % (search.last_depth, search.nodes))
        sys.stdout.write(DIRECTIONS[direction] + "\n")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
