#!/usr/bin/env python3
"""Dice Duel — Python reference of the differential fuzz, v1 alpha-beta on material.

Rules (referee eulerscheZahl/Dice-Duel, engine/Die.java): a die rolls exactly
`top` steps (U = y+1, D = y-1, R = x+1, L = x-1), never revisits a cell, never
crosses a die; the last step may land on an enemy die and capture it. The
state is [top, front, bottom, back, left, right]; U rolls (1,2,3,0,4,5), R
rolls (4,1,5,3,2,0) (D, L are their inverses). Wood 1 only (SUM_RULE): a
capture is legal when the rolled top + the captured top equal 7. No move =
loss; final score = dice left.
Moves are deduplicated by (die, end cell, end state), first path kept (DFS
order U, D, R, L). Negamax alpha-beta, iterative deepening, deterministic
node budget, captures first, material evaluation.
"""
import os
import sys

SIZE = 8
SUM_RULE = False          # True from Wood 1 on (capture only when tops sum to 7)
CAPTURE_SUM = 7
DIE_VALUE = 100
STEPS = (("U", 0, 1), ("D", 0, -1), ("R", 1, 0), ("L", -1, 0))
ROLL = {"U": (1, 2, 3, 0, 4, 5), "D": (3, 0, 1, 2, 4, 5),
        "R": (4, 1, 5, 3, 2, 0), "L": (5, 1, 4, 3, 0, 2)}
TURN_NODES = 4000


def rolled(state, step):
    return tuple(state[i] for i in ROLL[step])


class Generator:
    """Self-avoiding rolls of every die of one side (DFS order U, D, R, L)."""

    def __init__(self, dice, side):
        self.dice = dice
        self.side = side
        self.at = {(d[1], d[2]): i for i, d in enumerate(dice)}
        self.moves = []
        self.seen = set()

    def run(self):
        for i, (owner, x, y, state) in enumerate(self.dice):
            if owner == self.side:
                self.walk(i, x, y, state, "", {(x, y)})
        captures = [m for m in self.moves if m[5] >= 0]
        return captures + [m for m in self.moves if m[5] < 0]

    def walk(self, i, x, y, state, path, visited):
        top = self.dice[i][3][0]
        if len(path) == top:
            self.record(i, x, y, state, path)
            return
        for step, dx, dy in STEPS:
            nx, ny = x + dx, y + dy
            if not (0 <= nx < SIZE and 0 <= ny < SIZE) or (nx, ny) in visited:
                continue
            j = self.at.get((nx, ny), -1)
            if j >= 0 and (self.dice[j][0] == self.side or len(path) + 1 < top):
                continue    # dice block the way; only the last step captures
            visited.add((nx, ny))
            self.walk(i, nx, ny, rolled(state, step), path + step, visited)
            visited.discard((nx, ny))

    def record(self, i, x, y, state, path):
        j = self.at.get((x, y), -1)
        key = (i, x, y, state)
        if key in self.seen or (j >= 0 and SUM_RULE and state[0] + self.dice[j][3][0] != CAPTURE_SUM):
            return
        self.seen.add(key)
        self.moves.append((i, path, x, y, state, j))


def gen_moves(dice, side):
    """(die index, path, x, y, state, captured index or -1), captures first."""
    return Generator(dice, side).run()


def apply(dice, move):
    i, _, x, y, state, j = move
    out = list(dice)
    out[i] = (dice[i][0], x, y, state)
    if j >= 0:
        out.pop(j)
    return out


def evaluate(dice, side):
    mine = sum(1 for d in dice if d[0] == side)
    return DIE_VALUE * (mine - (len(dice) - mine))


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


class Search:
    def __init__(self):
        self.nodes = 0
        self.depth = 0

    def negamax(self, dice, side, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        if depth == 0:
            return evaluate(dice, side)
        moves = gen_moves(dice, side)
        if not moves:
            return -(WIN - ply)
        best = -INF
        for m in moves:
            v = -self.negamax(apply(dice, m), 1 - side, depth - 1, -beta, -alpha, ply + 1)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, dice, moves):
        best = moves[0]
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur = -INF, -INF, 0
            try:
                for k, m in enumerate(moves):
                    v = -self.negamax(apply(dice, m), 1, depth - 1, -INF, -alpha, 1)
                    if v > best_v:
                        best_v, cur = v, k
                    if v > alpha:
                        alpha = v
            except Aborted:
                break
            best = moves[cur]
            self.depth = depth
            moves.insert(0, moves.pop(cur))   # PV first next iteration
            if abs(best_v) >= WIN - MAX_DEPTH:
                break
        return best


def main():
    read = sys.stdin.readline
    while True:
        line = read()
        if not line.strip():
            return
        dice = []
        for _ in range(int(line)):
            parts = read().split()
            owner, cell = int(parts[0]), parts[1]
            dice.append((owner, ord(cell[0]) - ord("A"), int(cell[1:]) - 1, tuple(int(v) for v in parts[2:8])))
        moves = gen_moves(dice, 0)
        if not moves:
            print("A1 U", flush=True)
            continue
        i, path, *_ = Search().run(dice, moves)
        print("%s%d %s" % (chr(ord("A") + dice[i][1]), dice[i][2] + 1, path), flush=True)


if __name__ == "__main__":
    main()
