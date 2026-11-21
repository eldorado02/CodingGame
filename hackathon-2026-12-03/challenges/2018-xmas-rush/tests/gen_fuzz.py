#!/usr/bin/env python3
"""Génère 200 cas de fuzz Xmas Rush avec états Bronze (jusqu'à 3 quêtes révélées)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
FUZZ = HERE / "fuzz"
FUZZ.mkdir(exist_ok=True)
for old in FUZZ.glob("case_*.in"):
    old.unlink()

NAMES = ["CANDY", "CANE", "MASK", "SCROLL", "POTION", "SHIELD",
         "DIAMOND", "FISH", "SWORD", "ARROW", "BOOK", "KEY"]
CH = "URDL"


def turn(rng: random.Random, tt: int, quests: list[int]) -> str:
    lines = [str(tt)]
    for _ in range(7):
        lines.append(" ".join("".join(rng.choice("01") for _ in range(4))
                              for _ in range(7)))
    px, py = rng.randrange(7), rng.randrange(7)
    ox, oy = rng.randrange(7), rng.randrange(7)
    p1 = "".join(rng.choice("01") for _ in range(4))
    p2 = "".join(rng.choice("01") for _ in range(4))
    lines.append(f"{rng.randrange(1, 13)} {px} {py} {p1}")
    lines.append(f"{rng.randrange(1, 13)} {ox} {oy} {p2}")
    items = []
    for i in range(12):
        where = rng.randrange(4)
        if where == 0:      # sur notre tuile
            x, y = -1, -1
        elif where == 1:    # sur la tuile adverse
            x, y = -2, -2
        else:
            x, y = rng.randrange(7), rng.randrange(7)
        items.append(f"{NAMES[i]} {x} {y} 0")
    for i in range(12):
        x, y = rng.randrange(7), rng.randrange(7)
        items.append(f"{NAMES[i]}_{i} {x} {y} 1")
    rng.shuffle(items)
    lines.append(str(len(items)))
    lines += items
    q_lines = []
    for q in quests:
        q_lines.append(f"{NAMES[q]} 0")
        if rng.random() < 0.3:
            q_lines.append(f"{NAMES[(q + 1) % 12]}_x 1")
    lines.append(str(len(q_lines)))
    lines += q_lines
    return "\n".join(lines) + "\n"


for seed in range(200):
    rng = random.Random(90_000 + seed)
    text = ""
    quests = rng.sample(range(12), rng.randint(1, 3))
    for t in range(10):
        text += turn(rng, t % 2, quests)
        if t % 3 == 2 and len(quests) < 12:
            quests.append((quests[-1] + 1) % 12)
    (FUZZ / f"case_{seed:03d}.in").write_text(text)

print(f"generated 200 fuzz cases in {FUZZ}")
