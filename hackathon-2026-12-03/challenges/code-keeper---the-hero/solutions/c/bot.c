/* Code Keeper - The Hero — C port of cpp/bot.cpp (same rules, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { WIDTH = 16, HEIGHT = 12, CELLS = WIDTH * HEIGHT };
enum { TURN_LIMIT = 150, HERO_FOV = 3 };
enum { UNKNOWN = 0, FLOOR = 1, WALL = 2 };
enum { EXIT = 0, OBSTACLE = 1, TREASURE = 2, POTION = 3 };
enum { FIRST_CHEST = 4, LAST_CHEST = 6, BOX = 7, FIRST_MONSTER = 8, LAST_MONSTER = 11 };
enum { SWORD = 0, HAMMER = 1, SCYTHE = 2, BOW = 3, WEAPONS = 4 };
enum { BOW_RANGE = 3 };
enum { EXIT_MARGIN = 6 };      /* spare turns kept when heading for the exit */
enum { LOW_HEALTH = 10 };      /* a potion is worth a detour below this */
enum { CRITICAL_HEALTH = 6 };  /* leave through the exit if it is known */
enum { CAUTION_HEALTH = 10 };  /* below this, walk around cells monsters can see */
enum { NONE = -1, MAX_ENTITIES = 4 * CELLS, COMMAND_SIZE = 64 };
static const int VIEW_RANGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3};
static const int ATTACK_RANGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1};
static const int DAMAGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3};
static const int WEAPON_DAMAGE[] = {10, 6, 7, 8};
static const int CARDINAL[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

static int max_i(int a, int b) { return a > b ? a : b; }
static int min_i(int a, int b) { return a < b ? a : b; }
static int cell_of(int x, int y) { return y * WIDTH + x; }
static int x_of(int c) { return c % WIDTH; }
static int y_of(int c) { return c / WIDTH; }
static bool inside(int x, int y) { return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT; }
static bool is_monster_type(int t) { return t >= FIRST_MONSTER && t <= LAST_MONSTER; }
static bool is_chest(int t) { return t >= FIRST_CHEST && t <= LAST_CHEST; }
static int chebyshev(int a, int b) { return max_i(abs(x_of(a) - x_of(b)), abs(y_of(a) - y_of(b))); }
static bool is_cardinal_neighbour(int a, int b) { return abs(x_of(a) - x_of(b)) + abs(y_of(a) - y_of(b)) == 1; }

typedef struct { int cell, type, hp; } Monster;
/* Monsters move only when they see the hero, i.e. inside our view: remember them. */
typedef struct { int known[CELLS], items[CELLS], monster_type[CELLS], monster_hp[CELLS], exit, turn; } Memory;
typedef struct { int dist[CELLS], first[CELLS]; } Paths;

static void memory_init(Memory *m) {
    memset(m->known, 0, sizeof m->known);
    for (int c = 0; c < CELLS; ++c) m->items[c] = m->monster_type[c] = NONE;
    m->exit = NONE;
    m->turn = 0;
}

/* Every in-bounds cell within the field of view is now known; fills the
 * remembered monsters by increasing cell and returns their count. */
static int memory_update(Memory *m, int hero, int (*entities)[4], int count, Monster *monsters) {
    int hx = x_of(hero), hy = y_of(hero), n = 0;
    for (int y = max_i(0, hy - HERO_FOV); y < min_i(HEIGHT, hy + HERO_FOV + 1); ++y)
        for (int x = max_i(0, hx - HERO_FOV); x < min_i(WIDTH, hx + HERO_FOV + 1); ++x) {
            m->known[cell_of(x, y)] = FLOOR;
            m->items[cell_of(x, y)] = NONE;
            m->monster_type[cell_of(x, y)] = NONE;
        }
    for (int i = 0; i < count; ++i) {
        int *e = entities[i];
        if (!inside(e[0], e[1])) continue;
        int c = cell_of(e[0], e[1]), t = e[2];
        if (t == OBSTACLE) m->known[c] = WALL;
        else if (t == EXIT || t == TREASURE || t == POTION || is_chest(t)) {
            m->items[c] = t;
            if (t == EXIT) m->exit = c;
        } else if (t == BOX || is_monster_type(t)) {
            m->monster_type[c] = t;
            m->monster_hp[c] = e[3];
        }
    }
    for (int c = 0; c < CELLS; ++c)
        if (m->monster_type[c] != NONE) monsters[n++] = (Monster){c, m->monster_type[c], m->monster_hp[c]};
    return n;
}

/* Distances and first steps over known floor, avoiding blocked cells;
 * dead-end cells can be reached but the path never goes through them. */
static void bfs(const Memory *m, int start, const bool *blocked, const bool *dead_end, Paths *p) {
    int queue[CELLS], head = 0, tail = 0;
    for (int c = 0; c < CELLS; ++c) p->dist[c] = NONE;
    p->dist[start] = 0;
    p->first[start] = start;
    queue[tail++] = start;
    while (head < tail) {
        int c = queue[head++];
        for (int k = 0; k < 4; ++k) {
            int nx = x_of(c) + CARDINAL[k][0], ny = y_of(c) + CARDINAL[k][1];
            if (!inside(nx, ny)) continue;
            int n = cell_of(nx, ny);
            if (p->dist[n] != NONE || m->known[n] != FLOOR || blocked[n]) continue;
            p->dist[n] = p->dist[c] + 1;
            p->first[n] = c == start ? n : p->first[c];
            if (!dead_end[n]) queue[tail++] = n;
        }
    }
}

static bool is_frontier(const Memory *m, int c) {
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = x_of(c) + dx, ny = y_of(c) + dy;
            if (inside(nx, ny) && m->known[cell_of(nx, ny)] == UNKNOWN) return true;
        }
    return false;
}

/* Higher is better: kills first, then the most dangerous, then the weakest. */
static bool better_target(const Monster *a, const Monster *b, int damage) {
    int ka = a->hp <= damage, kb = b->hp <= damage;
    if (ka != kb) return ka > kb;
    if (DAMAGE[a->type] != DAMAGE[b->type]) return DAMAGE[a->type] > DAMAGE[b->type];
    return a->hp < b->hp;
}

static const Monster *pick(const Monster **candidates, int n, int damage) {
    const Monster *best = NULL;
    for (int i = 0; i < n; ++i)
        if (!best || better_target(candidates[i], best, damage)) best = candidates[i];
    return best;
}

static void attack(char *out, int weapon, int cell) {
    snprintf(out, COMMAND_SIZE, "ATTACK %d %d %d", weapon, x_of(cell), y_of(cell));
}
static void move_to(char *out, int cell) { snprintf(out, COMMAND_SIZE, "MOVE %d %d", x_of(cell), y_of(cell)); }

typedef enum { NO_FIGHT, ACT, CHASE } FightKind;

/* Writes the command for ACT, the monster cell in *chase for CHASE. */
static FightKind combat(int hero, const int *charges, const Monster *monsters, int count,
                        char *command, int *chase) {
    const Monster *threats[MAX_ENTITIES], *adjacent[MAX_ENTITIES], *killable[MAX_ENTITIES];
    const Monster *diagonal[MAX_ENTITIES], *hitting[MAX_ENTITIES];
    int nt = 0, na = 0, nk = 0, nd = 0, nh = 0;
    for (int i = 0; i < count; ++i)
        if (is_monster_type(monsters[i].type) && chebyshev(monsters[i].cell, hero) <= VIEW_RANGE[monsters[i].type])
            threats[nt++] = &monsters[i];
    if (!nt) return NO_FIGHT;
    for (int i = 0; i < nt; ++i) {
        const Monster *m = threats[i];
        int d = chebyshev(m->cell, hero);
        if (is_cardinal_neighbour(m->cell, hero)) adjacent[na++] = m;
        if (m->hp <= WEAPON_DAMAGE[BOW] && d <= BOW_RANGE) killable[nk++] = m;
        if (d == 1) diagonal[nd++] = m;
        if (d <= ATTACK_RANGE[m->type]) hitting[nh++] = m;
    }
    if (na) { attack(command, SWORD, pick(adjacent, na, WEAPON_DAMAGE[SWORD])->cell); return ACT; }
    if (charges[BOW] > 0 && nk) { attack(command, BOW, pick(killable, nk, WEAPON_DAMAGE[BOW])->cell); return ACT; }
    if (nd) {
        const Monster *target = pick(diagonal, nd, WEAPON_DAMAGE[SCYTHE]);
        if (charges[SCYTHE] > 0 && target->hp <= WEAPON_DAMAGE[SCYTHE]) {
            attack(command, SCYTHE, target->cell);
            return ACT;
        }
        if (charges[HAMMER] > 0) { attack(command, HAMMER, target->cell); return ACT; }
    }
    if (nh) { *chase = pick(hitting, nh, WEAPON_DAMAGE[SWORD])->cell; return CHASE; }  /* reach a cardinal side */
    return NO_FIGHT;
}

/* Cell to walk to, or NONE. */
static int choose_target(const Memory *m, int hero, int health, const Paths *p) {
    int remaining = TURN_LIMIT - m->turn;
    bool exit_reachable = m->exit != NONE && p->dist[m->exit] != NONE;
    if (exit_reachable && (remaining <= p->dist[m->exit] + EXIT_MARGIN || health <= CRITICAL_HEALTH)) return m->exit;
    int best = NONE;
    for (int c = 0; c < CELLS; ++c) {
        if (p->dist[c] == NONE || c == m->exit) continue;
        int t = m->items[c];
        bool wanted = t == TREASURE || is_chest(t) || (t == POTION && health <= LOW_HEALTH) ||
                      (c != hero && is_frontier(m, c));
        if (wanted && (best == NONE || p->dist[c] < p->dist[best])) best = c;
    }
    if (best != NONE) return best;
    return exit_reachable ? m->exit : NONE;
}

/* Floor cells inside the view range of a visible monster. */
static void mark_watched(const Monster *monsters, int count, bool *watched) {
    for (int i = 0; i < count; ++i) {
        if (!is_monster_type(monsters[i].type)) continue;
        int mx = x_of(monsters[i].cell), my = y_of(monsters[i].cell), r = VIEW_RANGE[monsters[i].type];
        for (int y = max_i(0, my - r); y < min_i(HEIGHT, my + r + 1); ++y)
            for (int x = max_i(0, mx - r); x < min_i(WIDTH, mx + r + 1); ++x) watched[cell_of(x, y)] = true;
    }
}

/* First step towards the chase cell or the chosen target, or NONE.
 * With through_monsters, monster cells are walkable (the step is a sword hit). */
static int plan_step(const Memory *m, int hero, int health, const Monster *monsters, int count, int chase,
                     const bool *extra_blocked, bool through_monsters) {
    bool blocked[CELLS], dead_end[CELLS];
    memcpy(blocked, extra_blocked, sizeof blocked);
    if (!through_monsters)
        for (int i = 0; i < count; ++i)
            if (monsters[i].type != BOX) blocked[monsters[i].cell] = true;
    blocked[hero] = false;
    memcpy(dead_end, blocked, sizeof dead_end);
    if (m->exit != NONE) dead_end[m->exit] = true;
    if (chase != NONE) { blocked[chase] = false; dead_end[chase] = true; }
    Paths p;
    bfs(m, hero, blocked, dead_end, &p);
    int target = chase != NONE && p.dist[chase] != NONE ? chase : choose_target(m, hero, health, &p);
    if (target == NONE || target == hero) return NONE;
    return p.first[target];
}

static void decide(const Memory *m, int hero, int health, const int *charges, const Monster *monsters, int count,
                   char *command) {
    int chase = NONE;
    if (combat(hero, charges, monsters, count, command, &chase) == ACT) return;
    bool none_blocked[CELLS] = {false};
    int step = NONE;
    if (health < CAUTION_HEALTH && chase == NONE) {
        bool watched[CELLS] = {false};
        mark_watched(monsters, count, watched);
        step = plan_step(m, hero, health, monsters, count, chase, watched, false);
    }
    if (step == NONE) step = plan_step(m, hero, health, monsters, count, chase, none_blocked, false);
    if (step == NONE) /* a monster that never moves may block the only corridor */
        step = plan_step(m, hero, health, monsters, count, chase, none_blocked, true);
    move_to(command, step != NONE ? step : hero);
}

int main(void) {
    static Memory memory;
    static int entities[MAX_ENTITIES][4];
    static Monster monsters[MAX_ENTITIES];
    memory_init(&memory);
    int x, y, health, score, charges[WEAPONS] = {0}, count;
    while (scanf("%d %d %d %d %d %d %d", &x, &y, &health, &score, &charges[HAMMER], &charges[SCYTHE],
                 &charges[BOW]) == 7) {
        if (scanf("%d", &count) != 1) return 0;
        if (count > MAX_ENTITIES) count = MAX_ENTITIES;
        for (int i = 0; i < count; ++i)
            if (scanf("%d %d %d %d", &entities[i][0], &entities[i][1], &entities[i][2], &entities[i][3]) != 4)
                return 0;
        int hero = cell_of(x, y);
        int n = memory_update(&memory, hero, entities, count, monsters);
        char command[COMMAND_SIZE];
        decide(&memory, hero, health, charges, monsters, n, command);
        fprintf(stderr, "t=%d hp=%d exit=%d\n", memory.turn, health, memory.exit);
        ++memory.turn;
        printf("%s\n", command);
        fflush(stdout);
    }
    return 0;
}
