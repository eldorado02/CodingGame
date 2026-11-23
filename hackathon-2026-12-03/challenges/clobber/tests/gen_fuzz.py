#!/usr/bin/env python3
"""Clobber fuzz: streams of 1..5 snapshot turns from seeded random playouts.

Board sizes 5..8 (checkerboard start), random legal moves, our side to move
with at least one move. lastAction / actionsCount lines are filled but
ignored by the bots (they regenerate moves).
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_PLIES = 40

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def start(board):
    white = black = 0
    for i in board.cells:
        x, y = i % bot.STRIDE, i // bot.STRIDE
        if (x + y) % 2 == 0:
            white |= 1 << i
        else:
            black |= 1 << i
    return white, black


def snapshot(rng, board, me):
    while True:
        white, black = start(board)
        s = bot.State(white, black)   # white to move
        side = "w"
        for _ in range(rng.randint(0, MAX_PLIES)):
            mv = bot.gen_moves(board, s)
            if not mv:
                break
            s = bot.apply_move(s, rng.choice(mv))
            side = "b" if side == "w" else "w"
        if side == me and bot.gen_moves(board, s):
            return s


def render(board, s, me):
    rows = []
    other = "b" if me == "w" else "w"
    for y in range(board.n):
        row = ""
        for x in range(board.n):
            bit = 1 << (y * bot.STRIDE + x)
            row += me if s.mine & bit else other if s.theirs & bit else "."
        rows.append(row)
    return rows


def make_case(rng):
    n = rng.choice([5, 6, 7, 8, 8, 8])
    me = rng.choice("wb")
    board = bot.Board(n)
    lines = [str(n), me]
    for _ in range(rng.randint(1, 5)):
        s = snapshot(rng, board, me)
        lines += render(board, s, me)
        lines += ["null", str(len(bot.gen_moves(board, s)))]
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
