#!/usr/bin/env python3
"""Yavalath fuzz: the bot is stateless apart from myId and "first turn",
so each case = myId line + 1..3 board snapshots (player's view: 1 = mine).
Snapshots: real first turns (empty board / one opponent stone for the steal
rule) and random games cut at a random ply where nobody has a line of 3+
(dense boards give forced blocks, lost positions, immediate wins).
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


def snapshot(b, last):
    lines = ["9"]
    k = 0
    for y in range(9):
        lines.append("".join(str(b[k + x]) for x in range(bot.ROWS[y])))
        k += bot.ROWS[y]
    lines.append("%d %d" % (bot.CELLS[last] if last >= 0 else (-1, -1)))
    return lines


def random_position(rng, my_id):
    b = [0] * bot.NC
    plies = rng.randint(1, 50)
    p = 1 if my_id == 1 else 2  # first mover in my view
    last = -1
    n_moves = 0
    for _ in range(plies * 3):
        if n_moves >= plies:
            break
        cells = [c for c in range(bot.NC) if b[c] == 0]
        if not cells:
            break
        rng.shuffle(cells)
        ok = [c for c in cells if bot.place_result(b, c, p) == 0]
        if not ok:
            break
        c = ok[0]
        b[c] = p
        last = c
        p = 3 - p
        n_moves += 1
    if p != 1:  # I must be to move: drop one opponent... simpler: one more opp move
        cells = [c for c in range(bot.NC) if b[c] == 0 and bot.place_result(b, c, 2) == 0]
        if not cells:
            return None
        c = rng.choice(cells)
        b[c] = 2
        last = c
    if all(v for v in b):
        return None
    return b, last


def make_case(rng):
    my_id = rng.choice((1, 2))
    lines = [str(my_id)]
    for t in range(rng.randint(1, 3)):
        if t == 0 and rng.random() < 0.3:
            b = [0] * bot.NC
            last = -1
            if my_id == 2:
                last = rng.randrange(bot.NC)
                b[last] = 2
            lines += snapshot(b, last)
            continue
        pos = None
        while pos is None:
            pos = random_position(rng, my_id)
        lines += snapshot(*pos)
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
