#!/usr/bin/env python3
"""Onitama fuzz: streams of 1..5 snapshot turns from seeded random playouts.

Five random cards from a pool of 16 move patterns (player 0 frame); each
snapshot is written like the referee: cards in the frame of their next user
(ours and the centre card for us, the opponent's for him).
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_PLIES = 20
STEPS = [(dx, dy) for dx in range(-2, 3) for dy in range(-2, 3) if (dx, dy) != (0, 0) and abs(dx) + abs(dy) <= 3]

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)


def random_cards(rng):
    return {cid: tuple(rng.sample(STEPS, rng.randint(2, 4))) for cid in rng.sample(range(1, 17), 5)}


def start_state(ids):
    kind, owner = [bot.EMPTY] * 25, [0] * 25
    for x in range(5):
        kind[x], owner[x] = bot.STUDENT, 0
        kind[20 + x], owner[20 + x] = bot.STUDENT, 1
    kind[2] = kind[22] = bot.MASTER
    return bot.State(kind, owner, [[ids[0], ids[1]], [ids[2], ids[3]]], ids[4], 0)


def snapshot(rng, me):
    while True:
        cards = random_cards(rng)
        bot.CARDS.clear()
        bot.CARDS.update(cards)
        s = start_state(list(cards))
        for _ in range(rng.randint(0, MAX_PLIES)):
            if bot.winner(s) >= 0:
                break
            s = bot.apply_move(s, rng.choice(bot.gen_moves(s)))
        if bot.winner(s) < 0 and s.side == me:
            return s, cards


def render(s, cards, me):
    lines = []
    for li in range(5):
        row = 5 - li
        out = ""
        for x in range(5):
            i = bot.cell_index(x, row)
            if s.kind[i] == bot.EMPTY:
                out += "."
            else:
                ch = "w" if s.owner[i] == 0 else "b"
                out += ch.upper() if s.kind[i] == bot.MASTER else ch
        lines.append(out)
    entries = [(0, c) for c in s.hands[0]] + [(1, c) for c in s.hands[1]] + [(-1, s.center)]
    for owner, cid in entries:
        user = me if owner == -1 else owner
        sign = 1 if user == 0 else -1
        moves = [v for dx, dy in cards[cid] for v in (sign * dx, sign * dy)]
        moves += [0] * (8 - len(moves))
        lines.append(" ".join(map(str, [owner, cid] + moves)))
    legal = bot.gen_moves(s)
    lines.append(str(len(legal)))
    lines += [bot.action_text(m) for m in legal]
    return lines


def make_case(rng):
    me = rng.randrange(2)
    lines = [str(me)]
    for _ in range(rng.randint(1, 5)):
        s, cards = snapshot(rng, me)
        lines += render(s, cards, me)
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
