/* Tower Dereference (CodinGame) — C port of solutions/python/bot.py (v1c).
 * Same path model, value model, option order and tie-breaks (integer math).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_SIDE = 32, MAX_CELLS = MAX_SIDE * MAX_SIDE, MAX_TOWERS = 512, MAX_COMMANDS = 128 };
enum { KIND_GUN = 0, KIND_GLUE = 1 };
enum { GUN_COST = 100, GLUE_COST = 70 };
static const int GLUE_RANGE[4] = {3, 4, 5, 6};
enum { GLUE_BONUS = 3 };  /* slowed (speed / 4) attackers stay ~4x longer in range */
static const int GUN_DAMAGE[4] = {5, 8, 15, 30};
static const int GUN_RANGE[4] = {3, 4, 5, 6};
static const int GUN_RELOAD[4] = {5, 4, 3, 2};
static const int UPGRADE_COST[3] = {50, 100, 150};
enum { MAX_LEVEL = 3 };
enum { RATE_SCALE = 60 }; /* lcm of reload times: damage rate = dmg * 60 / reload */
enum { DAMAGE_CAP = 15 }; /* wave HP is 12..18: damage above this is mostly overkill */
enum { CONTESTED_PERCENT = 90 }; /* value kept for cells on the opponent half (lost build conflicts) */
static const char *PROPERTIES[3] = {"DAMAGE", "RANGE", "RELOAD"};

typedef struct { int x, y; } Cell;
typedef struct { int id, kind, x, y, levels[3]; } Tower; /* id -1 = planned this turn */

static int W, H;
static char GRID[MAX_SIDE][MAX_SIDE + 2];
static Cell PATH[MAX_CELLS];
static int PATH_LEN;

static void bfs_from_column(int source_x, int *dist) {
    int queue[MAX_CELLS], head = 0, tail = 0;
    for (int i = 0; i < W * H; ++i) dist[i] = -1;
    for (int y = 0; y < H; ++y)
        if (GRID[y][source_x] == '.') { dist[y * W + source_x] = 0; queue[tail++] = y * W + source_x; }
    while (head < tail) {
        int c = queue[head++], x = c % W, y = c / W;
        const int nx[4] = {x + 1, x - 1, x, x}, ny[4] = {y, y, y + 1, y - 1};
        for (int k = 0; k < 4; ++k) {
            if (nx[k] < 0 || nx[k] >= W || ny[k] < 0 || ny[k] >= H) continue;
            int n = ny[k] * W + nx[k];
            if (GRID[ny[k]][nx[k]] == '.' && dist[n] < 0) { dist[n] = dist[c] + 1; queue[tail++] = n; }
        }
    }
}

/* Canyon cells lying on a shortest path between the left and right ends. */
static void build_path(void) {
    int left[MAX_CELLS], right[MAX_CELLS], total = -1;
    bfs_from_column(0, left);
    bfs_from_column(W - 1, right);
    for (int y = 0; y < H; ++y) {
        int d = left[y * W + W - 1];
        if (GRID[y][W - 1] == '.' && d >= 0 && (total < 0 || d < total)) total = d;
    }
    PATH_LEN = 0;
    for (int x = 0; x < W; ++x)
        for (int y = 0; y < H; ++y) {
            if (GRID[y][x] != '.') continue;
            int c = y * W + x;
            int on_shortest = left[c] >= 0 && right[c] >= 0 && left[c] + right[c] == total;
            if (total < 0 || on_shortest) { PATH[PATH_LEN].x = x; PATH[PATH_LEN].y = y; ++PATH_LEN; }
        }
}

static int in_range(Cell c, int x, int y, int radius) {
    return (c.x - x) * (c.x - x) + (c.y - y) * (c.y - y) <= radius * radius;
}

static int damage_rate(int damage_level, int reload_level) {
    int damage = GUN_DAMAGE[damage_level] < DAMAGE_CAP ? GUN_DAMAGE[damage_level] : DAMAGE_CAP;
    return damage * RATE_SCALE / GUN_RELOAD[reload_level];
}

static int ME;

/* Simultaneous builds on one cell go to the player whose base is closer. */
static int on_opponent_half(int x) {
    int middle = W / 2;
    return ME == 0 ? x > middle : x < middle;
}

static int side_adjusted(int value, int x) {
    return on_opponent_half(x) ? value * CONTESTED_PERCENT / 100 : value;
}

static int covers(const Tower *t, Cell c, const int *lv) {
    int radius = (t->kind == KIND_GUN ? GUN_RANGE : GLUE_RANGE)[lv[1]];
    return in_range(c, t->x, t->y, radius);
}

/* ------------------------------------------------------------- value model */
static Tower TOWERS[MAX_TOWERS];
static int TOWER_COUNT;
static int GLUED[MAX_CELLS], WEIGHT[MAX_CELLS], RATE[MAX_CELLS];

/* Per-path-cell glue flag, weight and gun rate; refreshed after every purchase. */
static void refresh(void) {
    for (int i = 0; i < PATH_LEN; ++i) {
        GLUED[i] = 0; RATE[i] = 0;
        for (int t = 0; t < TOWER_COUNT; ++t) {
            if (!covers(&TOWERS[t], PATH[i], TOWERS[t].levels)) continue;
            if (TOWERS[t].kind == KIND_GLUE) GLUED[i] = 1;
            else RATE[i] += damage_rate(TOWERS[t].levels[0], TOWERS[t].levels[2]);
        }
        WEIGHT[i] = GLUED[i] ? 1 + GLUE_BONUS : 1;
    }
}

static int gun_value(const Tower *gun, const int *lv) {
    int covered = 0;
    for (int i = 0; i < PATH_LEN; ++i) if (covers(gun, PATH[i], lv)) covered += WEIGHT[i];
    return damage_rate(lv[0], lv[2]) * covered;
}

/* Extra value of a level-0 glue tower at (x, y). */
static int glue_value(int x, int y) {
    int sum = 0;
    for (int i = 0; i < PATH_LEN; ++i)
        if (!GLUED[i] && in_range(PATH[i], x, y, GLUE_RANGE[0])) sum += RATE[i];
    return GLUE_BONUS * sum;
}

static int build_gun_value(int x, int y) {
    int covered = 0;
    for (int i = 0; i < PATH_LEN; ++i) if (in_range(PATH[i], x, y, GUN_RANGE[0])) covered += WEIGHT[i];
    return damage_rate(0, 0) * covered;
}

/* Upgrade level of a printed property value, -1 if not in the table. */
static int level_of(const int *table, char *text) {
    for (char *p = text; *p; ++p) if (*p == ',') *p = '.';
    char *end;
    double value = strtod(text, &end);
    if (end == text) return -1;
    for (int level = 0; level < 4; ++level) if (fabs(table[level] - value) < 1e-9) return level;
    return -1;
}

/* ------------------------------------------------------------- planning */
typedef struct { int gain, cost, kind, x, y, tower, prop; } Option; /* kind: 0 gun, 1 glue, 2 upgrade */

/* 1 when option a has a strictly higher gain per coin than b. */
static int better(const Option *a, const Option *b) {
    return (long long)a->gain * b->cost > (long long)b->gain * a->cost;
}

static int best_free_cell(int taken[MAX_SIDE][MAX_SIDE], int (*value_of)(int, int), int *bx, int *by, int *best_value) {
    int found = 0;
    *best_value = 0;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            if (GRID[y][x] != '#' || taken[y][x]) continue;
            int value = side_adjusted(value_of(x, y), x);
            if (value > *best_value) { *best_value = value; *bx = x; *by = y; found = 1; }
        }
    return found;
}

static int collect_options(int taken[MAX_SIDE][MAX_SIDE], Option *out) {
    int n = 0, x = 0, y = 0, value;
    if (best_free_cell(taken, build_gun_value, &x, &y, &value)) {
        Option o = {value, GUN_COST, 0, x, y, -1, -1};
        out[n++] = o;
    }
    if (best_free_cell(taken, glue_value, &x, &y, &value)) {
        Option o = {value, GLUE_COST, 1, x, y, -1, -1};
        out[n++] = o;
    }
    for (int t = 0; t < TOWER_COUNT; ++t) {
        const Tower *gun = &TOWERS[t];
        if (gun->kind != KIND_GUN || gun->id < 0) continue;
        for (int prop = 0; prop < 3; ++prop) {
            int level = gun->levels[prop];
            if (level >= MAX_LEVEL) continue;
            int upgraded[3] = {gun->levels[0], gun->levels[1], gun->levels[2]};
            ++upgraded[prop];
            Option o = {gun_value(gun, upgraded) - gun_value(gun, gun->levels), UPGRADE_COST[level], 2, 0, 0, t, prop};
            out[n++] = o;
        }
    }
    return n;
}

static void plan_turn(int money, int taken[MAX_SIDE][MAX_SIDE], char *out) {
    static Option options[MAX_TOWERS * 3 + 2];
    out[0] = 0;
    refresh();
    while (1) {
        int n = collect_options(taken, options);
        const Option *best = NULL;
        for (int i = 0; i < n; ++i)
            if (options[i].gain > 0 && options[i].cost <= money && (!best || better(&options[i], best))) best = &options[i];
        if (!best) return;
        money -= best->cost;
        char command[64];
        if (best->kind == 2) {
            Tower *t = &TOWERS[best->tower];
            sprintf(command, "UPGRADE %d %s", t->id, PROPERTIES[best->prop]);
            ++t->levels[best->prop];
        } else {
            sprintf(command, "BUILD %d %d %s", best->x, best->y, best->kind == 0 ? "GUNTOWER" : "GLUETOWER");
            taken[best->y][best->x] = 1;
            Tower t = {-1, best->kind == 0 ? KIND_GUN : KIND_GLUE, best->x, best->y, {0, 0, 0}};
            TOWERS[TOWER_COUNT++] = t;
        }
        if (out[0]) strcat(out, ";");
        strcat(out, command);
        refresh();
    }
}

static int read_int(void) {
    char line[256];
    return fgets(line, sizeof line, stdin) ? atoi(line) : 0;
}

int main(void) {
    char line[512];
    int me = read_int();
    ME = me;
    if (!fgets(line, sizeof line, stdin) || sscanf(line, "%d %d", &W, &H) != 2) return 0;
    for (int y = 0; y < H; ++y) {
        if (!fgets(line, sizeof line, stdin)) line[0] = 0;
        for (int x = 0; x < W; ++x) {
            char ch = (x < (int)strlen(line) && line[x] != '\n' && line[x] != '\r') ? line[x] : '#';
            GRID[y][x] = ch;
        }
        GRID[y][W] = 0;
    }
    build_path();
    static char out[MAX_COMMANDS * 64];
    while (fgets(line, sizeof line, stdin)) {
        int money = atoi(line);
        if (!fgets(line, sizeof line, stdin)) break; /* opponent money and lives */
        static int taken[MAX_SIDE][MAX_SIDE];
        memset(taken, 0, sizeof taken);
        TOWER_COUNT = 0;
        int tower_count = read_int();
        for (int i = 0; i < tower_count; ++i) {
            char type[32], damage[32], range[32], reload[32];
            int id, owner, x, y;
            if (!fgets(line, sizeof line, stdin)) break;
            if (sscanf(line, "%31s %d %d %d %d %31s %31s %31s", type, &id, &owner, &x, &y, damage, range, reload) != 8) continue;
            if (y >= 0 && y < H && x >= 0 && x < W) taken[y][x] = 1;
            int is_gun = strcmp(type, "GUNTOWER") == 0, is_glue = strcmp(type, "GLUETOWER") == 0;
            if ((!is_gun && !is_glue) || owner != me) continue;
            Tower t = {id, is_gun ? KIND_GUN : KIND_GLUE, x, y, {0, 0, 0}};
            if (is_gun) { t.levels[0] = level_of(GUN_DAMAGE, damage); t.levels[1] = level_of(GUN_RANGE, range); t.levels[2] = level_of(GUN_RELOAD, reload); }
            else t.levels[1] = level_of(GLUE_RANGE, range);
            if (t.levels[0] >= 0 && t.levels[1] >= 0 && t.levels[2] >= 0) TOWERS[TOWER_COUNT++] = t;
        }
        int attacker_count = read_int();
        for (int i = 0; i < attacker_count; ++i) if (!fgets(line, sizeof line, stdin)) break; /* unused by v1b */
        plan_turn(money, taken, out);
        printf("%s\n", out[0] ? out : "PASS");
        fflush(stdout);
    }
    return 0;
}
