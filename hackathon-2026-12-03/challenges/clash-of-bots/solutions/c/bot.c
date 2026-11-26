/* Clash of Bots — C port of cpp/bot.cpp (same rules, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum { SIZE = 5, CENTER = 2 };
enum { EXPLOSION_DAMAGE = 4, KILLS_TO_EXPLODE = 2 };
enum { NONE = -1, DIRECTION_COUNT = 4 };
typedef struct { const char *name; int dr, dc; } Direction;
static const Direction DIRECTIONS[DIRECTION_COUNT] = {
    {"UP", -1, 0}, {"DOWN", 1, 0}, {"LEFT", 0, -1}, {"RIGHT", 0, 1}};  /* tie-break order */
enum { DRIFT = 0 };  /* UP: the whole army keeps its shape */

typedef int Grid[SIZE][SIZE];

static int manhattan(int r1, int c1, int r2, int c2) { return abs(r1 - r2) + abs(c1 - c2); }
static bool is_center(int r, int c) { return r == CENTER && c == CENTER; }

static bool explosion_pays(const Grid g) {
    int kills = 0;
    for (int r = CENTER - 1; r <= CENTER + 1; ++r)
        for (int c = CENTER - 1; c <= CENTER + 1; ++c) {
            if (is_center(r, c)) continue;
            if (g[r][c] > 0) return false;
            if (g[r][c] < 0 && -g[r][c] <= EXPLOSION_DAMAGE) ++kills;
        }
    return kills >= KILLS_TO_EXPLODE;
}

static int adjacent_attack(const Grid g) {
    int best = NONE, best_hp = 0;
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
        int v = g[CENTER + DIRECTIONS[d].dr][CENTER + DIRECTIONS[d].dc];
        if (v < 0 && (best == NONE || -v < best_hp)) { best = d; best_hp = -v; }
    }
    return best;
}

/* Side cell (empty) that the most enemies at distance 2 can move into. */
static int ambush_attack(const Grid g) {
    int best = NONE, best_count = 0;
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
        int r = CENTER + DIRECTIONS[d].dr, c = CENTER + DIRECTIONS[d].dc;
        if (g[r][c] != 0) continue;
        int count = 0;
        for (int er = 0; er < SIZE; ++er)
            for (int ec = 0; ec < SIZE; ++ec)
                if (g[er][ec] < 0 && manhattan(er, ec, CENTER, CENTER) == 2 && manhattan(er, ec, r, c) == 1) ++count;
        if (count > best_count) { best = d; best_count = count; }
    }
    return best;
}

static bool any_enemy(const Grid g) {
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            if (g[r][c] < 0) return true;
    return false;
}

/* First free side cell that brings us closer to the nearest enemy. */
static int approach(const Grid g) {
    int er = NONE, ec = NONE;
    for (int r = 0; r < SIZE; ++r) /* nearest, then smallest row, then column */
        for (int c = 0; c < SIZE; ++c)
            if (g[r][c] < 0 && (er == NONE || manhattan(r, c, CENTER, CENTER) < manhattan(er, ec, CENTER, CENTER))) {
                er = r;
                ec = c;
            }
    if (er == NONE) return NONE;
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
        int r = CENTER + DIRECTIONS[d].dr, c = CENTER + DIRECTIONS[d].dc;
        if (g[r][c] == 0 && manhattan(er, ec, r, c) < manhattan(er, ec, CENTER, CENTER)) return d;
    }
    return NONE;
}

static void decide(const Grid g) {
    if (explosion_pays(g)) { puts("SELFDESTRUCTION"); return; }
    int target = adjacent_attack(g);
    if (target == NONE) target = ambush_attack(g);
    if (target != NONE) { printf("ATTACK %s\n", DIRECTIONS[target].name); return; }
    int step = approach(g);
    if (step != NONE) { printf("MOVE %s\n", DIRECTIONS[step].name); return; }
    const Direction *drift = &DIRECTIONS[DRIFT];
    if (!any_enemy(g) && g[CENTER + drift->dr][CENTER + drift->dc] == 0) { printf("MOVE %s\n", drift->name); return; }
    puts("GUARD");
}

int main(void) {
    int count;
    while (scanf("%d", &count) == 1) {
        for (int i = 0; i < count; ++i) {
            Grid g;
            for (int r = 0; r < SIZE; ++r)
                for (int c = 0; c < SIZE; ++c)
                    if (scanf("%d", &g[r][c]) != 1) return 0;
            decide((const int (*)[SIZE])g);
        }
        fflush(stdout);
    }
    return 0;
}
