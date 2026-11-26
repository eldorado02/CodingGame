#!/usr/bin/env python3
"""Start-up! fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

The bot keeps no memory, so each case is 1..40 independent random turns:
low / huge cash, 0..12 managers, features around the 10 threshold, tests
around 4 per feature, bugs, our share 0 or not, rivals with higher or lower
reputation, a full or empty market. One value per line as in the stub.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


PLAYERS = 4
MAX_TURNS = 40


def turn(rng):
    me = rng.randrange(PLAYERS)
    features = rng.choice((0, rng.randint(0, 9), 10, rng.randint(10, 40)))
    tests = rng.choice((0, 4 * features, rng.randint(0, 5 * features + 3)))
    head = [me, PLAYERS, rng.randint(0, 99), rng.choice((0, rng.randint(0, 5000))),
            rng.choice((0, rng.randint(0, 300), rng.randint(0, 50000))),
            rng.randint(0, 20), rng.randint(0, 30), rng.randint(0, 12), features, tests,
            rng.choice((0, 0, rng.randint(0, 15)))]
    shares = [rng.choice((0, rng.randint(0, 400))) for _ in range(PLAYERS)]
    lines = [str(v) for v in head]
    for pid in range(PLAYERS):
        lines.append("%d %d %d" % (pid, shares[pid], rng.choice((1, rng.randint(1, 2000)))))
    return lines


def make_case(rng):
    out = []
    for _ in range(rng.randint(1, MAX_TURNS)):
        out += turn(rng)
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
