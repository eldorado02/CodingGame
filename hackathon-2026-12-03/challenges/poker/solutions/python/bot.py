#!/usr/bin/env python3
"""Poker (Texas hold'em) — Python reference of the differential fuzz, v2 Monte Carlo equity.

Cards "9H", "TS" ("X" unknown); possible actions CALL, CHECK, FOLD, ALL-IN,
BET_x (x = minimum raise; we answer "BET x").
Equity: SIMULATIONS deals of the unknown cards (opponents' hands and the rest
of the board) with a deterministic xorshift32 seeded by the round, the same
in every language; a 7-card evaluator ranks category then kickers.
Action: equity >= ALL_IN_EQUITY all-in; >= RAISE_EQUITY minimum raise; free
to check: check; call when equity beats the pot odds; else fold. Short stack
(<= SHORT_STACK_BLINDS big blinds) with equity >= PUSH_EQUITY: all-in.
"""
import os
import sys

RANKS = "23456789TJQKA"
SUITS = "HDSC"
HIGH_CARD, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL_HOUSE, QUADS, STRAIGHT_FLUSH = range(9)
SIMULATIONS = int(os.environ.get("CG_NODES", "400"))
ALL_IN_PERCENT = 80
RAISE_PERCENT = 62
PUSH_PERCENT = 50
SHORT_STACK_BLINDS = 10
BOARD_SIZE = 5
MASK = 0xFFFFFFFF
SEED_SALT = 2654435761
BLIND_CAP = 10 ** 9           # blinds keep doubling: stay in 64 bits


class Rng:
    """xorshift32: identical sequence in every language."""

    def __init__(self, seed):
        self.state = (seed & MASK) or 1

    def next(self):
        x = self.state
        x ^= (x << 13) & MASK
        x ^= x >> 17
        x ^= (x << 5) & MASK
        self.state = x
        return x


def parse(cards):
    """'AD_QH_X' -> [card index = rank * 4 + suit] without unknown cards."""
    out = []
    for c in cards.split("_"):
        if len(c) == 2 and c[0] in RANKS and c[1] in SUITS:
            out.append(RANKS.index(c[0]) * 4 + SUITS.index(c[1]))
    return out


def straight_top(present):
    """Highest straight top rank among the present ranks (wheel = 3), or -1."""
    for top in range(12, 3, -1):
        if all(present[top - k] for k in range(5)):
            return top
    if present[12] and all(present[k] for k in range(4)):
        return 3
    return -1


def encode(category, kickers):
    value = category
    for k in (kickers + [0] * 5)[:5]:
        value = value * 16 + k
    return value


def evaluate(cards):
    """Comparable value of the best 5-card hand among the cards."""
    counts = [0] * 13
    by_suit = [[False] * 13 for _ in range(4)]
    for c in cards:
        counts[c // 4] += 1
        by_suit[c % 4][c // 4] = True
    for suit in range(4):
        if sum(by_suit[suit]) >= 5:
            top = straight_top(by_suit[suit])
            if top >= 0:
                return encode(STRAIGHT_FLUSH, [top])
    ranks_desc = [r for r in range(12, -1, -1) if counts[r]]
    quads = [r for r in ranks_desc if counts[r] == 4]
    trips = [r for r in ranks_desc if counts[r] == 3]
    pairs = [r for r in ranks_desc if counts[r] == 2]
    if quads:
        return encode(QUADS, [quads[0]] + [r for r in ranks_desc if r != quads[0]][:1])
    if trips and (len(trips) > 1 or pairs):
        second = max(trips[1:] + pairs)
        return encode(FULL_HOUSE, [trips[0], second])
    for suit in range(4):
        if sum(by_suit[suit]) >= 5:
            return encode(FLUSH, [r for r in range(12, -1, -1) if by_suit[suit][r]][:5])
    top = straight_top([counts[r] > 0 for r in range(13)])
    if top >= 0:
        return encode(STRAIGHT, [top])
    if trips:
        return encode(TRIPS, [trips[0]] + [r for r in ranks_desc if r != trips[0]][:2])
    if len(pairs) >= 2:
        kicker = [r for r in ranks_desc if r not in pairs[:2]][:1]
        return encode(TWO_PAIR, pairs[:2] + kicker)
    if pairs:
        return encode(PAIR, [pairs[0]] + [r for r in ranks_desc if r != pairs[0]][:3])
    return encode(HIGH_CARD, ranks_desc[:5])


def equity(hole, board, opponents, seed):
    """(twice the wins + ties, twice the simulations)."""
    rng = Rng(seed)
    known = set(hole + board)
    deck = [c for c in range(52) if c not in known]
    need = BOARD_SIZE - len(board) + 2 * opponents
    score = 0
    for _ in range(SIMULATIONS):
        for k in range(need):
            j = k + rng.next() % (len(deck) - k)
            deck[k], deck[j] = deck[j], deck[k]
        full_board = board + deck[:BOARD_SIZE - len(board)]
        ours = evaluate(hole + full_board)
        best = 0
        for o in range(opponents):
            start = BOARD_SIZE - len(board) + 2 * o
            best = max(best, evaluate(deck[start:start + 2] + full_board))
        score += 2 if ours > best else 1 if ours == best else 0
    return score, 2 * SIMULATIONS


def big_blind(start, multiplier, per_level, hand_nb):
    blind = start
    for _ in range((hand_nb - 1) // max(per_level, 1)):
        blind = min(blind * multiplier, BLIND_CAP)
    return blind


def decide(score, total, possible, to_call, pot, stack, big_blind):
    names = {}
    for p in possible:
        names.setdefault(p.split("_")[0], p)
    if "ALL-IN" in names and (score * 100 >= ALL_IN_PERCENT * total or
                              (stack <= SHORT_STACK_BLINDS * big_blind and score * 100 >= PUSH_PERCENT * total)):
        return "ALL-IN"
    if score * 100 >= RAISE_PERCENT * total and "BET" in names:
        return names["BET"].replace("_", " ")
    if "CHECK" in names:
        return "CHECK"
    if "CALL" in names and score * (pot + to_call) > total * to_call:
        return "CALL"
    if "FOLD" in names:
        return "FOLD"
    return possible[0].replace("_", " ") if possible else "FOLD"


def main():
    read = sys.stdin.readline
    init = [read() for _ in range(8)]
    if not init[-1].strip():
        return
    _, big, per_level, multiplier, _, _, players, me = (int(v) for v in init)
    while True:
        line = read()
        if not line.strip():
            return
        round_nb, hand_nb = int(line), int(read())
        seats = [tuple(int(v) for v in read().split()[:2]) for _ in range(players)]
        board, hole = parse(read().strip()), parse(read().strip())
        for _ in range(int(read())):
            read()
        for _ in range(int(read())):
            read()
        possible = [read().strip() for _ in range(int(read()))]
        to_call = max(s[1] for s in seats) - seats[me][1]
        pot = sum(s[1] for s in seats)
        opponents = max(1, sum(1 for k, s in enumerate(seats) if k != me and s[0] + s[1] > 0))
        if len(hole) == 2:
            score, total = equity(hole, board, opponents, round_nb * SEED_SALT + hand_nb)
        else:
            score, total = 0, 1
        blind = big_blind(big, multiplier, per_level, hand_nb)
        print(decide(score, total, possible, to_call, pot, seats[me][0], blind), flush=True)


if __name__ == "__main__":
    main()
