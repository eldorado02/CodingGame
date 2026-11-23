#!/usr/bin/env python3
"""TEMPLATE fuzz generator: writes tests/fuzz/case_XXX.in (default 200 cases).

GAME: replace make_case() so that each case is a full stdin stream the bot
can consume (one or several turns). Rules:
  * seeded RNG -> reproducible cases (seed = case index);
  * cover the bugs already paid (see ALGO.md §6): empty input, last turn,
    negative / out-of-range values, max board, ties, forced moves;
  * multi-turn cases must stay valid whatever the bot answered (e.g. opponent
    moves drawn from cells that cannot be illegal, or snapshot inputs).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


def make_case(rng):
    """GAME: toy tic-tac-toe stream, 1..4 turns, opponent cells distinct."""
    turns = rng.randint(1, 4)
    cells = list(range(9))
    rng.shuffle(cells)
    out = []
    first = rng.random() < 0.5
    for t in range(turns):
        if t == 0 and first:
            out.append("-1 -1")
        else:
            c = cells.pop()
            out.append(f"{c // 3} {c % 3}")
        # legal-moves list is ignored by the bot; send a plausible one
        out.append("1")
        out.append("0 0")
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
