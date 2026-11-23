#!/usr/bin/env python3
"""Domain Expansion fuzz: multi-turn streams of one game. Our replies are
computed by the Python reference (same CG_NODES as run.sh) so the stream stays
consistent; the opponent plays a seeded random legal action, preferring
actions that keep the game going. The stream stops when the game ends.
"""
import importlib.util
import os
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_TURNS = 25
SIZE = 7

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
NODES = int(os.environ.get("CG_NODES", "300"))
CORNERS = (0, bot.CELLS - 1)


def xy(cell):
    return "%d %d" % (cell % SIZE, cell // SIZE)


def opponent_action(rng, state):
    acts = bot.actions(state, 1)
    rng.shuffle(acts)
    for cell, d in acts:
        child = state.copy()
        child.apply(1, cell, d)
        if bot.final_score(child, 1) is None:
            return cell, d
    return acts[0]


def make_case(rng):
    mine, theirs = CORNERS if rng.random() < 0.5 else CORNERS[::-1]
    state = bot.State(bot.border_walls(), [mine, theirs])
    lines = ["7 7", xy(mine), xy(theirs)]
    we_start = rng.random() < 0.5
    for turn in range(rng.randint(1, MAX_TURNS)):
        if turn == 0 and we_start:
            lines.append("-1 -1 _")
        else:
            cell, d = opponent_action(rng, state)
            state.apply(1, cell, d)
            if bot.final_score(state, 1) is not None:
                break
            lines.append("%s %s" % (xy(cell), "URDL"[d]))
        cell, d = bot.Search(NODES).run(state, 0)
        state.apply(0, cell, d)
        if bot.final_score(state, 0) is not None:
            break
    if len(lines) == 3:
        lines.append("-1 -1 _")
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
