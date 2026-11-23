#!/usr/bin/env python3
"""Nine Men's Morris fuzz: self-play streams (standard 24-field board).

Our replies come from the Python reference (same CG_NODES as run.sh); the
opponent plays seeded random legal moves. The legal command list is the
reference generator's, shuffled.
"""
import importlib.util
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
CASES = int(sys.argv[1]) if len(sys.argv) > 1 else 200
MAX_MY_TURNS = 14
ADJ = {"A1": "D1;A4", "D1": "A1;G1;D2", "G1": "D1;G4", "B2": "D2;B4", "D2": "D1;B2;F2;D3", "F2": "D2;F4",
       "C3": "D3;C4", "D3": "D2;C3;E3", "E3": "D3;E4", "A4": "A1;A7;B4", "B4": "A4;B2;B6;C4", "C4": "B4;C3;C5",
       "E4": "E3;E5;F4", "F4": "E4;F2;F6;G4", "G4": "F4;G1;G7", "C5": "C4;D5", "D5": "C5;E5;D6", "E5": "D5;E4",
       "B6": "B4;D6", "D6": "B6;D5;F6;D7", "F6": "D6;F4", "A7": "A4;D7", "D7": "A7;D6;G7", "G7": "D7;G4"}

spec = importlib.util.spec_from_file_location("bot", HERE.parent / "solutions/python/bot.py")
bot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bot)
NAMES = list(ADJ)
BOARD = bot.Board(NAMES, {k: v.split(";") for k, v in ADJ.items()})


def text(m):
    src, dst, take = m
    n = BOARD.names
    if src < 0:
        return "PLACE;%s" % n[dst] if take < 0 else "PLACE&TAKE;%s;%s" % (n[dst], n[take])
    return "MOVE;%s;%s" % (n[src], n[dst]) if take < 0 else "MOVE&TAKE;%s;%s;%s" % (n[src], n[dst], n[take])


def make_case(rng):
    me = rng.randrange(2)
    lines = [str(me), str(len(NAMES))] + ["%s:%s" % (k, v) for k, v in ADJ.items()]
    s = bot.State([bot.EMPTY] * 24, [9, 9], 0)
    last = "-"
    for _ in range(rng.randint(1, MAX_MY_TURNS)):
        if s.side != me:
            mv = bot.gen_moves(BOARD, s)
            if not mv or bot.material(s, s.side) < 3:
                break
            m = rng.choice(mv)
            last = text(m)
            s = bot.apply_move(s, m)
        mv = bot.gen_moves(BOARD, s)
        if not mv or bot.material(s, me) < 3:
            break
        lines.append(last)
        lines.append(";".join("%s:%d" % (n, 2 if s.owner[i] < 0 else s.owner[i]) for i, n in enumerate(NAMES)))
        cmds = [text(m) for m in mv]
        rng.shuffle(cmds)
        lines.append(str(len(cmds)))
        lines += cmds
        s = bot.apply_move(s, bot.Search(BOARD).run(s, mv))
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
