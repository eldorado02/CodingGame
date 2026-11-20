import sys
import math

# Mad Pod Racing - checkpoint chaser with a learned racing line.
#
# Physics: the pod rotates at most 18 degrees per turn toward its target, then
# accelerates with thrust/10 along its heading; friction is 0.85 per turn.
# Checkpoints have a radius of 600 and the next one is the only one known
# until it is passed, so the track loop is recorded during the first lap and
# used afterwards to aim through the checkpoints.
#
# Heading is recovered exactly from nextCheckpointAngle instead of being
# guessed, and the velocity comes from the position delta.

FRICTION = 0.85
TURN_RATE = math.radians(18.0)
MAX_SPEED = 800.0


def norm(a):
    while a > math.pi:
        a -= 2.0 * math.pi
    while a < -math.pi:
        a += 2.0 * math.pi
    return a


def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def unit(ax, ay, bx, by):
    dx, dy = bx - ax, by - ay
    d = math.hypot(dx, dy) or 1.0
    return dx / d, dy / d


def main():
    x, y, cx, cy, cpd, cpang = [int(v) for v in input().split()]
    input()  # opponent position (collisions are handled by the referee)

    track = [(cx, cy)]          # checkpoints seen so far, in order
    seen = {(cx, cy)}
    loop_complete = False
    last_idx = 0
    boost_left = True
    prev_x, prev_y = x, y
    heading = math.atan2(cy - y, cx - x) - math.radians(cpang)

    while True:
        idx = track.index((cx, cy)) if (cx, cy) in seen else len(track) - 1
        # Aim at the checkpoint, or a bit past it along the next leg when the
        # loop is known, so that the pod keeps its speed through the corner.
        tx, ty = cx, cy
        if loop_complete:
            nx, ny = track[(idx + 1) % len(track)]
            ux, uy = unit(cx, cy, nx, ny)
            lead = min(900.0, 0.25 * math.hypot(nx - cx, ny - cy))
            tx, ty = cx + ux * lead, cy + uy * lead

        ang = abs(cpang)
        if ang < 45:
            thrust = 100
        elif ang < 75:
            thrust = 70
        elif ang < 110:
            thrust = 35
        elif ang < 150:
            thrust = 12
        else:
            thrust = 0

        speed = math.hypot((x - prev_x) * FRICTION, (y - prev_y) * FRICTION)
        if boost_left and ang < 8 and cpd > 3500 and speed > 0.7 * MAX_SPEED:
            boost_left = False
            print("%d %d BOOST" % (int(tx), int(ty)))
        else:
            print("%d %d %d" % (int(tx), int(ty), int(thrust)))
        sys.stdout.flush()

        prev_x, prev_y = x, y
        x, y, cx, cy, cpd, cpang = [int(v) for v in input().split()]
        input()
        heading = math.atan2(cy - y, cx - x) - math.radians(cpang)
        if (cx, cy) not in seen:
            seen.add((cx, cy))
            track.append((cx, cy))
            last_idx = len(track) - 1
        else:
            last_idx = track.index((cx, cy))
            if last_idx == 0 and len(track) > 2:
                loop_complete = True


if __name__ == "__main__":
    main()
