#!/usr/bin/env python3
"""Fireworks fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is 1..60 turns shaped like the referee's: mistakes / hints left,
events (NEWGAME with our id 0..2, PLAY / ERROR / DISCARD, SAYLEVEL / SAYCOLOR
followed by knowledge lines with '?', full knowledge lines), then the two
teammates' full hands. Some garbage lines and a deck that runs out cover the
parser edges. Our answers are not fed back (the bot only reads events).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


COLORS = ("WHITE", "RED", "BLUE", "GREEN", "YELLOW")
SLOTS = "ABCDE"
MAX_TURNS = 60


def card(rng):
    return "%s-%d" % (rng.choice(COLORS), rng.randint(1, 5))


def hint_lines(rng, target):
    if rng.random() < 0.5:
        level = rng.randint(1, 5)
        lines = ["%d:SAYLEVEL:%d:%d" % (rng.randint(0, 2), target, level)]
        value = "?-%d" % level
    else:
        colour = rng.choice(COLORS)
        lines = ["%d:SAYCOLOR:%d:%s" % (rng.randint(0, 2), target, colour)]
        value = "%s-?" % colour
    for slot in rng.sample(SLOTS, rng.randint(1, 3)):
        lines.append("%d:CARD:%s:%s" % (target, slot, rng.choice((value, card(rng)))))
    return lines


def event(rng, me):
    kind = rng.randrange(6)
    player = rng.randint(0, 2)
    if kind < 3:
        return ["%d:%s:%s:%s" % (player, ("PLAY", "ERROR", "DISCARD")[kind], rng.choice(SLOTS), card(rng))]
    if kind < 5:
        return hint_lines(rng, rng.choice((me, (me + 1) % 3, (me + 2) % 3)))
    return [rng.choice(("garbage", "x:PLAY:A:RED-1", "1:PLAY", "2:CARD:A:PINK-9", ""))]


def make_case(rng):
    out = []
    me = 0
    for turn in range(rng.randint(1, MAX_TURNS)):
        events = []
        if turn == 0 or rng.random() < 0.05:
            me = rng.randint(0, 2)
            events.append("%d:NEWGAME" % me)
        for _ in range(rng.randint(0, 6)):
            events += event(rng, me)
        for mate in ((me + 1) % 3, (me + 2) % 3):
            for slot in SLOTS[:rng.choice((5, 5, 5, 4))]:
                events.append("%d:CARD:%s:%s" % (mate, slot, card(rng)))
        out.append("%d %d" % (rng.randint(0, 3), rng.choice((0, 12, rng.randint(0, 12)))))
        out.append(str(len(events)))
        out += events
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
