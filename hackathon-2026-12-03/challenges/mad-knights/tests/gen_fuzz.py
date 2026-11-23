#!/usr/bin/env python3
"""Mad Knights fuzz: streams of 1..5 snapshot turns from random playouts.

Knights start off the border (like the referee); random legal moves are
played in turn order, eliminating stuck players, until it is our turn at a
random ply with at least one legal move. Legal list shuffled.
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


def snapshot(rng, me):
    while True:
        inner = [y * 8 + x for y in range(1, 7) for x in range(1, 7)]
        pos = rng.sample(inner, 3)
        blocked = 0
        for p in pos:
            blocked |= 1 << p
        s = bot.State(blocked, pos, [True, True, True], 0)
        target = rng.randint(0, 30)
        plies = 0
        while True:
            if not s.alive[me] or sum(s.alive) == 1:
                break
            mv = bot.moves_of(s.blocked, s.pos[s.turn])
            if not mv:
                s = bot.eliminate(s)
                continue
            if s.turn == me and plies >= target:
                return s, mv
            s = bot.apply_move(s, rng.choice(mv))
            plies += 1


def render(s, rng):
    lines = []
    for j, c in enumerate("rgb"):
        lines.append("%s %d %s" % (c, 1 if s.alive[j] else 0, rng.choice(["null", "e4", "c3"])))
    for y in range(8):
        row = ""
        for x in range(8):
            i = y * 8 + x
            ch = "#" if (s.blocked >> i) & 1 else "."
            for j, c in enumerate("rgb"):
                if s.pos[j] == i:
                    ch = c
            row += ch
        lines.append(row)
    return lines


def make_case(rng):
    me = rng.randrange(3)
    out = ["rgb"[me]]
    for _ in range(rng.randint(1, 5)):
        s, mv = snapshot(rng, me)
        out += render(s, rng)
        names = [bot.name_of(t) for t in mv]
        rng.shuffle(names)
        out.append(str(len(names)))
        out += names
    return "\n".join(out) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(random.Random(i)))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
