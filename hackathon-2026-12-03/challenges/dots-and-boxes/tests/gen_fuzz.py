#!/usr/bin/env python3
"""Dots and Boxes fuzz: streams of 1..4 snapshot turns (board sizes 2..5).

Random edges are drawn (scores counted for the player who closes boxes),
then the free sides of every box with at least one free side are listed in
a shuffled box order, like the referee's per-turn input.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def turn_lines(rng, board, drawn, mine, theirs):
    n = board.n
    entries = []
    for y in range(n):
        for x in range(n):
            es = board.box_edges[y * n + x]
            sides = "".join(bot.SIDE_LETTERS[k] for k in range(4) if not drawn[es[k]])
            if sides:
                entries.append("%s%d %s" % (chr(65 + x), y + 1, sides))
    rng.shuffle(entries)
    return ["%d %d" % (mine, theirs), str(len(entries))] + entries


def make_case(rng):
    n = rng.randint(2, 5)
    board = bot.Board(n)
    lines = [str(n), rng.choice("AB")]
    for _ in range(rng.randint(1, 4)):
        drawn = [False] * board.edges
        free = list(range(board.edges))
        rng.shuffle(free)
        cut = rng.randint(0, board.edges - 1)
        for e in free[:cut]:
            drawn[e] = True
        boxes = sum(1 for b in range(n * n) if bot.free_sides(board, drawn, b) == 0)
        mine = rng.randint(0, boxes)
        lines += turn_lines(rng, board, drawn, mine, boxes - mine)
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
