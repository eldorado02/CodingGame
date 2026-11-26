#!/usr/bin/env python3
"""LOCM Constructed fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is the constructed turn (120 random cards, mana 0) followed by
0..15 random battle snapshots: hand 0..8, boards 0..3 per lane, random
abilities (guards, wards, lethal), items of every colour, areas, opponent
action lines. The bot keeps no memory between turns.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


LETTERS = "BCDGLW"
MAX_BATTLE_TURNS = 15


def abilities(rng):
    return "".join(ch if rng.random() < 0.2 else "-" for ch in LETTERS)


def card(rng, number, iid, location, lane):
    kind = rng.choice((0, 0, 0, 1, 2, 3))
    cost = rng.randint(0, 12)
    if kind == 0:
        attack, defense = rng.randint(0, 9), rng.randint(1, 9)
    else:
        attack, defense = rng.randint(-3, 3), rng.randint(-6, 4)
    return "%d %d %d %d %d %d %d %s %d %d %d %d %d" % (
        number, iid, location, kind if location == 0 else 0, cost, attack, defense, abilities(rng),
        rng.choice((0, 0, rng.randint(-3, 3))), rng.choice((0, 0, rng.randint(-3, 3))),
        rng.choice((0, 0, 1, 2)), rng.choice((0, 0, 0, 1, 2)), lane)


def header(rng, mana):
    return ["%d %d %d %d" % (rng.randint(1, 40), mana, rng.randint(0, 30), rng.randint(0, 2)),
            "%d %d %d %d" % (rng.randint(1, 40), rng.randint(0, 12), rng.randint(0, 30), rng.randint(0, 2))]


def make_case(rng):
    out = header(rng, 0) + ["0 0", "120"]
    out += [card(rng, n, -1, 0, -1) for n in range(1, 121)]
    for _ in range(rng.randint(0, MAX_BATTLE_TURNS)):
        out += header(rng, rng.randint(1, 13))
        actions = rng.randint(0, 3)
        out.append("%d %d" % (rng.randint(0, 8), actions))
        out += ["%d SUMMON %d %d" % (rng.randint(1, 160), rng.randint(1, 60), rng.randint(0, 1)) for _ in range(actions)]
        cards, iid = [], 1
        for _ in range(rng.randint(0, 8)):
            cards.append(card(rng, rng.randint(1, 160), iid, 0, -1))
            iid += 1
        for location in (1, -1):
            for lane in (0, 1):
                for _ in range(rng.randint(0, 3)):
                    cards.append(card(rng, rng.randint(1, 160), iid, location, lane))
                    iid += 1
        rng.shuffle(cards)
        out.append(str(len(cards)))
        out += cards
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
