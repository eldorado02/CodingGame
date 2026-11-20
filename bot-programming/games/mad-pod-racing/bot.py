import sys
import math

# Mad Pod Racing - drift-compensated heuristic.
#
# The pod rotates at most 18 degrees toward its target, accelerates by the
# requested thrust and keeps 85% of its speed each turn. Aiming at the
# checkpoint itself makes a fast pod drift past tight turns, so the target is
# shifted backwards by 3 * last frame displacement. Thrust is reduced both by
# misalignment and by proximity to the checkpoint.
#
# This is the heuristic that carried a well-known bot from Bronze to Gold.

FRICTION = 0.85
BOOST_ACCEL = 650.0
CHECKPOINT_RADIUS = 600.0
DRIFT_K = 5.0
DIST_K = 3.0
# BOOST is available from Wood 1 on (one per race).
ALLOW_BOOST = True
BOOST_ANGLE = 5.0
BOOST_DIST = 3500.0
# Bosses wait a few turns before boosting, then fire as soon as they are
# aligned on a long leg. Boosting on turn 1 wastes most of the acceleration.
BOOST_MIN_TURN = 15
BOOST_SPEED = 200.0
DEBUG = False


def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def norm(a):
    while a > math.pi:
        a -= 2.0 * math.pi
    while a < -math.pi:
        a += 2.0 * math.pi
    return a


def read_turn():
    vals = [int(v) for v in input().split()]
    if len(vals) >= 6:
        x, y, cx, cy, d, ang = vals[:6]
        try:
            input()  # opponent position
        except EOFError:
            pass
        return x, y, cx, cy, d, ang, True
    x, y, cx, cy = vals[:4]
    return x, y, cx, cy, None, None, False


def main():
    x, y, cx, cy, d, inp_ang, has_angle = read_turn()
    prev_x, prev_y = x, y
    boost_left = True
    turn = 0

    while True:
        # Position delta is the displacement of the last frame, before the
        # 15% friction that produced the current speed.
        dx, dy = x - prev_x, y - prev_y
        bearing = math.atan2(cy - y, cx - x)
        if inp_ang is not None:
            heading = bearing - math.radians(inp_ang)
            err = abs(float(inp_ang))
        elif dx or dy:
            heading = math.atan2(dy, dx)
            err = abs(math.degrees(norm(bearing - heading)))
        else:
            heading = bearing
            err = 0.0
        if d is None:
            d = math.hypot(cx - x, cy - y)

        misalign = clamp(1.0 - err / 90.0, 0.0, 1.0)
        proximity = clamp(d / (DIST_K * CHECKPOINT_RADIUS), 0.0, 1.0)
        thrust = int(100.0 * misalign * proximity)

        tx = cx - DRIFT_K * dx
        ty = cy - DRIFT_K * dy

        speed = math.hypot(dx, dy)
        boosting = (ALLOW_BOOST and boost_left and turn >= BOOST_MIN_TURN
                    and err < BOOST_ANGLE and d > BOOST_DIST
                    and speed > BOOST_SPEED)
        if boosting:
            boost_left = False
            print("%d %d BOOST" % (int(round(tx)), int(round(ty))))
        else:
            print("%d %d %d" % (int(round(tx)), int(round(ty)), thrust))
        sys.stdout.flush()

        if DEBUG:
            print("T%d xy=(%d,%d) cp=(%d,%d) d=%d err=%.0f dv=(%d,%d) th=%d"
                  % (turn, x, y, cx, cy, d, err, dx, dy, thrust), file=sys.stderr)
        turn += 1

        prev_x, prev_y = x, y
        x, y, cx, cy, d, inp_ang, has_angle = read_turn()


if __name__ == "__main__":
    main()
