#!/usr/bin/env python3
"""Local physics simulator for tuning Mad Pod Racing heuristics.

It reproduces the expert rules (rotate, accelerate, move, friction, rounding)
and counts the turns needed to complete three laps. Use it to compare parameter
sets offline; CodinGame test games remain the source of truth.
"""
import math
import re
import sys

FRICTION = 0.85
TURN = math.radians(18.0)
RADIUS = 600.0
LAPS = 3


def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def norm(a):
    while a > math.pi:
        a -= 2.0 * math.pi
    while a < -math.pi:
        a += 2.0 * math.pi
    return a


def simulate(cps, drift_k=3.0, dist_k=2.0, boost=True, boost_delay=25,
             boost_dist=3500.0, boost_speed=200.0, boost_angle=5.0,
             max_turns=3000):
    n = len(cps)
    target_i = 1 % n
    # Starting on checkpoint 0, three laps require n*LAPS further checkpoints,
    # the last one being checkpoint 0 again.
    last_target = n * LAPS + 1
    x, y = float(cps[0][0]), float(cps[0][1])
    heading = math.atan2(cps[target_i][1] - y, cps[target_i][0] - x)
    vx = vy = 0.0
    dx = dy = 0.0
    boost_left = True
    turn = 0

    while target_i < last_target and turn < max_turns:
        cx, cy = cps[target_i % n]
        tx = cx - drift_k * dx
        ty = cy - drift_k * dy
        d = math.hypot(cx - x, cy - y)
        bearing = math.atan2(cy - y, cx - x)
        err = abs(math.degrees(norm(bearing - heading)))
        speed = math.hypot(dx, dy)

        thrust = int(100.0 * clamp(1.0 - err / 90.0, 0.0, 1.0)
                     * clamp(d / (dist_k * RADIUS), 0.0, 1.0))
        use_boost = (boost and boost_left and turn >= boost_delay
                     and err < boost_angle and d > boost_dist
                     and speed > boost_speed)
        if use_boost:
            boost_left = False
        accel = 650.0 if use_boost else thrust

        desired = math.atan2(ty - y, tx - x)
        diff = norm(desired - heading)
        if turn != 0:
            diff = clamp(diff, -TURN, TURN)
        heading += diff

        vx += math.cos(heading) * accel
        vy += math.sin(heading) * accel
        x += vx
        y += vy
        turn += 1

        if math.hypot(x - cx, y - cy) <= RADIUS:
            target_i += 1
            if target_i < last_target:
                cx, cy = cps[target_i % n]

        dx, dy = vx, vy
        vx = int(vx * FRICTION)
        vy = int(vy * FRICTION)
        x = float(round(x))
        y = float(round(y))

    return turn if target_i >= last_target else None


def maps_from_logs(paths):
    out = []
    for path in paths:
        text = open(path).read()
        m = re.search(r"^    map=(.*)$", text, re.M)
        if m:
            nums = [int(v) for v in m.group(1).split()]
            out.append(list(zip(nums[::2], nums[1::2])))
    return out


def main():
    paths = sys.argv[1:]
    maps = maps_from_logs(paths) if paths else [
        [(11985, 2026), (13985, 3020), (3972, 7781)],
        [(13491, 1970), (5494, 7485), (6527, 5807), (4483, 6011)],
    ]
    for cps in maps:
        print(len(cps), "cps", simulate(cps), "turns",
              "no boost:", simulate(cps, boost=False))


if __name__ == "__main__":
    main()
