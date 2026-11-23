#!/usr/bin/env python3
"""Hex fuzz: random positions (the bot is stateless, it rebuilds the state
from the board every turn). Each case = init line + 1..3 turns.
Covers: empty board (opening), swap decision (blue, one red stone on edge or
inside), dense boards with immediate win / forced block, random middles.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200
S = 11
NB = []
for c in range(S * S):
    r, q = divmod(c, S)
    NB.append([(r + a) * S + q + b for a, b in ((-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0))
               if 0 <= r + a < S and 0 <= q + b < S])


def connected(board, p):
    st = [c for c in range(S) if board[c] == p] if p == 1 else [r * S for r in range(S) if board[r * S] == p]
    seen = set(st)
    while st:
        c = st.pop()
        if (p == 1 and c // S == S - 1) or (p == 2 and c % S == S - 1):
            return True
        for d in NB[c]:
            if d not in seen and board[d] == p:
                seen.add(d)
                st.append(d)
    return False


def name(c):
    return "%s%d" % (chr(97 + c % S), c // S + 1)


def rows(board):
    return [" ".join(".rb"[board[r * S + q]] for q in range(S)) for r in range(S)]


def position(rng, me):
    kind = rng.random()
    board = [0] * (S * S)
    if kind < 0.08 and me == 1:
        return "empty", board
    if kind < 0.2 and me == 2:
        c = rng.randrange(S * S)
        board[c] = 1
        return name(c), board
    target = rng.randint(1, 30) if kind < 0.6 else rng.randint(30, 80)
    order = list(range(S * S))
    rng.shuffle(order)
    p, last = 1, "empty"
    placed = 0
    for c in order:
        if placed >= target:
            break
        board[c] = p
        if connected(board, p):
            board[c] = 0
            continue
        last = name(c)
        p = 3 - p
        placed += 1
    # side to move must be me: fix parity by one more stone of the other side
    if p != me:
        for c in order:
            if board[c] == 0:
                board[c] = p
                if connected(board, p):
                    board[c] = 0
                    continue
                last = name(c)
                break
    if sum(1 for x in board if x == 0) == 0:
        return position(rng, me)
    return last, board


def make_case(rng):
    me = rng.choice((1, 2))
    lines = ["11 %s" % ("red" if me == 1 else "blue")]
    for _ in range(rng.randint(1, 3)):
        last, board = position(rng, me)
        lines.append(last)
        lines.extend(rows(board))
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
