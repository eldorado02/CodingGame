#!/usr/bin/env python3
"""Génère des entrées Spring Challenge 2021 déterministes (10 cas, 8 tours)."""
from pathlib import Path
import random

HERE = Path(__file__).parent
rng = random.Random(20210506)

for k in range(10):
    lines = ["37"]
    for idx in range(37):
        nb = [(idx + d) % 37 for d in range(1, 7)]
        lines.append(" ".join(map(str, [idx, rng.randrange(1, 4), *nb])))
    my_trees = rng.sample(range(37), 4)
    opp_trees = rng.sample([i for i in range(37) if i not in my_trees], 4)
    sun = 12
    osun = 12
    nutrients = 20
    for turn in range(8):
        lines += ["0", str(nutrients), f"{sun} {turn * 10}",
                  f"{osun} {turn * 9} 0"]
        trees = []
        for idx in my_trees:
            trees.append((idx, 3, 1, 0))
        for idx in opp_trees:
            trees.append((idx, 3, 0, 0))
        rng.shuffle(trees)
        lines.append(str(len(trees)))
        for t in trees:
            lines.append(" ".join(map(str, t)))
        acts = ["WAIT"] + [f"COMPLETE {i}" for i in my_trees]
        lines.append(str(len(acts)))
        lines.extend(acts)
        # Simulation grossière : on complète un arbre si possible.
        if turn < 3 and my_trees and sun >= 4:
            my_trees.pop(rng.randrange(len(my_trees)))
            sun -= 4
            nutrients = max(0, nutrients - 1)
        if turn < 3 and opp_trees and osun >= 4:
            opp_trees.pop(rng.randrange(len(opp_trees)))
            osun -= 4
            nutrients = max(0, nutrients - 1)
    (HERE / f"{k + 1:02d}.in").write_text("\n".join(lines) + "\n")
