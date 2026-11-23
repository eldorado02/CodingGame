#!/usr/bin/env python3
"""Paper Soccer fuzz: multi-turn streams of one game. Our replies inside the
stream are computed by the Python reference (same CG_NODES as run.sh) so the
stream stays consistent for any language matching it. The opponent plays a
seeded random complete turn (non-losing when possible), sometimes the turn
that ends closest to its target goal, which produces bounce chains and
positions near the goals.
"""
import importlib.util
import os
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_TURNS = 30
GREEDY_PROBABILITY = 0.4

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
NODES = int(os.environ.get("CG_NODES", "300"))


def opponent_turn(rng, pitch, player):
    turns = bot.generate_turns(pitch, player)
    safe = [t for t in turns if t[2] == bot.NORMAL] or turns
    if rng.random() < GREEDY_PROBABILITY:
        return min(safe, key=lambda t: bot.rows_to_goal(player, t[1]))
    return rng.choice(safe)


def game_over(pitch):
    return bot.goal_owner(pitch.ball) >= 0 or pitch.free[pitch.ball] == 0


def make_case(rng):
    me = rng.choice((0, 1))
    pitch = bot.Pitch()
    lines = [str(me)]
    for turn_index in range(rng.randint(1, MAX_TURNS)):
        if turn_index == 0 and me == 0:
            lines += ["1", "-"]
        else:
            move = opponent_turn(rng, pitch, 1 - me)[0]
            pitch.apply(move)
            if game_over(pitch):
                lines.pop()
                lines.pop()
                break
            lines += [str(len(move)), move]
        reply = bot.Search(NODES).run(pitch, me)[0]
        pitch.apply(reply)
        if game_over(pitch):
            break
    if len(lines) == 1:
        lines += ["1", "-"] if me == 0 else ["1", "4"]
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
