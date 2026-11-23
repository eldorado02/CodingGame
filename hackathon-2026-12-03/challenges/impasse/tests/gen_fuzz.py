#!/usr/bin/env python3
"""Impasse fuzz: random boards + random coordinate strings (1-3 coords).

The v1 bot only scores the given strings against the board, so cases mix
plausible moves, malformed tokens and edge rows (nearest/furthest).
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


def coord(rng):
    return chr(97 + rng.randrange(8)) + str(rng.randint(1, 8))


def make_case(rng):
    me = rng.choice("wb")
    lines = [me]
    for _ in range(rng.randint(1, 6)):
        for _ in range(8):
            lines.append("".join(rng.choice("....wWbB") for _ in range(8)))
        lines.append(rng.choice(["null", "d2f4", "b6", "f8g7c5"]))
        mv = []
        for _ in range(rng.randint(0, 15)):
            k = rng.choice([1, 2, 2, 2, 3])
            t = "".join(coord(rng) for _ in range(k))
            if rng.random() < 0.05:
                t = rng.choice(["zz", "a9b1", "a1b"])
            mv.append(t)
        lines.append(str(len(mv)))
        lines += mv
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
