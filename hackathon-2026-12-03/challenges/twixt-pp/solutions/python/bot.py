#!/usr/bin/env python3
"""TwixT-PP 12x12 (CodinGame) — Python reference of the differential fuzz.

Coordinates: "D6" = column D (x = 3), row 6 (y = 5); cell = y * 12 + x.
The first player ("FIRST" on turn 1) links the top and bottom rows and may
not play in columns A / L; the second player links left and right.
Corners are never playable. Links are automatic (knight move, not crossing
an opponent link); in the PP variant own links may cross each other.

Algorithm (v1): evaluation = knight-path distance (0-1 BFS: an own peg
costs 0, an empty hole costs 1, a segment crossing an opponent link is cut)
of the opponent minus ours. Two-ply search: every move is scored at one
ply, the best TOP_K are re-scored against every opponent reply.
TOP_K comes from env CG_NODES so the fuzz stays fast in Python.
"""
import os
import sys
from collections import deque

SIZE = 12
CELLS = SIZE * SIZE
EMPTY, ME, OPP = 0, 1, 2
TOP_BOTTOM, LEFT_RIGHT = 0, 1
KNIGHT_DELTAS = ((1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2))
UNREACHABLE = 99
WIN_SCORE = 1000000
DISTANCE_WEIGHT = 100     # one peg of path length outweighs every link count
LINK_WEIGHT = 3           # more links = more alternative routes
DEFAULT_TOP_K = 16        # 2-ply candidates on CodinGame (300 ms per turn)
SWAP_ZONE = (3, 8)        # swap an opening peg whose x and y are in this range
OPENING_CELL = 3 * SIZE + 5   # "F4": central column, not worth swapping much


def cell_of(x, y):
    return y * SIZE + x


def name_of(cell):
    return "%s%d" % (chr(ord("A") + cell % SIZE), cell // SIZE + 1)


def parse_cell(text):
    return cell_of(ord(text[0]) - ord("A"), int(text[1:]) - 1)


def transpose(cell):
    return cell_of(cell // SIZE, cell % SIZE)


def is_corner(x, y):
    return x in (0, SIZE - 1) and y in (0, SIZE - 1)


def _orientation(a, b, c):
    """Sign of the cross product (b - a) x (c - a)."""
    v = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
    return (v > 0) - (v < 0)


def _segments_cross(s, t):
    """Proper crossing of two segments that share no endpoint."""
    if s[0] in t or s[1] in t:
        return False
    p = [(c % SIZE, c // SIZE) for c in s + t]
    return (_orientation(p[0], p[1], p[2]) * _orientation(p[0], p[1], p[3]) < 0
            and _orientation(p[2], p[3], p[0]) * _orientation(p[2], p[3], p[1]) < 0)


def _bounding_boxes_overlap(s, t):
    """Cheap filter before the exact crossing test."""
    sx = [c % SIZE for c in s]
    sy = [c // SIZE for c in s]
    tx = [c % SIZE for c in t]
    ty = [c // SIZE for c in t]
    return (min(sx) <= max(tx) and min(tx) <= max(sx)
            and min(sy) <= max(ty) and min(ty) <= max(sy))


def _build_geometry():
    """All knight segments, each cell's (neighbour, segment) list, and the
    segments each segment crosses."""
    segments, seg_id = [], {}
    neighbours = [[] for _ in range(CELLS)]
    for cell in range(CELLS):
        x, y = cell % SIZE, cell // SIZE
        for dx, dy in KNIGHT_DELTAS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < SIZE and 0 <= ny < SIZE:
                other = cell_of(nx, ny)
                key = (min(cell, other), max(cell, other))
                if key not in seg_id:
                    seg_id[key] = len(segments)
                    segments.append(key)
                neighbours[cell].append((other, seg_id[key]))
    crosses = [[] for _ in segments]
    for i, s in enumerate(segments):
        for j in range(i + 1, len(segments)):
            t = segments[j]
            if _bounding_boxes_overlap(s, t) and _segments_cross(s, t):
                crosses[i].append(j)
                crosses[j].append(i)
    return segments, seg_id, neighbours, crosses


SEGMENTS, SEG_ID, NEIGHBOURS, CROSSES = _build_geometry()


class State:
    """Board seen from our side: board[cell] in {EMPTY, ME, OPP};
    linked[p][seg] = 1 if p owns that link; cut[p][seg] = number of
    opponent links crossing seg (seg unusable for p when > 0)."""

    def __init__(self, my_side):
        self.side = {ME: my_side, OPP: 1 - my_side}
        self.board = [EMPTY] * CELLS
        self.linked = {ME: bytearray(len(SEGMENTS)), OPP: bytearray(len(SEGMENTS))}
        self.cut = {ME: [0] * len(SEGMENTS), OPP: [0] * len(SEGMENTS)}
        self.link_count = {ME: 0, OPP: 0}

    def copy(self):
        s = State.__new__(State)
        s.side = self.side
        s.board = self.board[:]
        s.linked = {p: self.linked[p][:] for p in (ME, OPP)}
        s.cut = {p: self.cut[p][:] for p in (ME, OPP)}
        s.link_count = dict(self.link_count)
        return s

    def add_link(self, player, seg):
        if self.linked[player][seg]:
            return
        self.linked[player][seg] = 1
        self.link_count[player] += 1
        other = OPP if player == ME else ME
        for crossed in CROSSES[seg]:
            self.cut[other][crossed] += 1

    def place(self, player, cell):
        """Drop a peg and create every legal link, as the referee does."""
        self.board[cell] = player
        for other, seg in NEIGHBOURS[cell]:
            if self.board[other] == player and self.cut[player][seg] == 0:
                self.add_link(player, seg)

    def can_play(self, player, cell):
        if self.board[cell] != EMPTY:
            return False
        x, y = cell % SIZE, cell // SIZE
        if is_corner(x, y):
            return False
        if self.side[player] == TOP_BOTTOM:
            return x not in (0, SIZE - 1)
        return y not in (0, SIZE - 1)

    def legal_moves(self, player):
        return [c for c in range(CELLS) if self.can_play(player, c)]


def _on_start_border(side, cell):
    return (cell // SIZE == 0) if side == TOP_BOTTOM else (cell % SIZE == 0)


def _on_goal_border(side, cell):
    return (cell // SIZE == SIZE - 1) if side == TOP_BOTTOM else (cell % SIZE == SIZE - 1)


def path_distance(state, player):
    """Fewest pegs `player` still needs to connect his borders (0-1 BFS)."""
    side = state.side[player]
    board, cut, linked = state.board, state.cut[player], state.linked[player]
    dist = [UNREACHABLE] * CELLS
    queue = deque()
    for cell in range(CELLS):
        if not _on_start_border(side, cell):
            continue
        if board[cell] == player:
            dist[cell] = 0
            queue.appendleft(cell)
        elif state.can_play(player, cell):
            dist[cell] = 1
            queue.append(cell)
    while queue:
        cell = queue.popleft()
        d = dist[cell]
        if _on_goal_border(side, cell):
            return d
        for other, seg in NEIGHBOURS[cell]:
            if cut[seg]:
                continue
            owner = board[other]
            if owner == player:
                if board[cell] == player and not linked[seg]:
                    continue  # two own pegs not linked: an old opponent link cuts them
                if d < dist[other]:
                    dist[other] = d
                    queue.appendleft(other)
            elif owner == EMPTY and state.can_play(player, other) and d + 1 < dist[other]:
                dist[other] = d + 1
                queue.append(other)
    return UNREACHABLE


def evaluate(state):
    """Score from our point of view; WIN_SCORE when someone is connected."""
    mine = path_distance(state, ME)
    if mine == 0:
        return WIN_SCORE
    theirs = path_distance(state, OPP)
    if theirs == 0:
        return -WIN_SCORE
    links = state.link_count[ME] - state.link_count[OPP]
    return DISTANCE_WEIGHT * (theirs - mine) + LINK_WEIGHT * links


def _stable_sort_desc(items, keys):
    """Insertion sort on integer keys, descending, stable (same in 4 languages)."""
    for i in range(1, len(items)):
        item, key, j = items[i], keys[i], i - 1
        while j >= 0 and keys[j] < key:
            items[j + 1], keys[j + 1] = items[j], keys[j]
            j -= 1
        items[j + 1], keys[j + 1] = item, key


def _worst_reply(state, floor):
    """Lowest score the opponent can force; stops once it is <= floor."""
    worst = WIN_SCORE
    for cell in state.legal_moves(OPP):
        child = state.copy()
        child.place(OPP, cell)
        score = evaluate(child)
        if score < worst:
            worst = score
            if worst <= floor:
                break
    return worst


def choose_move(state, top_k):
    """Best cell for us: 1-ply ranking, then 2-ply on the top_k."""
    moves = state.legal_moves(ME)
    scores = []
    for cell in moves:
        child = state.copy()
        child.place(ME, cell)
        scores.append(evaluate(child))
    _stable_sort_desc(moves, scores)
    if scores[0] == WIN_SCORE:
        return moves[0], "win"
    best_cell, best_score = moves[0], -WIN_SCORE - 1
    for cell in moves[:top_k]:
        child = state.copy()
        child.place(ME, cell)
        score = _worst_reply(child, best_score)
        if score > best_score:
            best_cell, best_score = cell, score
    return best_cell, "score=%d" % best_score


def should_swap(cell):
    x, y = cell % SIZE, cell // SIZE
    low, high = SWAP_ZONE
    return low <= x <= high and low <= y <= high


def read_state(read, my_side):
    """Rebuild the state from one turn of input (pegs, then links)."""
    state = State(my_side)
    for player in (ME, OPP):
        for _ in range(int(read())):
            state.board[parse_cell(read().split()[0])] = player
        for _ in range(int(read())):
            a, b = (parse_cell(t) for t in read().split()[:2])
            state.add_link(player, SEG_ID[(min(a, b), max(a, b))])
    return state


def decide(state, last, first_turn, top_k):
    my_pegs = state.board.count(ME)
    their_pegs = state.board.count(OPP)
    if last == "FIRST":
        return name_of(OPENING_CELL), "opening"
    if first_turn and my_pegs == 0 and their_pegs == 1 and should_swap(state.board.index(OPP)):
        return "SWAP", "swap"
    if not state.legal_moves(ME):
        return name_of(0), "no move"
    cell, info = choose_move(state, top_k)
    return name_of(cell), info


def main():
    env = os.environ.get("CG_NODES")
    top_k = int(env) if env else DEFAULT_TOP_K

    def read():
        return sys.stdin.readline().strip()

    my_side = None
    while True:
        last = read()
        if not last:
            break
        first_turn = my_side is None
        if first_turn:
            my_side = TOP_BOTTOM if last == "FIRST" else LEFT_RIGHT
        state = read_state(read, my_side)
        move, info = decide(state, last, first_turn, top_k)
        sys.stderr.write(info + "\n")
        sys.stdout.write(move + "\n")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
