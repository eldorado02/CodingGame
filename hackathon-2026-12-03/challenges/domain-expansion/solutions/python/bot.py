#!/usr/bin/env python3
"""Domain Expansion 7x7 (CodinGame) — Python reference of the differential fuzz.

Rules from the referee (Mortis66666/CG-Domain-Expansion, Board.kt):
- an action = move the token to a cell at Manhattan distance <= 3 reachable
  by a BFS path (walls and the other token block) whose nodes are expanded up
  to depth 3, i.e. a path of length <= 4 is allowed; then build a wall on one
  free side of the token's cell (board borders are walls);
- after each action, if the mover can no longer reach the other token
  (walls only), the game ends: domain sizes are compared.

Algorithm (v1): negamax alpha-beta over actions with iterative deepening and
a node budget (env CG_NODES); root actions ordered by their 1-ply value.
Eval = Voronoi territory (BFS through walls, closer cells) difference.
"""
import os
import sys

SIZE = 7
CELLS = SIZE * SIZE
MAX_MOVE = 3                 # Manhattan reach of a move
PATH_EXPANSION_DEPTH = 3     # referee BFS expands nodes up to this depth
DIRECTIONS = (("U", 0, -1), ("R", 1, 0), ("D", 0, 1), ("L", -1, 0))
OPPOSITE = (2, 3, 0, 1)
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
DEFAULT_NODES = 10000        # CodinGame budget (100 ms per turn): ~15 ms local
FIRST_TURN_NODES = 60000     # first turn has 1000 ms


def _neighbour_table():
    table = []
    for cell in range(CELLS):
        x, y = cell % SIZE, cell // SIZE
        row = []
        for _, dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            row.append(ny * SIZE + nx if 0 <= nx < SIZE and 0 <= ny < SIZE else -1)
        table.append(tuple(row))
    return tuple(table)


NEIGHBOUR = _neighbour_table()


def border_walls():
    """walls[cell] = bitmask over DIRECTIONS of sides already walled."""
    walls = [0] * CELLS
    for cell in range(CELLS):
        for d in range(4):
            if NEIGHBOUR[cell][d] < 0:
                walls[cell] |= 1 << d
    return walls


def manhattan(a, b):
    return abs(a % SIZE - b % SIZE) + abs(a // SIZE - b // SIZE)


class State:
    def __init__(self, walls, tokens):
        self.walls = walls          # list of bitmasks
        self.tokens = tokens        # [player 0 cell, player 1 cell]

    def copy(self):
        return State(self.walls[:], self.tokens[:])

    def apply(self, player, cell, d):
        self.tokens[player] = cell
        self.walls[cell] |= 1 << d
        self.walls[NEIGHBOUR[cell][d]] |= 1 << OPPOSITE[d]


def destinations(state, player):
    """Cells the token of `player` may move to, ascending (referee findPath)."""
    start = state.tokens[player]
    blocker = state.tokens[1 - player]
    depth = {start: 0}
    queue = [start]
    for cell in queue:
        if depth[cell] > PATH_EXPANSION_DEPTH:
            continue
        for d in range(4):
            nxt = NEIGHBOUR[cell][d]
            if state.walls[cell] >> d & 1 or nxt == blocker or nxt in depth:
                continue
            depth[nxt] = depth[cell] + 1
            queue.append(nxt)
    return sorted(c for c in depth if manhattan(start, c) <= MAX_MOVE)


def actions(state, player):
    """(cell, wall direction) pairs in a fixed order."""
    result = []
    for cell in destinations(state, player):
        for d in range(4):
            if not state.walls[cell] >> d & 1:
                result.append((cell, d))
    return result


def distances(state, start):
    """BFS steps through walls only (tokens do not block territory)."""
    dist = [-1] * CELLS
    dist[start] = 0
    queue = [start]
    for cell in queue:
        for d in range(4):
            if state.walls[cell] >> d & 1:
                continue
            nxt = NEIGHBOUR[cell][d]
            if dist[nxt] < 0:
                dist[nxt] = dist[cell] + 1
                queue.append(nxt)
    return dist


def final_score(state, player):
    """None while the tokens are connected, else the outcome for `player`."""
    mine = distances(state, state.tokens[player])
    if mine[state.tokens[1 - player]] >= 0:
        return None
    theirs = distances(state, state.tokens[1 - player])
    diff = sum(1 for v in mine if v >= 0) - sum(1 for v in theirs if v >= 0)
    if diff > 0:
        return WIN + diff
    if diff < 0:
        return -WIN + diff
    return 0


def evaluate(state, player):
    """Voronoi territory of `player` minus the opponent's."""
    mine = distances(state, state.tokens[player])
    theirs = distances(state, state.tokens[1 - player])
    score = 0
    for a, b in zip(mine, theirs):
        if a < 0 and b < 0:
            continue
        if b < 0 or (0 <= a < b):
            score += 1
        elif a < 0 or b < a:
            score -= 1
    return score


class Search:
    def __init__(self, node_limit):
        self.node_limit = node_limit
        self.nodes = 0
        self.aborted = False
        self.last_depth = 0

    def action_value(self, state, player, action, depth, alpha, beta):
        """Value of `action` for `player`, searching depth - 1 below it."""
        child = state.copy()
        child.apply(player, *action)
        final = final_score(child, player)
        if final is not None:
            return final
        return -self.negamax(child, 1 - player, depth - 1, -beta, -alpha)

    def negamax(self, state, player, depth, alpha, beta):
        self.nodes += 1
        if self.nodes > self.node_limit:
            self.aborted = True
            return 0
        if depth == 0:
            return evaluate(state, player)
        best = -INF
        for action in actions(state, player):
            value = self.action_value(state, player, action, depth, alpha, beta)
            if self.aborted:
                return 0
            if value > best:
                best = value
            if value > alpha:
                alpha = value
            if alpha >= beta:
                break
        return best

    def ordered_root(self, state, player):
        """Root actions sorted by 1-ply value (stable insertion sort)."""
        acts = actions(state, player)
        keys = [self.action_value(state, player, a, 1, -INF, INF) for a in acts]
        for i in range(1, len(acts)):
            item, key, j = acts[i], keys[i], i - 1
            while j >= 0 and keys[j] < key:
                acts[j + 1], keys[j + 1] = acts[j], keys[j]
                j -= 1
            acts[j + 1], keys[j + 1] = item, key
        return acts

    def run(self, state, player):
        acts = self.ordered_root(state, player)
        best = acts[0]
        for depth in range(2, MAX_DEPTH + 1):
            alpha, best_value, best_index = -INF, -INF, 0
            for i, action in enumerate(acts):
                value = self.action_value(state, player, action, depth, alpha, INF)
                if self.aborted:
                    break
                if value > best_value:
                    best_value, best_index = value, i
                if value > alpha:
                    alpha = value
            if self.aborted:
                break
            best = acts[best_index]
            self.last_depth = depth
            acts.insert(0, acts.pop(best_index))
            if best_value >= WIN or best_value <= -WIN:
                break
        return best


def read_ints(line):
    return [int(t) for t in line.split()[:2]]


def main():
    env = os.environ.get("CG_NODES")
    head = sys.stdin.readline()
    if not head.strip():
        return
    mx, my = read_ints(sys.stdin.readline())
    ox, oy = read_ints(sys.stdin.readline())
    me, opp = 0, 1     # internal indexes: 0 = us, 1 = them
    state = State(border_walls(), [my * SIZE + mx, oy * SIZE + ox])
    letters = [name for name, _, _ in DIRECTIONS]
    first_turn = True
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        parts = line.split()
        if len(parts) >= 3 and parts[2] in letters:
            state.apply(opp, int(parts[1]) * SIZE + int(parts[0]), letters.index(parts[2]))
        budget = int(env) if env else (FIRST_TURN_NODES if first_turn else DEFAULT_NODES)
        search = Search(budget)
        cell, d = search.run(state, me)
        state.apply(me, cell, d)
        sys.stderr.write("d=%d n=%d\n" % (search.last_depth, search.nodes))
        sys.stdout.write("%d %d %s\n" % (cell % SIZE, cell // SIZE, letters[d]))
        sys.stdout.flush()
        first_turn = False


if __name__ == "__main__":
    main()
