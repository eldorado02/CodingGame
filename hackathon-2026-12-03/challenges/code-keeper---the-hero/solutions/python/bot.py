#!/usr/bin/env python3
"""Code Keeper - The Hero (CodinGame) — Python reference of the differential fuzz.

Facts from the referee (acatai/CodeKeeper-TheHero): 16x12 maze, the hero sees
every entity at Chebyshev distance <= 3, 150 turns; exit +10000, treasure
+100, a kill gives the monster's initial health, damage taken is subtracted
from the score, death -1000. Four heroes play identical mazes; best score wins.

Strategy (v1), rule based with memory:
- remember seen cells (floor / obstacle), items and the exit;
- combat: sword a cardinal neighbour, bow a killable monster at range,
  scythe / hammer a diagonal neighbour, chase a monster that already hits
  us (monsters stop on a diagonal to attack), never wait for the others;
- movement (one BFS step on known floor): exit when the deadline or low
  health forces it, else nearest treasure / chest / frontier (potion when
  hurt), else exit; when hurt, first try a path avoiding monster views.
"""
import sys
from collections import deque

WIDTH, HEIGHT = 16, 12
CELLS = WIDTH * HEIGHT
TURN_LIMIT = 150
HERO_FOV = 3
UNKNOWN, FLOOR, WALL = 0, 1, 2
EXIT, OBSTACLE, TREASURE, POTION = 0, 1, 2, 3
CHESTS = (4, 5, 6)
BOX = 7
MONSTER_TYPES = (8, 9, 10, 11)
VIEW_RANGE = {8: 1, 9: 2, 10: 2, 11: 3}
ATTACK_RANGE = {8: 1, 9: 1, 10: 2, 11: 1}
DAMAGE = {8: 1, 9: 2, 10: 2, 11: 3}
SWORD, HAMMER, SCYTHE, BOW = 0, 1, 2, 3
WEAPON_DAMAGE = (10, 6, 7, 8)
BOW_RANGE = 3
EXIT_MARGIN = 6        # spare turns kept when heading for the exit
LOW_HEALTH = 10        # a potion is worth a detour below this
CRITICAL_HEALTH = 6    # leave through the exit if it is known
CAUTION_HEALTH = 10    # below this, walk around cells that monsters can see
CARDINAL = ((-1, 0), (1, 0), (0, -1), (0, 1))


def cell_of(x, y):
    return y * WIDTH + x


def xy_of(cell):
    return cell % WIDTH, cell // WIDTH


def chebyshev(a, b):
    (ax, ay), (bx, by) = xy_of(a), xy_of(b)
    return max(abs(ax - bx), abs(ay - by))


def is_cardinal_neighbour(a, b):
    (ax, ay), (bx, by) = xy_of(a), xy_of(b)
    return abs(ax - bx) + abs(ay - by) == 1


class Memory:
    def __init__(self):
        self.known = [UNKNOWN] * CELLS
        self.items = {}       # cell -> item type
        self.monsters = {}    # cell -> (type, health); monsters move only when they see the hero, i.e. inside our view
        self.exit = -1
        self.turn = 0

    def update(self, hero, entities):
        """Every in-bounds cell within the field of view is now known; returns
        the remembered monsters (cell, type, health) by increasing cell."""
        hx, hy = xy_of(hero)
        visible = []
        for y in range(max(0, hy - HERO_FOV), min(HEIGHT, hy + HERO_FOV + 1)):
            for x in range(max(0, hx - HERO_FOV), min(WIDTH, hx + HERO_FOV + 1)):
                visible.append(cell_of(x, y))
        for c in visible:
            self.known[c] = FLOOR
            self.items.pop(c, None)
            self.monsters.pop(c, None)
        for x, y, etype, value in entities:
            if not (0 <= x < WIDTH and 0 <= y < HEIGHT):
                continue
            c = cell_of(x, y)
            if etype == OBSTACLE:
                self.known[c] = WALL
            elif etype == EXIT or etype == TREASURE or etype == POTION or etype in CHESTS:
                self.items[c] = etype
                if etype == EXIT:
                    self.exit = c
            elif etype == BOX or etype in MONSTER_TYPES:
                self.monsters[c] = (etype, value)
        return [(c,) + self.monsters[c] for c in sorted(self.monsters)]


def bfs(memory, start, blocked, dead_ends):
    """Distances and first steps over known floor, avoiding `blocked` cells;
    `dead_ends` cells can be reached but the path never goes through them."""
    dist = {start: 0}
    first = {start: start}
    queue = deque([start])
    while queue:
        c = queue.popleft()
        x, y = xy_of(c)
        for dx, dy in CARDINAL:
            nx, ny = x + dx, y + dy
            if not (0 <= nx < WIDTH and 0 <= ny < HEIGHT):
                continue
            n = cell_of(nx, ny)
            if n in dist or memory.known[n] != FLOOR or n in blocked:
                continue
            dist[n] = dist[c] + 1
            first[n] = n if c == start else first[c]
            if n not in dead_ends:
                queue.append(n)
    return dist, first


def is_frontier(memory, c):
    x, y = xy_of(c)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            nx, ny = x + dx, y + dy
            if 0 <= nx < WIDTH and 0 <= ny < HEIGHT and memory.known[cell_of(nx, ny)] == UNKNOWN:
                return True
    return False


def threatening(hero, monsters):
    """Real monsters that see the hero (they act this turn)."""
    return [m for m in monsters if m[1] in MONSTER_TYPES and chebyshev(m[0], hero) <= VIEW_RANGE[m[1]]]


def kill_priority(monster, damage):
    """Higher is better: kills first, then the most dangerous, then weakest."""
    _, etype, hp = monster
    return (1 if hp <= damage else 0, DAMAGE[etype], -hp)


def pick(monsters, damage):
    best = None
    for m in monsters:
        if best is None or kill_priority(m, damage) > kill_priority(best, damage):
            best = m
    return best


def combat(hero, charges, monsters):
    """("ACT", command), ("CHASE", monster cell) or None."""
    threats = threatening(hero, monsters)
    if not threats:
        return None
    adjacent = [m for m in threats if is_cardinal_neighbour(m[0], hero)]
    if adjacent:
        target = pick(adjacent, WEAPON_DAMAGE[SWORD])
        return "ACT", "ATTACK %d %d %d" % ((SWORD,) + xy_of(target[0]))
    if charges[BOW] > 0:
        killable = [m for m in threats if m[2] <= WEAPON_DAMAGE[BOW] and chebyshev(m[0], hero) <= BOW_RANGE]
        if killable:
            target = pick(killable, WEAPON_DAMAGE[BOW])
            return "ACT", "ATTACK %d %d %d" % ((BOW,) + xy_of(target[0]))
    diagonal = [m for m in threats if chebyshev(m[0], hero) == 1]
    if diagonal:
        target = pick(diagonal, WEAPON_DAMAGE[SCYTHE])
        if charges[SCYTHE] > 0 and target[2] <= WEAPON_DAMAGE[SCYTHE]:
            return "ACT", "ATTACK %d %d %d" % ((SCYTHE,) + xy_of(target[0]))
        if charges[HAMMER] > 0:
            return "ACT", "ATTACK %d %d %d" % ((HAMMER,) + xy_of(target[0]))
    hitting = [m for m in threats if chebyshev(m[0], hero) <= ATTACK_RANGE[m[1]]]
    if hitting:
        return "CHASE", pick(hitting, WEAPON_DAMAGE[SWORD])[0]  # reach a cardinal side
    return None


def choose_target(memory, hero, health, dist):
    """Cell to walk to, or -1."""
    remaining = TURN_LIMIT - memory.turn
    exit_reachable = memory.exit >= 0 and memory.exit in dist
    if exit_reachable and (remaining <= dist[memory.exit] + EXIT_MARGIN or health <= CRITICAL_HEALTH):
        return memory.exit
    wanted = []
    for c, etype in memory.items.items():
        if etype == TREASURE or etype in CHESTS or (etype == POTION and health <= LOW_HEALTH):
            wanted.append(c)
    wanted += [c for c in dist if c != hero and is_frontier(memory, c)]
    best, best_d = -1, None
    for c in sorted(set(wanted)):
        if c in dist and c != memory.exit and (best_d is None or dist[c] < best_d):
            best, best_d = c, dist[c]
    if best >= 0:
        return best
    return memory.exit if exit_reachable else -1


def watched_cells(monsters):
    """Floor cells inside the view range of a visible monster."""
    cells = set()
    for c, etype, _ in monsters:
        if etype not in MONSTER_TYPES:
            continue
        mx, my = xy_of(c)
        r = VIEW_RANGE[etype]
        for y in range(max(0, my - r), min(HEIGHT, my + r + 1)):
            for x in range(max(0, mx - r), min(WIDTH, mx + r + 1)):
                cells.add(cell_of(x, y))
    return cells


def plan_step(memory, hero, health, monsters, chase, extra_blocked, through_monsters):
    """First step towards the chase cell or the chosen target, or -1.
    With through_monsters, monster cells are walkable (the step is a sword hit)."""
    standing = set() if through_monsters else {m[0] for m in monsters if m[1] != BOX}
    blocked = standing | extra_blocked
    blocked.discard(hero)
    dead_ends = set(blocked) | ({memory.exit} if memory.exit >= 0 else set())
    if chase >= 0:
        blocked.discard(chase)
        dead_ends.add(chase)
    dist, first = bfs(memory, hero, blocked, dead_ends)
    target = chase if chase >= 0 and chase in dist else choose_target(memory, hero, health, dist)
    if target < 0 or target == hero:
        return -1
    return first[target]


def decide(memory, hero, health, charges, monsters):
    fight = combat(hero, charges, monsters)
    if fight and fight[0] == "ACT":
        return fight[1]
    chase = fight[1] if fight else -1
    step = -1
    if health < CAUTION_HEALTH and chase < 0:
        step = plan_step(memory, hero, health, monsters, chase, watched_cells(monsters), False)
    if step < 0:
        step = plan_step(memory, hero, health, monsters, chase, set(), False)
    if step < 0:  # a monster that never moves may block the only corridor
        step = plan_step(memory, hero, health, monsters, chase, set(), True)
    return "MOVE %d %d" % xy_of(step if step >= 0 else hero)


def main():
    memory = Memory()
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        x, y, health, _score, hammer, scythe, bow = map(int, line.split()[:7])
        entities = [tuple(map(int, sys.stdin.readline().split()[:4])) for _ in range(int(sys.stdin.readline()))]
        hero = cell_of(x, y)
        monsters = memory.update(hero, entities)
        action = decide(memory, hero, health, (0, hammer, scythe, bow), monsters)
        sys.stderr.write("t=%d hp=%d exit=%d \n" % (memory.turn, health, memory.exit))
        memory.turn += 1
        sys.stdout.write(action + "\n")
        sys.stdout.flush()


if __name__ == "__main__":
    main()
