/* Vindinium — C port of cpp/bot.cpp (same rules, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum { KILL_LIFE = 20 };   /* one hit kills */
enum { KILL_REACH = 2 };   /* moves to stand next to the prey */
enum { LOW_LIFE = 50, TOP_UP = 80, MINE_FIGHT = 20, SAFETY = 15, BEER_PRICE = 2 };
enum { MAX_SIZE = 32, MAX_ENTITIES = 512, MAX_HEROES = 16, NO_MOVE = -1, UNREACHED = -1, UNDECIDED = -1 };
#define TAVERN 'T'
static const char *WALKABLE = ".0123";
typedef struct { const char *name; int dr, dc; } Step;
static const Step MOVES[4] = {{"NORTH", -1, 0}, {"EAST", 0, 1}, {"SOUTH", 1, 0}, {"WEST", 0, -1}};

typedef struct { int owner, x, y, life, gold; } Entity;
typedef struct { int dist[MAX_SIZE][MAX_SIZE], first[MAX_SIZE][MAX_SIZE]; } Reach;
typedef struct { int r, c; } Cell;

static char grid[MAX_SIZE][MAX_SIZE + 2];
static int size, me, x_is_column = UNDECIDED;

static bool inside(int r, int c) { return r >= 0 && r < size && c >= 0 && c < size; }
static Cell cell(int x, int y) { return x_is_column ? (Cell){y, x} : (Cell){x, y}; }
static bool walkable(char ch) { return ch && strchr(WALKABLE, ch) != NULL; }

static void detect_axes(int x, int y) {
    if (x_is_column != UNDECIDED) return;
    char spawn = (char)('0' + me);
    bool row_first = inside(x, y) && grid[x][y] == spawn && !(inside(y, x) && grid[y][x] == spawn);
    x_is_column = !row_first;
}

static void bfs(Cell start, bool blocked[MAX_SIZE][MAX_SIZE], Reach *reach) {
    static Cell queue[MAX_SIZE * MAX_SIZE + 1];
    int head = 0, tail = 0;
    memset(reach->dist, UNREACHED, sizeof reach->dist);
    if (!inside(start.r, start.c)) return;
    reach->dist[start.r][start.c] = 0;
    reach->first[start.r][start.c] = NO_MOVE;
    queue[tail++] = start;
    while (head < tail) {
        Cell cur = queue[head++];
        for (int m = 0; m < 4; ++m) {
            int nr = cur.r + MOVES[m].dr, nc = cur.c + MOVES[m].dc;
            if (!inside(nr, nc) || reach->dist[nr][nc] != UNREACHED || !walkable(grid[nr][nc]) || blocked[nr][nc]) continue;
            reach->dist[nr][nc] = reach->dist[cur.r][cur.c] + 1;
            reach->first[nr][nc] = reach->first[cur.r][cur.c] == NO_MOVE ? m : reach->first[cur.r][cur.c];
            queue[tail++] = (Cell){nr, nc};
        }
    }
}

static int reached(const Reach *reach, int r, int c) { return inside(r, c) ? reach->dist[r][c] : UNREACHED; }

static int compare_cells(const void *a, const void *b) {
    const Cell *p = a, *q = b;
    return p->r != q->r ? p->r - q->r : p->c - q->c;
}

/* Cost and move to step into the best target cell; returns the cost, -1 when none. */
static int closest(const Reach *reach, Cell *targets, int n, int *move) {
    qsort(targets, n, sizeof *targets, compare_cells);
    int best = -1;
    Cell best_cell = {0, 0};
    for (int i = 0; i < n; ++i)
        for (int m = 0; m < 4; ++m) {
            int r = targets[i].r - MOVES[m].dr, c = targets[i].c - MOVES[m].dc; /* we step from (r, c) with move m */
            int d = reached(reach, r, c);
            if (d == UNREACHED) continue;
            int cost = d + 1;
            if (best < 0 || cost < best || (cost == best && compare_cells(&targets[i], &best_cell) < 0)) {
                best = cost;
                best_cell = targets[i];
                *move = reach->first[r][c] == NO_MOVE ? m : reach->first[r][c];
            }
        }
    return best;
}

static const char *decide(Entity *heroes, bool *present, Entity *mines, int n_mines) {
    static bool blocked[MAX_SIZE][MAX_SIZE];
    static Reach reach;
    static Cell targets[MAX_ENTITIES + MAX_SIZE * MAX_SIZE];
    const Entity *hero = &heroes[me];
    Cell pos = cell(hero->x, hero->y);
    memset(blocked, 0, sizeof blocked);
    for (int i = 0; i < MAX_HEROES; ++i) {
        Cell c = cell(heroes[i].x, heroes[i].y);
        if (present[i] && i != me && inside(c.r, c.c)) blocked[c.r][c.c] = true;
    }
    bfs(pos, blocked, &reach);
    int prey_dist = -1;
    const char *prey = NULL;
    for (int i = 0; i < MAX_HEROES; ++i) { /* weak enemy owning mines within KILL_REACH */
        bool owns = false;
        for (int k = 0; k < n_mines; ++k) owns |= mines[k].owner == i;
        if (!present[i] || i == me || heroes[i].life > KILL_LIFE || !owns) continue;
        Cell c = cell(heroes[i].x, heroes[i].y);
        for (int m = 0; m < 4; ++m) {
            int r = c.r + MOVES[m].dr, cc = c.c + MOVES[m].dc, d = reached(&reach, r, cc);
            if (d == UNREACHED || d > KILL_REACH || (prey_dist >= 0 && d >= prey_dist)) continue;
            prey_dist = d;
            prey = reach.first[r][cc] == NO_MOVE ? "WAIT" : MOVES[reach.first[r][cc]].name;
        }
    }
    if (prey) return prey;
    for (int i = 0; i < MAX_HEROES; ++i) { /* cells next to an enemy at least as healthy as us */
        if (!present[i] || i == me || heroes[i].life < hero->life) continue;
        Cell c = cell(heroes[i].x, heroes[i].y);
        for (int m = 0; m < 4; ++m) {
            int r = c.r + MOVES[m].dr, cc = c.c + MOVES[m].dc;
            if (inside(r, cc) && !(r == pos.r && cc == pos.c)) blocked[r][cc] = true;
        }
    }
    bfs(pos, blocked, &reach);
    int n = 0, tavern_move = NO_MOVE, mine_move = NO_MOVE;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (grid[r][c] == TAVERN) targets[n++] = (Cell){r, c};
    int to_tavern = closest(&reach, targets, n, &tavern_move);
    bool can_pay = hero->gold >= BEER_PRICE;
    if (to_tavern >= 0 && can_pay && (hero->life <= LOW_LIFE || (to_tavern == 1 && hero->life <= TOP_UP)))
        return MOVES[tavern_move].name;
    n = 0;
    for (int k = 0; k < n_mines; ++k)
        if (mines[k].owner != me) targets[n++] = cell(mines[k].x, mines[k].y);
    int to_mine = closest(&reach, targets, n, &mine_move);
    if (to_mine >= 0 && hero->life - to_mine - MINE_FIGHT > SAFETY) return MOVES[mine_move].name;
    if (to_tavern >= 0 && can_pay) return MOVES[tavern_move].name;
    return to_mine >= 0 ? MOVES[mine_move].name : "WAIT";
}

int main(void) {
    static Entity heroes[MAX_HEROES], mines[MAX_ENTITIES];
    char row[MAX_SIZE * 4], kind[16];
    if (scanf("%d", &size) != 1) return 0;
    if (size > MAX_SIZE) size = MAX_SIZE;
    for (int r = 0; r < size; ++r) {
        if (scanf("%127s", row) != 1) return 0;
        size_t len = strlen(row);
        for (int c = 0; c < size; ++c) grid[r][c] = (size_t)c < len ? row[c] : '#';
        grid[r][size] = '\0';
    }
    if (scanf("%d", &me) != 1) return 0;
    int count;
    while (scanf("%d", &count) == 1) {
        bool present[MAX_HEROES] = {false};
        int n_mines = 0;
        for (int i = 0; i < count; ++i) {
            Entity e;
            if (scanf("%15s %d %d %d %d %d", kind, &e.owner, &e.x, &e.y, &e.life, &e.gold) != 6) return 0;
            if (!strcmp(kind, "HERO")) {
                if (e.owner >= 0 && e.owner < MAX_HEROES) { heroes[e.owner] = e; present[e.owner] = true; }
            } else if (n_mines < MAX_ENTITIES) {
                mines[n_mines++] = e;
            }
        }
        if (me < 0 || me >= MAX_HEROES || !present[me]) { printf("WAIT\n"); fflush(stdout); continue; }
        detect_axes(heroes[me].x, heroes[me].y);
        printf("%s\n", decide(heroes, present, mines, n_mines));
        fflush(stdout);
    }
    return 0;
}
