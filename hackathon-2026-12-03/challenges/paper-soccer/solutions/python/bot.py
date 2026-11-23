#!/usr/bin/env python3
"""Paper Soccer 8x10 (CodinGame) — Python reference of the differential fuzz.

Geometry copied from the referee (jdermont/CodinGame-paper-soccer, Pitch.java):
points (x, y), x in 0..8, y in 0..10, plus goal points x in 3..5 at y = -1
(top, player 0 scores there) and y = 11 (bottom, player 1 scores there).
Border and goal-frame edges are drawn from the start. Arriving on a point that
already had a drawn edge makes the ball bounce (the turn goes on); a turn
ending on a point with no free edge loses for the mover.

Algorithm (v1): negamax alpha-beta over complete turns with iterative
deepening and a node budget (env CG_NODES). A turn = bounce sequence found
by DFS; only the first sequence (direction order 0..7) reaching each
(end point, outcome) is kept, with a step cap. Eval = rows to goal + unused-edge
BFS goal distance difference (+ centring near goals).
"""
import os
import sys

WIDTH, HEIGHT = 8, 10
COLS, ROWS = WIDTH + 1, HEIGHT + 1
FIELD_POINTS = COLS * ROWS            # 99
TOP_GOAL = FIELD_POINTS               # 99..101 = (3..5, -1)
BOTTOM_GOAL = FIELD_POINTS + 3        # 102..104 = (3..5, 11)
POINTS = FIELD_POINTS + 6
GOAL_XS = (3, 4, 5)
CENTER_X = WIDTH // 2
START_BALL = (HEIGHT // 2) * COLS + CENTER_X
DIRECTIONS = ((0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1))
REMOVED_PAIRS = (((2, 0), (3, -1)), ((6, 0), (5, -1)), ((2, 10), (3, 11)), ((6, 10), (5, 11)))
PLAYER_TOP, PLAYER_BOTTOM = 0, 1      # player 0 attacks the top goal

WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
ROW_WEIGHT = 80                       # one row of vertical progress
GOAL_DISTANCE_WEIGHT = 120            # unused-edge path to goal (Lecorbio evaluator)
NO_PATH = 40                          # goal distance when every route is drawn
CENTRE_WEIGHT = 15                    # per column away from the goal mouth
CENTRE_ZONE_ROWS = 3                  # centring matters this close to a goal
MAX_DFS_STEPS = 20000                 # bounce-sequence enumeration cap per node
DEFAULT_NODES = 3000                  # CodinGame budget (200 ms per turn)
FIRST_TURN_NODES = 15000              # first turn has 1000 ms


def _point_index(x, y):
    if 0 <= x < COLS and 0 <= y < ROWS:
        return y * COLS + x
    if x in GOAL_XS and y == -1:
        return TOP_GOAL + x - GOAL_XS[0]
    if x in GOAL_XS and y == ROWS:
        return BOTTOM_GOAL + x - GOAL_XS[0]
    return -1


def _point_xy(p):
    if p < FIELD_POINTS:
        return p % COLS, p // COLS
    if p < BOTTOM_GOAL:
        return GOAL_XS[0] + p - TOP_GOAL, -1
    return GOAL_XS[0] + p - BOTTOM_GOAL, ROWS


def _build_neighbours():
    removed = set()
    for a, b in REMOVED_PAIRS:
        removed.add((_point_index(*a), _point_index(*b)))
        removed.add((_point_index(*b), _point_index(*a)))
    table = []
    for p in range(POINTS):
        x, y = _point_xy(p)
        row = []
        for dx, dy in DIRECTIONS:
            q = _point_index(x + dx, y + dy)
            row.append(-1 if q < 0 or (p, q) in removed else q)
        table.append(tuple(row))
    return tuple(table)


NEIGHBOUR = _build_neighbours()
DEGREE = tuple(sum(1 for q in row if q >= 0) for row in NEIGHBOUR)


def _direction_between(p, q):
    return NEIGHBOUR[p].index(q)


def _initial_edges():
    """Border lines and goal frames, as Pitch() draws them."""
    pairs = []
    for y in range(HEIGHT):
        pairs.append(((0, y), (0, y + 1)))
        pairs.append(((WIDTH, y), (WIDTH, y + 1)))
    for x in range(WIDTH):
        if x < GOAL_XS[0] or x >= GOAL_XS[-1]:
            pairs.append(((x, 0), (x + 1, 0)))
            pairs.append(((x, HEIGHT), (x + 1, HEIGHT)))
    for gy, fy in ((-1, 0), (ROWS, HEIGHT)):
        pairs.append(((3, gy), (4, gy)))
        pairs.append(((4, gy), (5, gy)))
        pairs.append(((3, gy), (3, fy)))
        pairs.append(((5, gy), (5, fy)))
    return [(_point_index(*a), _point_index(*b)) for a, b in pairs]


class Pitch:
    """drawn[p] = bitmask of directions already drawn from point p."""

    def __init__(self):
        self.drawn = [0] * POINTS
        self.free = list(DEGREE)
        self.ball = START_BALL
        for a, b in _initial_edges():
            self.draw(a, _direction_between(a, b))

    def copy(self):
        c = Pitch.__new__(Pitch)
        c.drawn = self.drawn[:]
        c.free = self.free[:]
        c.ball = self.ball
        return c

    def draw(self, p, d):
        q = NEIGHBOUR[p][d]
        self.drawn[p] |= 1 << d
        self.drawn[q] |= 1 << ((d + 4) % 8)
        self.free[p] -= 1
        self.free[q] -= 1
        return q

    def undraw(self, p, d):
        q = NEIGHBOUR[p][d]
        self.drawn[p] &= ~(1 << d)
        self.drawn[q] &= ~(1 << ((d + 4) % 8))
        self.free[p] += 1
        self.free[q] += 1

    def can_move(self, d):
        return NEIGHBOUR[self.ball][d] >= 0 and not self.drawn[self.ball] >> d & 1

    def apply(self, digits):
        for ch in digits:
            d = ord(ch) - ord("0")
            if 0 <= d < 8 and self.can_move(d):
                self.ball = self.draw(self.ball, d)


def goal_owner(p):
    """Player who scores when the ball reaches p, or -1."""
    if TOP_GOAL <= p < BOTTOM_GOAL:
        return PLAYER_TOP
    if p >= BOTTOM_GOAL:
        return PLAYER_BOTTOM
    return -1


# Outcomes of a complete turn, from the mover's point of view.
NORMAL, WON, LOST = 0, 1, 2


def generate_turns(pitch, mover):
    """Complete turns from pitch.ball: list of (digits, end_point, outcome),
    first sequence per (end point, outcome) in DFS order, at most
    MAX_DFS_STEPS steps."""
    found = {}
    order = []
    steps = [0]
    path = []

    def dfs():
        p = pitch.ball
        for d in range(8):
            if steps[0] >= MAX_DFS_STEPS:
                return
            if not pitch.can_move(d):
                continue
            steps[0] += 1
            q = pitch.draw(p, d)
            pitch.ball = q
            path.append(d)
            owner = goal_owner(q)
            bounce = DEGREE[q] - pitch.free[q] > 1
            if owner >= 0:
                outcome = WON if owner == mover else LOST
            elif pitch.free[q] == 0:
                outcome = LOST
            elif bounce:
                outcome = None
            else:
                outcome = NORMAL
            if outcome is None:
                dfs()
            elif (q, outcome) not in found:
                found[(q, outcome)] = ("".join(map(str, path)), q, outcome)
                order.append((q, outcome))
            path.pop()
            pitch.ball = p
            pitch.undraw(p, d)

    dfs()
    return [found[key] for key in order]


def rows_to_goal(player, p):
    _, y = _point_xy(p)
    return y + 1 if player == PLAYER_TOP else ROWS - y


def goal_distance(pitch, player):
    """Edges the ball needs, over unused edges only, to reach the goal
    `player` attacks (bounces ignored); NO_PATH if walled off."""
    target_lo = TOP_GOAL if player == PLAYER_TOP else BOTTOM_GOAL
    dist = [NO_PATH] * POINTS
    dist[pitch.ball] = 0
    queue = [pitch.ball]
    for p in queue:
        if target_lo <= p < target_lo + len(GOAL_XS):
            return dist[p]
        if goal_owner(p) >= 0:
            continue  # the ball stops in any goal
        for d in range(8):
            q = NEIGHBOUR[p][d]
            if q >= 0 and not pitch.drawn[p] >> d & 1 and dist[q] == NO_PATH:
                dist[q] = dist[p] + 1
                queue.append(q)
    return NO_PATH


def evaluate(pitch, player):
    """Score for `player`, who is about to move with the ball at pitch.ball."""
    x, _ = _point_xy(pitch.ball)
    mine = rows_to_goal(player, pitch.ball)
    theirs = rows_to_goal(1 - player, pitch.ball)
    score = ROW_WEIGHT * (theirs - mine)
    score += GOAL_DISTANCE_WEIGHT * (goal_distance(pitch, 1 - player) - goal_distance(pitch, player))
    off_centre = abs(x - CENTER_X)
    if mine <= CENTRE_ZONE_ROWS:
        score -= CENTRE_WEIGHT * off_centre
    if theirs <= CENTRE_ZONE_ROWS:
        score += CENTRE_WEIGHT * off_centre
    return score


def order_key(turn, mover):
    _, end, outcome = turn
    if outcome == WON:
        return WIN
    if outcome == LOST:
        return -WIN
    return -rows_to_goal(mover, end)


def sort_turns(turns, mover):
    """Stable insertion sort, best first (identical in the 4 languages)."""
    keys = [order_key(t, mover) for t in turns]
    for i in range(1, len(turns)):
        item, key, j = turns[i], keys[i], i - 1
        while j >= 0 and keys[j] < key:
            turns[j + 1], keys[j + 1] = turns[j], keys[j]
            j -= 1
        turns[j + 1], keys[j + 1] = item, key


class Search:
    def __init__(self, node_limit):
        self.node_limit = node_limit
        self.nodes = 0
        self.aborted = False
        self.last_depth = 0

    def child(self, pitch, turn):
        nxt = pitch.copy()
        nxt.apply(turn[0])
        return nxt

    def negamax(self, pitch, player, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > self.node_limit:
            self.aborted = True
            return 0
        if depth == 0:
            return evaluate(pitch, player)
        turns = generate_turns(pitch, player)
        if not turns:
            return -(WIN - ply)
        sort_turns(turns, player)
        best = -INF
        for turn in turns:
            if turn[2] == WON:
                return WIN - ply
            if turn[2] == LOST:
                value = -(WIN - ply)
            else:
                value = -self.negamax(self.child(pitch, turn), 1 - player, depth - 1, -beta, -alpha, ply + 1)
                if self.aborted:
                    return 0
            if value > best:
                best = value
            if value > alpha:
                alpha = value
            if alpha >= beta:
                break
        return best

    def run(self, pitch, player):
        turns = generate_turns(pitch, player)
        sort_turns(turns, player)
        best = turns[0]
        if best[2] == WON:
            return best
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_value, best_index = -INF, -INF, 0
            for i, turn in enumerate(turns):
                if turn[2] == LOST:
                    value = -WIN
                else:
                    value = -self.negamax(self.child(pitch, turn), 1 - player, depth - 1, -INF, -alpha, 1)
                if self.aborted:
                    break
                if value > best_value:
                    best_value, best_index = value, i
                if value > alpha:
                    alpha = value
            if self.aborted:
                break
            best = turns[best_index]
            self.last_depth = depth
            turns.insert(0, turns.pop(best_index))
            if best_value >= WIN - MAX_DEPTH or best_value <= -(WIN - MAX_DEPTH):
                break
        return best


def main():
    env = os.environ.get("CG_NODES")
    line = sys.stdin.readline()
    if not line.strip():
        return
    me = int(line.split()[0])
    pitch = Pitch()
    first_turn = True
    while True:
        if not sys.stdin.readline():
            break
        opponent_move = sys.stdin.readline().strip()
        if opponent_move != "-":
            pitch.apply(opponent_move)
        budget = int(env) if env else (FIRST_TURN_NODES if first_turn else DEFAULT_NODES)
        search = Search(budget)
        digits = search.run(pitch, me)[0]
        pitch.apply(digits)
        sys.stderr.write("d=%d n=%d\n" % (search.last_depth, search.nodes))
        sys.stdout.write(digits + "\n")
        sys.stdout.flush()
        first_turn = False


if __name__ == "__main__":
    main()
