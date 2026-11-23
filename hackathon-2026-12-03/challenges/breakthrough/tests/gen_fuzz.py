#!/usr/bin/env python3
"""Breakthrough fuzz: self-play input streams.

Each case is a multi-turn stdin stream. Our replies inside the stream are
computed with the Python reference itself (same CG_NODES as run.sh), so the
stream stays consistent for every language that matches the reference; any
divergence shows up as a stdout difference. The opponent plays a seeded
random legal move (captures favoured). Legal lists are shuffled because the
bot must not depend on their order.
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


def make_case(rng):
    me = bot.WHITE if rng.random() < 0.5 else bot.BLACK
    s = bot.initial_state()
    turns = rng.randint(1, 30)
    lines = []
    for t in range(turns):
        if t == 0 and me == bot.WHITE:
            lines.append("None")
        else:
            s.to_move = 3 - me
            if bot.winner(s):
                break
            opp = bot.gen_moves(s)
            if not opp:
                break
            caps = [m for m in opp if (1 << m[1]) & (s.w | s.b)]
            m = rng.choice(caps) if caps and rng.random() < 0.6 else rng.choice(opp)
            lines.append(bot.name_of(m))
            s = bot.apply_move(s, m)
        s.to_move = me
        if bot.winner(s):
            lines.pop()
            break
        mine = bot.gen_moves(s)
        if not mine:
            lines.pop()
            break
        names = [bot.name_of(m) for m in mine]
        rng.shuffle(names)
        lines.append(str(len(names)))
        lines.extend(names)
        best = bot.Search().run(s, mine)
        s = bot.apply_move(s, best)
    if not lines:
        lines = ["None", "1", "a2a3"]
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
