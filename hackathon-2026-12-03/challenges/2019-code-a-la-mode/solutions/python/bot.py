#!/usr/bin/env python3
"""Code a la Mode — machine à états : fabriquer un plat B+I et le livrer."""
import sys

FOODS = ("BLUEBERRIES", "ICE_CREAM")
NEEDED = set(FOODS)


def rd() -> str:
    return sys.stdin.readline().strip()


def parse_item(s: str) -> set[str]:
    parts = s.split("-")
    return set(parts[1:]) if parts[0] == "DISH" else set()


def main() -> None:
    n_all = int(rd())
    for _ in range(n_all):
        rd()
    spots = {"D": None, "W": None, "B": None, "I": None, "#": []}
    for y in range(7):
        row = rd()
        for x, c in enumerate(row):
            if c in spots:
                if c == "#":
                    spots[c].append((x, y))
                elif spots[c] is None:
                    spots[c] = (x, y)

    while True:
        turns = rd()
        if not turns:
            break
        px, py, held = rd().split()
        qx, qy, partner = rd().split()
        nt = int(rd())
        tables = []
        for _ in range(nt):
            tx, ty, item = rd().split()
            tables.append((int(tx), int(ty), item))
        rd()  # four
        nc = int(rd())
        for _ in range(nc):
            rd()

        contents = parse_item(held)
        target = None
        if "DISH" in held and NEEDED <= contents:
            target = spots["W"]
        elif "DISH" in held:
            missing = sorted(NEEDED - contents)
            target = None
            for food in missing:
                for tx, ty, item in tables:
                    if item == food:
                        target = (tx, ty)
                        break
                if target:
                    break
            if target is None:
                target = spots["B"] if missing[0] == "BLUEBERRIES" else spots["I"]
        elif held in FOODS:
            target = spots["D"]
        else:
            for wanted in (NEEDED, set(FOODS)):
                for tx, ty, item in tables:
                    if item.startswith("DISH") and parse_item(item) == wanted:
                        target = (tx, ty)
                        break
                if target:
                    break
            if target is None:
                for tx, ty, item in tables:
                    if item.startswith("DISH"):
                        target = (tx, ty)
                        break
            if target is None:
                for tx, ty, item in tables:
                    if item in FOODS:
                        target = (tx, ty)
                        break
            if target is None:
                target = spots["D"]

        if target is None:
            print("WAIT")
        else:
            print(f"USE {target[0]} {target[1]}")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
