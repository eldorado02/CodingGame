#!/usr/bin/env python3
"""Oware Abapa — Python reference (template board-2p), v1 alpha-beta.

Rules from the referee (github.com/MaximeCheramy/cg-oware Referee.java):
  * a house is playable if not empty and (opponent has seeds or the sowing
    reaches the opponent side: seeds >= 6 - house);
  * sowing skips the origin house; captures go backwards from the last
    house while on the opponent side with 2 or 3 seeds; capturing every
    opponent seed (grand slam) cancels the capture;
  * score > 24 wins; a side without legal move takes all remaining seeds
    and the game ends; 200 turns max (most seeds wins).
State is relative: houses[0..5] belong to the side to move.
Output: the RELATIVE house 0..5 (the referee adds 6 for player 1, Player.getAction).
"""
import os
import sys

HOUSES = 12
SIDE = 6
WIN_SCORE = 24          # strictly more than this wins
MAX_TURNS = 200
SCORE_WEIGHT = 4        # captured seed vs seed still on our side
TURN_NODES = 60000


class State:
    __slots__ = ("h", "mine", "theirs", "turn")

    def __init__(self, h, mine, theirs, turn):
        self.h = h            # houses, first 6 = side to move
        self.mine = mine      # score of the side to move
        self.theirs = theirs
        self.turn = turn      # turns already played (referee counter)


def legal_moves(s):
    opponent_has_seeds = any(s.h[SIDE:])
    return [i for i in range(SIDE) if s.h[i] > 0 and (opponent_has_seeds or s.h[i] >= SIDE - i)]


def apply_move(s, i):
    """Returns the next state from the opponent's point of view."""
    h = s.h[:]
    seeds = h[i]
    h[i] = 0
    pos = i
    while seeds:
        pos = (pos + 1) % HOUSES
        if pos == i:
            continue
        h[pos] += 1
        seeds -= 1
    captured_houses = []
    cur = pos
    while cur >= SIDE and h[cur] in (2, 3):
        captured_houses.append(cur)
        cur -= 1
    grand_slam = all(h[j] == 0 or j in captured_houses for j in range(SIDE, HOUSES))
    gain = 0
    if not grand_slam:
        for j in captured_houses:
            gain += h[j]
            h[j] = 0
    rotated = h[SIDE:] + h[:SIDE]
    return State(rotated, s.theirs, s.mine + gain, s.turn + 1)


def evaluate(s):
    return SCORE_WEIGHT * (s.mine - s.theirs) + sum(s.h[:SIDE]) - sum(s.h[SIDE:])


# ------------------------------------------------------------- ENGINE
WIN = 1000000
INF = 1 << 30
MAX_DEPTH = 64
NODE_LIMIT = int(os.environ.get("CG_NODES", str(TURN_NODES)))


class Aborted(Exception):
    pass


def final_score(mine, theirs, ply):
    if mine > theirs:
        return WIN - ply
    if mine < theirs:
        return -(WIN - ply)
    return 0


class Search:
    def __init__(self):
        self.nodes = 0
        self.depth = 0

    def negamax(self, s, depth, alpha, beta, ply):
        self.nodes += 1
        if self.nodes > NODE_LIMIT:
            raise Aborted
        if s.theirs > WIN_SCORE:
            return -(WIN - ply)
        if s.mine > WIN_SCORE:
            return WIN - ply
        mv = legal_moves(s)
        if not mv:
            return final_score(s.mine + sum(s.h), s.theirs, ply)
        if s.turn >= MAX_TURNS:
            return final_score(s.mine, s.theirs, ply)
        if depth == 0:
            return evaluate(s)
        best = -INF
        for m in mv:
            v = -self.negamax(apply_move(s, m), depth - 1, -beta, -alpha, ply + 1)
            if v > best:
                best = v
            if v > alpha:
                alpha = v
            if alpha >= beta:
                break
        return best

    def run(self, root):
        mv = legal_moves(root)
        best = mv[0]
        self.nodes = 0
        for depth in range(1, MAX_DEPTH + 1):
            alpha, best_v, cur_idx = -INF, -INF, 0
            try:
                for i, m in enumerate(mv):
                    v = -self.negamax(apply_move(root, m), depth - 1, -INF, -alpha, 1)
                    if v > best_v:
                        best_v, cur_idx = v, i
                    if v > alpha:
                        alpha = v
            except Aborted:
                break
            best = mv[cur_idx]
            self.depth = depth
            mv.insert(0, mv.pop(cur_idx))   # PV first next iteration
            if best_v >= WIN - MAX_DEPTH or best_v <= -(WIN - MAX_DEPTH):
                break
        return best


# ------------------------------------------------------------- I/O
TOTAL_SEEDS = 48


def main():
    me_id = None
    my_score = their_score = 0
    seeds_after_my_move = TOTAL_SEEDS
    my_turns = 0
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        h = [int(v) for v in line.split()[:HOUSES]]
        if len(h) < HOUSES:
            return
        if me_id is None:
            me_id = 0 if all(v == 4 for v in h) else 1
        # the opponent's captures are the seeds that vanished since our move
        their_score += seeds_after_my_move - sum(h)
        turn = 2 * my_turns + me_id
        s = State(h, my_score, their_score, turn)
        mv = legal_moves(s)
        if not mv:
            print(0, flush=True)
            continue
        m = Search().run(s)
        nxt = apply_move(s, m)
        my_score = nxt.theirs
        seeds_after_my_move = sum(nxt.h)
        my_turns += 1
        print(m, flush=True)   # relative house: the referee adds 6 for player 1


if __name__ == "__main__":
    main()
