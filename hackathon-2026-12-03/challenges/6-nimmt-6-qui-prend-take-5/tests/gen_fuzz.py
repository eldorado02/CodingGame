#!/usr/bin/env python3
"""6 nimmt! fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case simulates 1..5 rounds of a 4-player game with random players
(our own choices included), so the stream stays valid whatever the bot
answers. Covers: forced line picks, full rows, disqualified players (-999,
last card -1), the last card of a round.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


PLAYERS = 4
HAND = 10
ROW_LIMIT = 5
DISQUALIFY_RATE = 0.05


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


def snapshot(phase, last, rows, scores, hand):
    """One turn as the real referee sends it: the phase comes last."""
    out = [" ".join(map(str, last))]
    for row in rows:
        out += [str(len(row)), " ".join(map(str, row))]
    out += [" ".join(map(str, scores)), str(len(hand)), " ".join(map(str, hand)), phase]
    return out


def place(rng, card, owner, me, rows, scores, last, hands, out):
    """Puts one card on the table; a too-small card of ours asks for a line."""
    fits = [i for i, r in enumerate(rows) if r[-1] < card]
    if not fits:
        if owner == me:
            out += snapshot("CHOOSE_LINE_TO_PICK", last, rows, scores, hands[me])
        i = rng.randrange(len(rows))
        scores[owner] += sum(cows(c) for c in rows[i])
        rows[i] = [card]
        return
    i = max(fits, key=lambda k: rows[k][-1])
    if len(rows[i]) == ROW_LIMIT:
        scores[owner] += sum(cows(c) for c in rows[i])
        rows[i] = [card]
    else:
        rows[i].append(card)


def make_case(rng):
    me = rng.randrange(PLAYERS)
    out = ["%d %d" % (PLAYERS, me)]
    scores = [0] * PLAYERS
    out_of_game = set()
    for _ in range(rng.randint(1, 5)):
        deck = list(range(1, 105))
        rng.shuffle(deck)
        hands = [sorted(deck[k * HAND:(k + 1) * HAND]) for k in range(PLAYERS)]
        rows = [[c] for c in deck[PLAYERS * HAND:PLAYERS * HAND + 4]]
        last = [-1] * PLAYERS
        for _ in range(HAND):
            out += snapshot("CHOOSE_CARD_TO_PLAY", last, rows, scores, hands[me])
            for k in range(PLAYERS):
                if k != me and k not in out_of_game and rng.random() < DISQUALIFY_RATE:
                    out_of_game.add(k)
                    scores[k] = -999
            played = []
            for k in range(PLAYERS):
                if k in out_of_game:
                    last[k] = -1
                    continue
                card = rng.choice(hands[k])
                hands[k].remove(card)
                last[k] = card
                played.append((card, k))
            for card, k in sorted(played):
                place(rng, card, k, me, rows, scores, last, hands, out)
    return "\n".join(out) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        rng = random.Random(i)
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(rng))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
