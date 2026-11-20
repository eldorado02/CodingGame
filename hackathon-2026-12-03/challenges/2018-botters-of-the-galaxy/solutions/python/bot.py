#!/usr/bin/env python3
"""Botters of the Galaxy Wood 6 — HULK, ATTACK héros puis tour."""
import sys


def d2(ax: int, ay: int, bx: int, by: int) -> int:
    return (ax - bx) ** 2 + (ay - by) ** 2


def main() -> None:
    line = sys.stdin.readline()
    if not line:
        return
    team = int(line)
    bushes = int(sys.stdin.readline())
    for _ in range(bushes):
        sys.stdin.readline()
    items = int(sys.stdin.readline())
    for _ in range(items):
        sys.stdin.readline()

    my_x = my_y = 0
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        int(line)  # gold
        int(sys.stdin.readline())  # adversaire gold
        round_type = int(sys.stdin.readline())
        count = int(sys.stdin.readline())

        entities: list[dict] = []
        for _ in range(count):
            parts = sys.stdin.readline().split()
            if not parts:
                return
            eid = int(parts[0])
            eteam = int(parts[1])
            kind = parts[2]
            x, y = int(parts[3]), int(parts[4])
            health = int(parts[6])
            if kind == "HERO" and eteam == team:
                my_x, my_y = x, y
            if eteam == team or health <= 0:
                continue
            entities.append({"id": eid, "kind": kind, "x": x, "y": y})

        # La phase de pick contient aussi des entités : elles viennent d'être
        # consommées. On sort le nom du héros et on passe au tour suivant.
        if round_type < 0:
            print("HULK", flush=True)
            continue

        enemies = [e for e in entities if e["kind"] == "HERO"]
        towers = [e for e in entities if e["kind"] == "TOWER"]
        others = [e for e in entities if e["kind"] not in ("HERO", "TOWER")]

        if enemies:
            target = min(enemies, key=lambda e: (d2(my_x, my_y, e["x"], e["y"]), e["id"]))
        elif towers:
            target = min(towers, key=lambda e: (d2(my_x, my_y, e["x"], e["y"]), e["id"]))
        elif others:
            target = min(others, key=lambda e: (d2(my_x, my_y, e["x"], e["y"]), e["id"]))
        else:
            target = None

        order = f"ATTACK {target['id']}" if target else "WAIT"
        for _ in range(round_type):
            print(order, flush=True)


if __name__ == "__main__":
    main()
