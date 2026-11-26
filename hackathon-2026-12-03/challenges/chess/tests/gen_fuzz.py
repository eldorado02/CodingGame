#!/usr/bin/env python3
"""Chess fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case: the constants block, then 1..20 random positions (FEN, side to
move w/b) with 1..40 moves made of real piece squares: plain moves, captures,
king onto its own rook (castling), diagonal pawn moves to empty squares (en
passant), promotions, plus a few malformed moves.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


PIECES = "PPPPNBRQ"
MAX_TURNS = 20


def fen_of(board):
    rows = []
    for rank in range(7, -1, -1):
        row, empty = "", 0
        for file in range(8):
            p = board.get((file, rank))
            if p:
                row += (str(empty) if empty else "") + p
                empty = 0
            else:
                empty += 1
        rows.append(row + (str(empty) if empty else ""))
    return "/".join(rows)


def name(sq):
    return "%s%d" % ("abcdefgh"[sq[0]], sq[1] + 1)


def position(rng):
    cells = rng.sample([(f, r) for f in range(8) for r in range(8)], rng.randint(4, 28))
    board = {cells[0]: "K", cells[1]: "k"}
    for sq in cells[2:]:
        p = rng.choice(PIECES)
        board[sq] = p if rng.random() < 0.5 else p.lower()
    return board


def move(rng, board, white):
    own = [sq for sq, p in board.items() if p.isupper() == white] or list(board)
    src = rng.choice(own)
    kind = rng.randrange(6)
    if kind == 0:
        rooks = [sq for sq, p in board.items() if p == ("R" if white else "r")]
        if board[src].lower() == "k" and rooks:
            return name(src) + name(rng.choice(rooks))
    if kind == 1:
        return name(src) + name((rng.randrange(8), rng.randrange(8))) + rng.choice("qrbn")
    if kind == 2:
        return rng.choice(("e2", "a", "z9z9", "e9e4", "a1"))
    return name(src) + name((rng.randrange(8), rng.randrange(8)))


def make_case(rng):
    out = ["2", "crazyHouse 0", "maxMoves 125"]
    for _ in range(rng.randint(1, MAX_TURNS)):
        board = position(rng)
        white = rng.random() < 0.5
        out.append("%s %s AHah - 0 1" % (fen_of(board), "w" if white else "b"))
        moves = [move(rng, board, white) for _ in range(rng.randint(1, 40))]
        out.append(str(len(moves)))
        out += moves
    return "\n".join(out) + "\n"


def main():
    FUZZ.mkdir(exist_ok=True)
    for old in FUZZ.glob("case_*.in"):
        old.unlink()
    for i in range(N):
        rng = random.Random(i)
        (FUZZ / f"case_{i:03d}.in").write_text(make_case(rng))
    print(f"{N} cases written to {FUZZ}")


if __name__ == "__main__":
    main()
