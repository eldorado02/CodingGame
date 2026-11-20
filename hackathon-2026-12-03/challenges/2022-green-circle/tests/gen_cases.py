#!/usr/bin/env python3
"""Génère des entrées Green Circle déterministes (10 cas, 6 paires de phases)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20220616)


def make_case(k: int) -> str:
    lines: list[str] = []
    apps = {}
    for aid in range(1, 13):
        skills = rng.sample(range(8), 3)
        req = [0] * 8
        for s in skills:
            req[s] = 2
        apps[aid] = req

    location = -1
    last_skill = -1
    for turn in range(6):
        phase = "MOVE"
        lines.append(phase)
        lines.append(str(len(apps)))
        for aid, req in apps.items():
            lines.append("APPLICATION " + " ".join(map(str, [aid, *req])))
        lines.append(f"{location} {turn // 2} 0 0")
        lines.append(f"{-1 if location < 0 else (location + 4) % 8} {turn // 2} 0 0")

        hand = [0] * 10
        if last_skill >= 0:
            hand[last_skill] = 1
        hand[8] = rng.randrange(0, 3)
        hand[9] = rng.randrange(0, 3)
        lines.append("4")
        lines.append("HAND " + " ".join(map(str, hand)))
        lines.append("DRAW " + " ".join(str(rng.randrange(0, 6)) for _ in range(10)))
        lines.append("DISCARD " + " ".join(str(rng.randrange(0, 6)) for _ in range(10)))
        lines.append("OPPONENT_CARDS 0 0 0 0 0 0 0 0 4 4")

        moves = [f"MOVE {d}" for d in range(8) if d != location] + ["RANDOM"]
        lines.append(str(len(moves)))
        lines.extend(moves)

        last_skill = rng.randrange(8)
        location = last_skill

        phase = "RELEASE"
        lines.append(phase)
        lines.append(str(len(apps)))
        for aid, req in apps.items():
            lines.append("APPLICATION " + " ".join(map(str, [aid, *req])))
        lines.append(f"{location} {turn // 2} 0 0")
        lines.append(f"{(location + 4) % 8} {turn // 2} 0 0")
        lines.append("4")
        lines.append("HAND " + " ".join(map(str, hand + [0] * (10 - len(hand)))))
        lines.append("DRAW " + " ".join(str(rng.randrange(0, 6)) for _ in range(10)))
        lines.append("DISCARD " + " ".join(str(rng.randrange(0, 6)) for _ in range(10)))
        lines.append("OPPONENT_CARDS 0 0 0 0 0 0 0 0 4 4")

        release_ids = sorted(rng.sample(list(apps), k=min(len(apps), rng.randrange(1, 6))))
        moves = [f"RELEASE {aid}" for aid in release_ids] + ["WAIT", "RANDOM"]
        lines.append(str(len(moves)))
        lines.extend(moves)

        if release_ids:
            apps.pop(release_ids[0], None)

    return "\n".join(lines) + "\n"


for k in range(10):
    (HERE / f"{k + 1:02d}.in").write_text(make_case(k))
