#!/usr/bin/env python3
"""Platinum Rift 2 fuzz: writes tests/fuzz/case_XXX.in (default 200 cases).

Each case is a hexagonal grid map (offset rows, 6 neighbours, duplicated
links) and 1..20 random turns: owners, pods of both players, fog, platinum.
The first turn usually shows one HQ per player, sometimes none (to cover the
late HQ detection). Turns are snapshots: the bot keeps only its HQs and the
platinum it saw.
"""
import random
import sys
from pathlib import Path

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 200


MAX_TURNS = 20


def hex_links(width, height):
    links = []
    for y in range(height):
        for x in range(width):
            z = y * width + x
            shifts = ((1, 0), (0, 1), (-1, 1)) if y % 2 == 0 else ((1, 0), (0, 1), (1, 1))
            for dx, dy in shifts:
                nx, ny = x + dx, y + dy
                if 0 <= nx < width and ny < height:
                    links.append((z, ny * width + nx))
    return links


def make_case(rng):
    width, height = rng.randint(6, 12), rng.randint(9, 12)
    zones = width * height
    links = hex_links(width, height)
    links += rng.sample(links, rng.randint(0, 3))      # duplicated links
    me = rng.randint(0, 1)
    out = ["2 %d %d %d" % (me, zones, len(links))]
    out += ["%d 0" % z for z in range(zones)]
    out += ["%d %d" % link for link in links]
    hqs = rng.sample(range(zones), 2) if rng.random() < 0.9 else []
    for turn in range(rng.randint(1, MAX_TURNS)):
        out.append(str(rng.randint(0, 60)))
        rows = []
        for z in range(zones):
            owner = rng.choice((-1, -1, 0, 1)) if turn else -1
            if hqs and turn == 0:
                owner = 0 if z == hqs[0] else (1 if z == hqs[1] else -1)
            pods = [rng.choice((0, 0, 0, rng.randint(1, 12))) for _ in range(2)]
            visible = rng.randint(0, 1)
            rows.append("%d %d %d %d %d %d" % (z, owner, pods[0], pods[1], visible, rng.randint(0, 6) if visible else 0))
        rng.shuffle(rows)
        out += rows
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
