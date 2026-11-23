#!/usr/bin/env python3
"""Checkers fuzz: streams of 1..5 snapshot turns.

Positions come from seeded random playouts (captures favoured) with the
reference rules; the legal list is the reference generator's, shuffled.
Colour line first, then per turn 8 board lines + move count + moves.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
INIT = [".r.r.r.r", "r.r.r.r.", ".r.r.r.r", "........", "........", "b.b.b.b.", ".b.b.b.b", "b.b.b.b."]
SYM = ".bBrR"


def snapshot(rng, side):
    while True:
        s = bot.State([bot.CHAR[ch] for row in INIT for ch in row], bot.SIDE_B)
        for _ in range(rng.randint(0, 60)):
            mv = bot.gen_moves(s)
            if not mv:
                break
            longest = max(len(m) for m in mv)
            pick = [m for m in mv if len(m) == longest] if rng.random() < 0.5 else mv
            s = bot.apply_move(s, rng.choice(pick))
        s.side = side
        mv = bot.gen_moves(s)
        if mv:
            return s, mv


def make_case(rng):
    side = bot.SIDE_B if rng.random() < 0.5 else bot.SIDE_R
    lines = ["b" if side == bot.SIDE_B else "r"]
    for _ in range(rng.randint(1, 5)):
        s, mv = snapshot(rng, side)
        lines += ["".join(SYM[v] for v in s.cell[y * 8:y * 8 + 8]) for y in range(8)]
        names = [bot.name_of(m) for m in mv]
        rng.shuffle(names)
        lines.append(str(len(names)))
        lines += names
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
