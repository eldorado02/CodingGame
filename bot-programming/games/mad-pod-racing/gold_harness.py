#!/usr/bin/env python3
"""Local end-to-end harness for the Gold/Legend C++ planner.

Feeds a synthetic 4-pod game to bot_gold.cpp, simulates the race with
collisions, and reports whether the bot stays within the time budget and
finishes. This is only a sanity check: CodinGame remains the source of truth.
"""
import math
import random
import subprocess
import sys
import time

BIN = "/tmp/opencode/bot_gold"
LAPS = 3
RADIUS = 600.0
POD_RADIUS = 800.0  # sum of both force-field radii
FRICTION = 0.85
MAX_ROT = 18.0
MAX_THRUST = 100
BOOST = 650
MIN_IMPULSE = 120.0


def norm(a):
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


def trunc(v):
    return math.trunc(v)


class Pod:
    def __init__(self, x, y, cp):
        self.x, self.y = x, y
        self.vx = self.vy = 0.0
        self.angle = 0.0
        self.cp = cp
        self.passed = 0
        self.boost = True
        self.shield = 0

    def pos(self):
        return self.x, self.y


def opponent_command(p, cps):
    """Simple drift-compensated opponent so the search sees a moving target."""
    cx, cy = cps[p.cp]
    tx, ty = cx - 3 * p.vx, cy - 3 * p.vy
    want = math.atan2(ty - p.y, tx - p.x)
    err = abs(norm(want - math.radians(p.angle)))
    thrust = int(100 * max(0.0, 1.0 - err / (math.pi / 2))
                 * min(1.0, math.hypot(cx - p.x, cy - p.y) / 1800))
    return tx, ty, str(thrust)


def run(cps, seed=1, max_turns=400):
    random.seed(seed)
    n = len(cps)
    pods = []
    for i in range(4):
        x = cps[0][0] + random.randint(-200, 200)
        y = cps[0][1] + random.randint(-200, 200)
        pods.append(Pod(x, y, 0))
    for p in pods:
        d = math.atan2(cps[0][1] - p.y, cps[0][0] - p.x)
        p.angle = math.degrees(d)

    proc = subprocess.Popen(
        [BIN], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, text=True, bufsize=1,
    )
    proc.stdin.write(f"{LAPS} {n}\n")
    for x, y in cps:
        proc.stdin.write(f"{x} {y}\n")
    proc.stdin.flush()

    worst_ms = 0.0
    worst_later_ms = 0.0
    for turn in range(max_turns):
        for p in pods:
            proc.stdin.write(
                f"{round(p.x)} {round(p.y)} {trunc(p.vx)} {trunc(p.vy)} "
                f"{round(p.angle) % 360} {p.cp}\n"
            )
        proc.stdin.flush()
        t0 = time.monotonic()
        out = [proc.stdout.readline().strip() for _ in range(2)]
        elapsed = (time.monotonic() - t0) * 1000
        worst_ms = max(worst_ms, elapsed)
        if turn > 0:
            worst_later_ms = max(worst_later_ms, elapsed)
        if any(not line for line in out):
            raise RuntimeError(f"bot stopped at turn {turn}: {out}")

        commands = []
        for line in out:
            parts = line.split()
            if len(parts) != 3:
                raise RuntimeError(f"bad output {line!r}")
            commands.append((float(parts[0]), float(parts[1]), parts[2]))
        commands.extend(opponent_command(p, cps) for p in pods[2:])

        for i, (tx, ty, power) in enumerate(commands):
            p = pods[i]
            if power == "SHIELD":
                p.shield = 4
                accel = 0
            else:
                if p.shield > 0:
                    p.shield -= 1
                    accel = 0
                elif power == "BOOST":
                    if not p.boost:
                        raise RuntimeError("boost used twice")
                    p.boost = False
                    accel = BOOST
                else:
                    accel = int(power)
            want = math.atan2(ty - p.y, tx - p.x)
            diff = norm(want - math.radians(p.angle))
            p.angle = math.degrees(math.radians(p.angle)
                                   + max(-math.radians(MAX_ROT),
                                         min(math.radians(MAX_ROT), diff)))
            p.vx += math.cos(math.radians(p.angle)) * accel
            p.vy += math.sin(math.radians(p.angle)) * accel

        # Move, then resolve overlapping pairs once (approximate CSB collision).
        for p in pods:
            p.x += p.vx
            p.y += p.vy
        for i in range(4):
            for j in range(i + 1, 4):
                a, b = pods[i], pods[j]
                dx, dy = b.x - a.x, b.y - a.y
                d = math.hypot(dx, dy)
                if d >= POD_RADIUS or d < 1e-6:
                    continue
                ux, uy = dx / d, dy / d
                ma = 10 if a.shield == 4 else 1
                mb = 10 if b.shield == 4 else 1
                m = ma * mb / (ma + mb)
                imp = -2 * m * ((b.vx - a.vx) * ux + (b.vy - a.vy) * uy)
                if abs(imp) < MIN_IMPULSE:
                    imp = math.copysign(MIN_IMPULSE, imp)
                a.vx -= imp * ux / ma
                a.vy -= imp * uy / ma
                b.vx += imp * ux / mb
                b.vy += imp * uy / mb
                overlap = (POD_RADIUS - d) / 2 + 1
                a.x -= ux * overlap
                a.y -= uy * overlap
                b.x += ux * overlap
                b.y += uy * overlap

        for p in pods:
            if math.hypot(p.x - cps[p.cp][0], p.y - cps[p.cp][1]) <= RADIUS:
                p.cp = (p.cp + 1) % n
                p.passed += 1
            p.vx = trunc(p.vx * FRICTION)
            p.vy = trunc(p.vy * FRICTION)
            p.x, p.y = round(p.x), round(p.y)

        if any(p.passed >= LAPS * n for p in pods):
            proc.terminate()
            return turn + 1, worst_ms, worst_later_ms, [p.passed for p in pods]
    proc.terminate()
    return None, worst_ms, worst_later_ms, [p.passed for p in pods]


def main():
    examples = [
        [(11985, 2026), (13985, 3020), (3972, 7781)],
        [(13491, 1970), (5494, 7485), (6527, 5807), (4483, 6011)],
        [(4531, 2154), (7379, 4923), (3340, 7212), (14598, 7719),
         (10540, 5050), (13104, 2331)],
    ]
    for i, cps in enumerate(examples):
        turns, ms, later_ms, passed = run(cps, seed=i + 1)
        print(f"map {i}: cps={len(cps)} turns={turns} "
              f"first_ms={ms:.1f} worst_later_ms={later_ms:.1f} "
              f"passed={passed}")


if __name__ == "__main__":
    main()
