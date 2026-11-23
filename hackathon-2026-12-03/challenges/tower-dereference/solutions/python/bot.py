#!/usr/bin/env python3
"""Tower Dereference (CodinGame) — Python reference of the differential fuzz.

Constants from the referee (eulerscheZahl/TowerDefense, Constants.java).
Turn order: upgrades are applied at once, builds at the end of the turn, so
a tower built this turn cannot be upgraded before next turn.

Strategy (v1c), stateless: every enemy attacker walks a shortest canyon path
from one end to the other, so a gun tower is worth the number of such path
cells in its range times its damage rate; cells covered by our glue towers
count 1 + GLUE_BONUS times. Each turn we greedily buy the option with the
best marginal value per coin (build a gun or a glue tower on the best free
plateau cell, or upgrade one of our guns) until nothing affordable improves
the defence. Damage counts up to DAMAGE_CAP (overkill), and build cells on
the opponent half are discounted (simultaneous builds there are lost).
All arithmetic is integer.
"""
import sys
from collections import deque

GUN, GLUE = "GUNTOWER", "GLUETOWER"
GUN_COST = 100
GLUE_COST = 70
GLUE_RANGE = (3, 4, 5, 6)
GLUE_BONUS = 3           # slowed (speed / 4) attackers stay ~4x longer in range
GUN_DAMAGE = (5, 8, 15, 30)
GUN_RANGE = (3, 4, 5, 6)
GUN_RELOAD = (5, 4, 3, 2)
UPGRADE_COST = (50, 100, 150)
MAX_LEVEL = 3
RATE_SCALE = 60          # lcm of reload times: damage rate = dmg * 60 / reload
DAMAGE_CAP = 15          # wave HP is 12..18: damage above this is mostly overkill
CONTESTED_PERCENT = 90   # value kept for cells on the opponent half (lost build conflicts)
PROPERTIES = ("DAMAGE", "RANGE", "RELOAD")


def path_cells(grid, width, height):
    """Canyon cells lying on a shortest path between the left and right ends."""
    canyon = {(x, y) for y in range(height) for x in range(width) if grid[y][x] == "."}

    def bfs(sources):
        dist = {c: 0 for c in sources}
        queue = deque(sorted(sources))
        while queue:
            x, y = queue.popleft()
            for nx, ny in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                if (nx, ny) in canyon and (nx, ny) not in dist:
                    dist[(nx, ny)] = dist[(x, y)] + 1
                    queue.append((nx, ny))
        return dist

    left = bfs([c for c in canyon if c[0] == 0])
    right = bfs([c for c in canyon if c[0] == width - 1])
    ends = [left[c] for c in canyon if c[0] == width - 1 and c in left]
    if not ends:
        return sorted(canyon)
    total = min(ends)
    return sorted(c for c in canyon if c in left and c in right and left[c] + right[c] == total)


def in_range(px, py, x, y, radius):
    return (px - x) ** 2 + (py - y) ** 2 <= radius * radius


def damage_rate(damage_level, reload_level):
    return min(GUN_DAMAGE[damage_level], DAMAGE_CAP) * RATE_SCALE // GUN_RELOAD[reload_level]


def on_opponent_half(me, x, width):
    """Simultaneous builds on one cell go to the player whose base is closer."""
    middle = width // 2
    return x > middle if me == 0 else x < middle


def side_adjusted(value, me, x, width):
    return value * CONTESTED_PERCENT // 100 if on_opponent_half(me, x, width) else value


class Tower:
    """One of our towers (or one planned this turn: id None, not upgradable)."""

    def __init__(self, tower_id, kind, x, y, levels):
        self.id = tower_id
        self.kind = kind
        self.x = x
        self.y = y
        self.levels = levels   # [damage, range, reload]

    def radius(self, levels=None):
        table = GUN_RANGE if self.kind == GUN else GLUE_RANGE
        return table[(levels or self.levels)[1]]

    def covers(self, cell, levels=None):
        return in_range(cell[0], cell[1], self.x, self.y, self.radius(levels))


class Defence:
    """Value model: a gun is worth sum(weight(cell) * damage rate) over the
    path cells in range; a path cell covered by our glue weighs 1 + GLUE_BONUS
    because slowed attackers stay longer in range. Per-cell weights and gun
    rates are cached and refreshed after every purchase."""

    def __init__(self, path, towers):
        self.path = path
        self.towers = towers
        self.refresh()

    def refresh(self):
        glues = [t for t in self.towers if t.kind == GLUE]
        self.glued = [any(g.covers(c) for g in glues) for c in self.path]
        self.weight = [1 + GLUE_BONUS if glued else 1 for glued in self.glued]
        self.rate = [sum(damage_rate(g.levels[0], g.levels[2]) for g in self.guns() if g.covers(c))
                     for c in self.path]

    def guns(self):
        return [t for t in self.towers if t.kind == GUN]

    def gun_value(self, gun, levels=None):
        levels = levels or gun.levels
        covered = sum(w for c, w in zip(self.path, self.weight) if gun.covers(c, levels))
        return damage_rate(levels[0], levels[2]) * covered

    def glue_value(self, x, y):
        """Extra value of a level-0 glue tower at (x, y)."""
        return GLUE_BONUS * sum(r for c, r, glued in zip(self.path, self.rate, self.glued)
                                if not glued and in_range(c[0], c[1], x, y, GLUE_RANGE[0]))

    def build_gun_value(self, x, y):
        return damage_rate(0, 0) * sum(w for c, w in zip(self.path, self.weight)
                                       if in_range(c[0], c[1], x, y, GUN_RANGE[0]))


def level_of(table, text):
    """Upgrade level of a printed property value, -1 if not in the table."""
    try:
        value = float(text.replace(",", "."))
    except ValueError:
        return -1
    for level, ref in enumerate(table):
        if abs(ref - value) < 1e-9:
            return level
    return -1


def best_free_cell(grid, width, height, taken, value_of, me):
    """Plateau cell with the largest positive side-adjusted value (row order)."""
    best, best_value = None, 0
    for y in range(height):
        for x in range(width):
            if grid[y][x] != "#" or (x, y) in taken:
                continue
            value = side_adjusted(value_of(x, y), me, x, width)
            if value > best_value:
                best, best_value = (x, y), value
    return best, best_value


def gun_upgrades(defence):
    """(gain, cost, command, (tower, property)) for every gun upgrade."""
    result = []
    for gun in defence.guns():
        if gun.id is None:
            continue
        for prop in range(len(PROPERTIES)):
            level = gun.levels[prop]
            if level >= MAX_LEVEL:
                continue
            upgraded = gun.levels[:]
            upgraded[prop] += 1
            gain = defence.gun_value(gun, upgraded) - defence.gun_value(gun)
            result.append((gain, UPGRADE_COST[level], "UPGRADE %d %s" % (gun.id, PROPERTIES[prop]), (gun, prop)))
    return result


def better(a, b):
    """True when option a has a strictly higher gain per coin than b."""
    return a[0] * b[1] > b[0] * a[1]


def plan_turn(grid, width, height, path, money, towers, occupied, me):
    defence = Defence(path, towers)
    commands = []
    taken = set(occupied)
    while True:
        gun_cell, gun_gain = best_free_cell(grid, width, height, taken, defence.build_gun_value, me)
        glue_cell, glue_gain = best_free_cell(grid, width, height, taken, defence.glue_value, me)
        candidates = []
        if gun_cell is not None:
            candidates.append((gun_gain, GUN_COST, "BUILD %d %d GUNTOWER" % gun_cell, (GUN, gun_cell)))
        if glue_cell is not None:
            candidates.append((glue_gain, GLUE_COST, "BUILD %d %d GLUETOWER" % glue_cell, (GLUE, glue_cell)))
        candidates += gun_upgrades(defence)
        best = None
        for opt in candidates:
            if opt[0] > 0 and opt[1] <= money and (best is None or better(opt, best)):
                best = opt
        if best is None:
            return commands
        gain, cost, command, effect = best
        money -= cost
        commands.append(command)
        if effect[0] in (GUN, GLUE):
            kind, (x, y) = effect
            taken.add((x, y))
            towers.append(Tower(None, kind, x, y, [0, 0, 0]))
        else:
            tower, prop = effect
            tower.levels[prop] += 1
        defence.refresh()


def main():
    read = sys.stdin.readline
    me = int(read())
    width, height = map(int, read().split())
    grid = [read().rstrip("\n")[:width].ljust(width, "#") for _ in range(height)]
    path = path_cells(grid, width, height)
    while True:
        line = read()
        if not line:
            break
        money = int(line.split()[0])
        read()  # opponent money and lives
        towers, occupied = [], []
        for _ in range(int(read())):
            t = read().split()
            x, y = int(t[3]), int(t[4])
            occupied.append((x, y))
            if t[0] not in (GUN, GLUE) or int(t[2]) != me:
                continue
            if t[0] == GUN:
                levels = [level_of(GUN_DAMAGE, t[5]), level_of(GUN_RANGE, t[6]), level_of(GUN_RELOAD, t[7])]
            else:
                levels = [0, level_of(GLUE_RANGE, t[6]), 0]
            if min(levels) >= 0:
                towers.append(Tower(int(t[1]), t[0], x, y, levels))
        for _ in range(int(read())):
            read()  # attackers are not used by v1
        commands = plan_turn(grid, width, height, path, money, towers, occupied, me)
        sys.stdout.write((";".join(commands) if commands else "PASS") + "\n")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
