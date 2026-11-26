#!/usr/bin/env python3
"""Poker fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: the 8 init lines (2..4 players), then 1..12 random turns: stacks
and chips in the pot, a board of 0/3/4/5 known cards, our two cards (drawn
without duplicates), past actions and showdowns (skipped by the bot) and a
random subset of the possible actions. Run with a small CG_NODES (the Python
evaluator is slow).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


RANKS = "23456789TJQKA"
SUITS = "HDSC"
ACTIONS = ("CALL", "CHECK", "FOLD", "ALL-IN")
MAX_TURNS = 12


def card(index):
    return RANKS[index // 4] + SUITS[index % 4]


def make_case(rng):
    players = rng.randint(2, 4)
    me = rng.randrange(players)
    out = ["5", "10", "10", "2", str(4800 // players), str(rng.randrange(players)), str(players), str(me)]
    for turn in range(rng.randint(1, MAX_TURNS)):
        out += [str(turn * 2 + 1), str(rng.randint(1, 80))]
        for _ in range(players):
            out.append("%d %d" % (rng.choice((0, rng.randint(0, 4800))), rng.randint(0, 400)))
        cards = rng.sample(range(52), 7)
        known = rng.choice((0, 3, 4, 5))
        out.append("_".join([card(c) for c in cards[:known]] + ["X"] * (5 - known)))
        out.append("_".join(card(c) for c in cards[5:7]) if rng.random() < 0.95 else "X_X")
        actions = rng.randint(0, 3)
        out.append(str(actions))
        out += ["%d 1 %d BET_20 X_X_X_X_X" % (turn, rng.randrange(players)) for _ in range(actions)]
        shows = rng.randint(0, 1)
        out.append(str(shows))
        out += ["1 X_X_X_X_X QD_8D_JH_2H" for _ in range(shows)]
        possible = rng.sample(ACTIONS, rng.randint(1, 4)) + (["BET_%d" % rng.randint(10, 500)] if rng.random() < 0.7 else [])
        out.append(str(len(possible)))
        out += possible
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
