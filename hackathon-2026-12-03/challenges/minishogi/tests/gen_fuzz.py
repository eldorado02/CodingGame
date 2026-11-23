#!/usr/bin/env python3
"""Minishogi fuzz: random boards (with promoted tokens), hands, actions.

The v1 bot only scores the given action strings against the board, so
cases mix moves, promotions ('+'), drops ('P*24') and malformed tokens.
DICE (dice-shogi copy) adds the rolled-dice line.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200
DICE = False
PIECES = ["K", "G", "S", "B", "R", "P", "+S", "+B", "+R", "+P"]


def tok(rng):
    if rng.random() < 0.55:
        return "."
    p = rng.choice(PIECES)
    return p if rng.random() < 0.5 else p.lower()


def make_case(rng):
    me = rng.randint(0, 1)
    lines = [str(me), "5"]
    for _ in range(rng.randint(1, 5)):
        for _ in range(5):
            lines.append("".join(tok(rng) for _ in range(5)))
        lines.append(rng.choice(["-", "P", "GS"]))
        lines.append(rng.choice(["-", "p", "rb"]))
        if DICE:
            lines.append(str(rng.randint(1, 6)))
        lines.append(rng.choice(["none", "2112", "G*33"]))
        acts = []
        for _ in range(rng.randint(0, 20)):
            r = rng.random()
            d = lambda: str(rng.randint(1, 5))
            if r < 0.6:
                a = d() + d() + d() + d() + ("+" if rng.random() < 0.2 else "")
            elif r < 0.95:
                a = rng.choice("PGSBRpgsbr") + "*" + d() + d()
            else:
                a = rng.choice(["x", "9999", "P*"])
            acts.append(a)
        lines.append(str(len(acts)))
        lines += acts
    return "\n".join(lines) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
