#!/usr/bin/env python3
"""Isola fuzz: self-play input streams.

Our replies are computed with the Python reference (same CG_NODES as
run.sh) so the stream stays consistent; the opponent plays a seeded random
legal (move, removal). Each case is the init lines + 1..12 turns.
"""
import importlib.util
import os
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_TURNS = 12
START = ((0, 4), (8, 4))

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def xy(i):
    return i % bot.W, i // bot.W


def random_action(rng, s):
    actions = []
    for t in bot.NB[s.pos[s.side]]:
        if bot.free(s, t):
            for r in range(81):
                if r != t and bot.free(s, r):
                    actions.append((t, r))
    return rng.choice(actions) if actions else None


def make_case(rng):
    me = rng.randrange(2)
    sx, sy = START[me]
    lines = [str(sx), str(sy)]
    pos = [START[0][1] * bot.W + START[0][0], START[1][1] * bot.W + START[1][0]]
    s = bot.State([False] * 81, pos, 0)
    last_removed = (-1, -1)
    for _ in range(rng.randint(1, MAX_TURNS)):
        if s.side != me:
            a = random_action(rng, s)
            if a is None:
                break
            s = bot.apply_action(s, a)
            last_removed = xy(a[1])
        if not bot.moves_of(s, me):
            break
        ox, oy = xy(s.pos[1 - me])
        lines += [str(ox), str(oy), str(last_removed[0]), str(last_removed[1])]
        s = bot.apply_action(s, bot.Search().run(s))
        if not bot.moves_of(s, s.side):
            break
    return "\n".join(lines) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(CASES):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{CASES} cases written to {FUZZ} (CG_NODES={os.environ.get('CG_NODES')})")


if __name__ == "__main__":
    main()
