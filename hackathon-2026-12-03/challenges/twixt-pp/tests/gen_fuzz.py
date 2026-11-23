#!/usr/bin/env python3
"""TwixT-PP fuzz: random games played with the reference engine (links are
created exactly as the referee does). Each case is a stream of 1..3 turns of
the same game seen by us: first turn ("FIRST" or the opponent's opening peg,
which exercises the swap rule), optional "SWAP" answer, then random middles.
Peg and link lists are shuffled because the output must not depend on them.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_GAP = 25   # random plies between two snapshots

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def turn_lines(rng, state, last):
    lines = [last]
    for player in (bot.ME, bot.OPP):
        pegs = [bot.name_of(c) for c in range(bot.CELLS) if state.board[c] == player]
        rng.shuffle(pegs)
        lines += [str(len(pegs))] + pegs
        links = []
        for seg, (a, b) in enumerate(bot.SEGMENTS):
            if state.linked[player][seg]:
                pair = [bot.name_of(a), bot.name_of(b)]
                rng.shuffle(pair)
                links.append(" ".join(pair))
        rng.shuffle(links)
        lines += [str(len(links))] + links
    return lines


def game_over(state):
    return bot.path_distance(state, bot.ME) == 0 or bot.path_distance(state, bot.OPP) == 0


def random_ply(rng, state, player):
    moves = state.legal_moves(player)
    if not moves:
        return None
    cell = rng.choice(moves)
    state.place(player, cell)
    return cell


def make_case(rng):
    my_side = rng.choice((bot.TOP_BOTTOM, bot.LEFT_RIGHT))
    state = bot.State(my_side)
    lines = []
    if my_side == bot.TOP_BOTTOM:
        lines += turn_lines(rng, state, "FIRST")
        if rng.random() < 0.3:   # opponent swaps our opening peg
            state.place(bot.OPP, bot.transpose(bot.OPENING_CELL))
            lines += turn_lines(rng, state, "SWAP")
        else:
            state.place(bot.ME, bot.OPENING_CELL)
    else:
        opening = random_ply(rng, state, bot.OPP)
        lines += turn_lines(rng, state, bot.name_of(opening))
        state.place(bot.ME, rng.choice(state.legal_moves(bot.ME)))
    for _ in range(rng.randint(0, 2)):
        last = None
        for _ in range(rng.randint(1, MAX_GAP)):
            last = random_ply(rng, state, bot.OPP)
            if last is None or game_over(state):
                return "\n".join(lines) + "\n"
            if random_ply(rng, state, bot.ME) is None or game_over(state):
                return "\n".join(lines) + "\n"
        last = random_ply(rng, state, bot.OPP)
        if last is None or game_over(state) or not state.legal_moves(bot.ME):
            break
        lines += turn_lines(rng, state, bot.name_of(last))
        state.place(bot.ME, rng.choice(state.legal_moves(bot.ME)))
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
