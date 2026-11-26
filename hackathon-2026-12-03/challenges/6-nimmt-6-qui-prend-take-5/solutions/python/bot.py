#!/usr/bin/env python3
"""6 nimmt! — Python reference of the differential fuzz, v1 expected-penalty greedy.

Card choice: for each card of the hand, the expected number of cows it costs.
- Too small for every row: we pick the cheapest row -> its cows.
- Otherwise it goes on the row whose last card is the greatest below it. The
  row is taken when the other players drop enough cards in between to fill it:
  each of them plays one card, an unseen card of the gap with probability
  gap / unseen, so the count is Binomial(others, gap / unseen).
Ties: smaller cost, then smaller gap, then smaller card.
Line pick: the row with the fewest cows, then the lowest index.
"""
import sys

PHASE_PLAY = "CHOOSE_CARD_TO_PLAY"
DECK_SIZE = 104
HAND_SIZE = 10          # a full hand means a new round: forget the seen cards
ROW_LIMIT = 5           # the sixth card of a row takes the five others
ROWS = 4
DISQUALIFIED_SCORE = -999


def cows(card):
    if card == 55:
        return 7
    if card % 11 == 0:
        return 5
    if card % 10 == 0:
        return 3
    if card % 5 == 0:
        return 2
    return 1


def row_cows(row):
    return sum(cows(c) for c in row)


def target_row(rows, card):
    """Index of the row the card goes on, or -1 when it is below every row."""
    best = -1
    for i, row in enumerate(rows):
        if row[-1] < card and (best < 0 or row[-1] > rows[best][-1]):
            best = i
    return best


def at_least(count, draws, p):
    """P(Binomial(draws, p) >= count), products written out for identical floats."""
    if count <= 0:
        return 1.0
    total = 0.0
    for j in range(count, draws + 1):
        term = 1.0
        for _ in range(j):
            term *= p
        for _ in range(draws - j):
            term *= 1.0 - p
        total += binomial(draws, j) * term
    return total


def binomial(n, k):
    value = 1
    for i in range(k):
        value = value * (n - i) // (i + 1)
    return value


def gap_unseen(seen, low, high):
    return sum(1 for c in range(low + 1, high) if not seen[c])


def play_cost(rows, seen, unseen, others, card):
    """(expected cows, gap) of playing this card now."""
    r = target_row(rows, card)
    if r < 0:
        return float(min(row_cows(row) for row in rows)), 0
    gap = gap_unseen(seen, rows[r][-1], card)
    p = gap / unseen if unseen else 0.0
    slots = ROW_LIMIT - len(rows[r])
    return at_least(slots, others, p) * row_cows(rows[r]), gap


def choose_card(rows, hand, seen, others):
    unseen = sum(1 for c in range(1, DECK_SIZE + 1) if not seen[c])
    best, best_key = hand[0], None
    for card in sorted(hand):
        cost, gap = play_cost(rows, seen, unseen, others, card)
        key = (cost, gap, card)
        if best_key is None or key < best_key:
            best, best_key = card, key
    return best


def choose_row(rows):
    best = 0
    for i in range(1, len(rows)):
        if row_cows(rows[i]) < row_cows(rows[best]):
            best = i
    return best


def read_ints(line):
    return [int(v) for v in line.split()]


def main():
    read = sys.stdin.readline
    first = read()
    if not first:
        return
    player_count = read_ints(first)[0]
    seen = [False] * (DECK_SIZE + 2)
    # The real referee sends the phase as the LAST line of a turn (the statement
    # lists it first): last cards, rows, scores, hand size, hand, phase.
    while True:
        line = read()
        if not line:
            return
        last_played = read_ints(line)
        rows = []
        for _ in range(ROWS):
            read()
            rows.append(read_ints(read()) or [0])
        scores = read_ints(read())
        read()
        hand = read_ints(read())
        phase = read().strip()
        if phase == PHASE_PLAY and len(hand) == HAND_SIZE:
            seen = [False] * (DECK_SIZE + 2)
        for card in last_played + hand + [c for row in rows for c in row]:
            if 1 <= card <= DECK_SIZE:
                seen[card] = True
        active = sum(1 for s in scores[:player_count] if s != DISQUALIFIED_SCORE)
        others = max(0, active - 1)
        if phase == PHASE_PLAY and hand:
            print("PLAY %d" % choose_card(rows, hand, seen, others), flush=True)
        else:
            print("PICK %d" % choose_row(rows), flush=True)


if __name__ == "__main__":
    main()
