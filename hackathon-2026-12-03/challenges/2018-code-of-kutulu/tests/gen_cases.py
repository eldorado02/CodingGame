#!/usr/bin/env python3
"""Génère les cas déterministes de Code of Kutulu."""
from pathlib import Path

HERE = Path(__file__).parent
W = H = 10


def write(name: str, grid: list[str], entities: list[str]) -> None:
    lines = [str(W), str(H)] + grid + ["0 0 0 0", str(len(entities))] + entities
    (HERE / name).write_text("\n".join(lines) + "\n")


def explorer(eid: int, x: int, y: int, sanity: int) -> str:
    return f"EXPLORER {eid} {x} {y} {sanity} 0 -1"


def wanderer(eid: int, x: int, y: int, state: int = 1) -> str:
    return f"WANDERER {eid} {x} {y} 5 {state} 0"


OPEN = ["." * W for _ in range(H)]
WALLS = ["." * W for _ in range(H)]
WALLS[6] = WALLS[6][:5] + "#" + WALLS[6][6:]
WALLS[5] = WALLS[5][:6] + "#" + WALLS[5][7:]

# 01 — déjà réassuré, pas de menace.
write("01_reassure.in", OPEN, [explorer(0, 5, 5, 250), explorer(1, 6, 5, 250)])
# 02 — seul : se rapprocher du plus proche explorateur.
write("02_alone.in", OPEN, [explorer(7, 5, 5, 250), explorer(3, 8, 8, 250)])
# 03 — réassuré mais wanderer à distance 1 : fuir sans rompre la réassurance.
write("03_flee.in", OPEN,
      [explorer(0, 5, 5, 250), explorer(1, 6, 5, 250), wanderer(9, 5, 4)])
# 04 — murs à droite et en bas.
write("04_wall.in", WALLS, [explorer(0, 5, 5, 250), explorer(1, 8, 8, 250)])
# 05 — les autres explorateurs sont morts : on ne peut plus être réassuré.
write("05_dead.in", OPEN, [explorer(0, 5, 5, 250), explorer(1, 6, 5, 0)])
# 06 — minion en cours de spawn (param1=0) : ignoré.
write("06_spawning.in", OPEN,
      [explorer(0, 5, 5, 250), explorer(1, 6, 5, 250), wanderer(9, 5, 4, 0)])
