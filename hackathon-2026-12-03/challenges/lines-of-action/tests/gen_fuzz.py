#!/usr/bin/env python3
"""Lines of Action fuzz: streams of 1..4 snapshot turns from random playouts.

Standard opening, random legal moves (no winner yet), our colour to move;
the legal list is the reference generator's, shuffled.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_PLIES = 40
OPENING = [".bbbbbb.", "w......w", "w......w", "w......w", "w......w", "w......w", "w......w", ".bbbbbb."]

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def snapshot(rng, me):
    while True:
        s = bot.State(list("".join(OPENING)), "b", "w")
        ok = True
        for _ in range(rng.randint(0, MAX_PLIES)):
            mv = bot.gen_moves(s)
            if not mv:
                ok = False
                break
            s = bot.apply_move(s, rng.choice(mv))
            if bot.winner_after_move(s) is not None:
                ok = False
                break
        if ok and s.me == me and bot.gen_moves(s):
            return s


def make_case(rng):
    me = rng.choice("bw")
    lines = [me]
    for _ in range(rng.randint(1, 4)):
        s = snapshot(rng, me)
        lines += ["".join(s.cell[y * 8:(y + 1) * 8]) for y in range(8)]
        lines.append("null")
        names = [bot.name_of(a) + bot.name_of(b) for a, b in bot.gen_moves(s)]
        rng.shuffle(names)
        lines.append(str(len(names)))
        lines += names
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
