#!/usr/bin/env python3
"""Penguins fuzz: random boards (narrow/wide rows), penguins and actions.

Bots only score the listed actions, so actions are random positions for
random penguin ids (ours and unknown), plus malformed tokens.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
ROWS = 8


def pos(rng):
    return chr(65 + rng.randrange(8)) + str(rng.randint(1, ROWS))


def make_case(rng):
    players = rng.choice([2, 2, 3, 4])
    me = rng.randrange(players)
    per = {2: 4, 3: 3, 4: 2}[players]
    lines = [str(players), str(me), str(per)]
    for _ in range(rng.randint(1, 4)):
        lines.append(str(ROWS))
        for r in range(ROWS):
            width = 7 if r % 2 == 0 else 8
            lines.append("".join(rng.choice("0123") for _ in range(width)))
        lines += [str(rng.randint(0, 5000)) for _ in range(players)]
        penguins = ["%d %d %s" % (i, o, pos(rng)) for o in range(players) for i in range(per) if rng.random() < 0.8]
        lines.append(str(len(penguins)))
        lines += penguins
        actions = ["MOVE %d %s" % (rng.randrange(per), pos(rng)) for _ in range(rng.randint(0, 15))]
        if rng.random() < 0.1:
            actions.append("MOVE x Z99")
        lines.append(str(len(actions)))
        lines += actions
    return "\n".join(lines) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(CASES):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{CASES} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
