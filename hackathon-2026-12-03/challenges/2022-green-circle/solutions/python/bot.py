#!/usr/bin/env python3
"""Green Circle (Wood 4) — choisir les releases utiles et les bureaux utiles."""
import sys


def rd() -> str:
    line = sys.stdin.readline()
    return line.strip() if line else ""


def good_score(hand: list[int], app: list[int]) -> int:
    """Nombre de compétences « propres » utilisables pour cette application."""
    covered = sum(min(app[i], 2 * hand[i]) for i in range(8))
    return covered + min(hand[8], sum(app) - covered)


def best_app(hand: list[int], apps: dict[int, list[int]], ids: list[int]) -> int:
    best_id = -1
    best_score = -1
    for aid in ids:
        app = apps.get(aid)
        if app is None:
            continue
        score = good_score(hand, app)
        if score > best_score or (score == best_score and aid < best_id):
            best_score = score
            best_id = aid
    return best_id


def main() -> None:
    while True:
        phase = rd()
        if not phase:
            break
        app_count = int(rd())
        apps: dict[int, list[int]] = {}
        for _ in range(app_count):
            f = rd().split()
            if len(f) < 9:
                continue
            apps[int(f[1])] = [int(v) for v in f[2:10]]
        rd()  # nous : location, score, permanents
        rd()  # adversaire

        loc_count = int(rd())
        hand = [0] * 10
        for _ in range(loc_count):
            f = rd().split()
            if len(f) >= 11 and f[0] == "HAND":
                hand = [int(v) for v in f[1:11]]

        move_count = int(rd())
        moves = [rd() for _ in range(move_count)]

        if phase == "MOVE":
            release_ids = [int(m.split()[1]) for m in moves
                           if m.startswith("RELEASE ") and len(m.split()) >= 2]
            move_ids = [int(m.split()[1]) for m in moves
                        if m.startswith("MOVE ") and len(m.split()) >= 2]
            chosen = -1
            best_score = -1
            for dest in move_ids:
                h2 = hand[:]
                h2[dest] += 1
                app_id = best_app(h2, apps, release_ids or list(apps))
                score = good_score(h2, apps[app_id]) if app_id >= 0 else 0
                if score > best_score or (score == best_score and dest < chosen):
                    best_score = score
                    chosen = dest
            print(f"MOVE {chosen}" if chosen >= 0 else (moves[0] if moves else "WAIT"))
        elif phase == "RELEASE":
            release_ids = [int(m.split()[1]) for m in moves
                           if m.startswith("RELEASE ") and len(m.split()) >= 2]
            app_id = best_app(hand, apps, release_ids)
            if app_id >= 0:
                print(f"RELEASE {app_id}")
            else:
                print("WAIT")
        else:
            print(moves[0] if moves else "WAIT")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
