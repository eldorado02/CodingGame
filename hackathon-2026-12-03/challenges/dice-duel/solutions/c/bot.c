/* Dice Duel — C port of cpp/bot.cpp (same moves, same search, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L  /* clock_gettime under -std=c11 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { SIZE = 8, FACES = 6, MAX_DICE = 16, MAX_PATH = 8, MAX_ORIENT = 24 };
static const bool SUM_RULE = true;  /* Wood 1 rule (Wood 2: false): capture only when tops sum to 7 */
enum { CAPTURE_SUM = 7, DIE_VALUE = 100, NONE = -1 };
enum { TURN_NODES = 4000, TURN_MS = 30 };  /* node cost varies x4 with the dice count */
typedef struct { char name; int dx, dy; int roll[FACES]; } Step;
static const Step STEPS[4] = {{'U', 0, 1, {1, 2, 3, 0, 4, 5}},
                              {'D', 0, -1, {3, 0, 1, 2, 4, 5}},
                              {'R', 1, 0, {4, 1, 5, 3, 2, 0}},
                              {'L', -1, 0, {5, 1, 4, 3, 0, 2}}};

typedef struct { int owner, x, y, faces[FACES]; } Die;   /* top, front, bottom, back, left, right */
typedef struct { Die d[MAX_DICE]; int n; } Dice;
typedef struct { int die; char path[MAX_PATH]; int x, y, faces[FACES], captured; } Move;
typedef struct { Move *items; int n, cap; } MoveList;

static void push_move(MoveList *list, Move m) {
    if (list->n == list->cap) {
        list->cap = list->cap ? 2 * list->cap : 64;
        list->items = realloc(list->items, list->cap * sizeof(Move));
    }
    list->items[list->n++] = m;
}

/* Self-avoiding rolls of every die of one side (DFS order U, D, R, L). */
typedef struct {
    const Dice *dice;
    int side, at[SIZE][SIZE];
    bool visited[SIZE][SIZE];
    int seen[MAX_DICE][SIZE][SIZE][MAX_ORIENT][FACES];
    int seen_n[MAX_DICE][SIZE][SIZE];
    MoveList moves;
} Generator;

static void record(Generator *g, int i, int x, int y, const int *faces, const char *path) {
    int j = g->at[x][y];
    if (j >= 0 && SUM_RULE && faces[0] + g->dice->d[j].faces[0] != CAPTURE_SUM) return;
    int *n = &g->seen_n[i][x][y];
    for (int k = 0; k < *n; ++k)
        if (!memcmp(g->seen[i][x][y][k], faces, sizeof(int) * FACES)) return;
    if (*n < MAX_ORIENT) memcpy(g->seen[i][x][y][(*n)++], faces, sizeof(int) * FACES);
    Move m = {i, {0}, x, y, {0}, j};
    strcpy(m.path, path);
    memcpy(m.faces, faces, sizeof m.faces);
    push_move(&g->moves, m);
}

static void walk(Generator *g, int i, int x, int y, const int *faces, char *path, int len) {
    int top = g->dice->d[i].faces[0];
    if (len == top) { record(g, i, x, y, faces, path); return; }
    if (len >= MAX_PATH - 1) return;
    for (int s = 0; s < 4; ++s) {
        int nx = x + STEPS[s].dx, ny = y + STEPS[s].dy;
        if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE || g->visited[nx][ny]) continue;
        int j = g->at[nx][ny];
        if (j >= 0 && (g->dice->d[j].owner == g->side || len + 1 < top)) continue; /* only the last step captures */
        int next[FACES];
        for (int k = 0; k < FACES; ++k) next[k] = faces[STEPS[s].roll[k]];
        g->visited[nx][ny] = true;
        path[len] = STEPS[s].name;
        path[len + 1] = '\0';
        walk(g, i, nx, ny, next, path, len + 1);
        path[len] = '\0';
        g->visited[nx][ny] = false;
    }
}

/* Moves of one side, captures first (generation order kept); caller frees items. */
static MoveList gen_moves(const Dice *dice, int side) {
    Generator *g = calloc(1, sizeof(Generator));
    g->dice = dice;
    g->side = side;
    memset(g->at, -1, sizeof g->at);
    for (int i = 0; i < dice->n; ++i) g->at[dice->d[i].x][dice->d[i].y] = i;
    for (int i = 0; i < dice->n; ++i) {
        if (dice->d[i].owner != side) continue;
        char path[MAX_PATH] = {0};
        g->visited[dice->d[i].x][dice->d[i].y] = true;
        walk(g, i, dice->d[i].x, dice->d[i].y, dice->d[i].faces, path, 0);
        g->visited[dice->d[i].x][dice->d[i].y] = false;
    }
    MoveList ordered = {0};
    for (int k = 0; k < g->moves.n; ++k)
        if (g->moves.items[k].captured >= 0) push_move(&ordered, g->moves.items[k]);
    for (int k = 0; k < g->moves.n; ++k)
        if (g->moves.items[k].captured < 0) push_move(&ordered, g->moves.items[k]);
    free(g->moves.items);
    free(g);
    return ordered;
}

static Dice apply_move(const Dice *dice, const Move *m) {
    Dice out = *dice;
    out.d[m->die].x = m->x;
    out.d[m->die].y = m->y;
    memcpy(out.d[m->die].faces, m->faces, sizeof m->faces);
    if (m->captured >= 0) {
        for (int k = m->captured; k + 1 < out.n; ++k) out.d[k] = out.d[k + 1];
        --out.n;
    }
    return out;
}

static int evaluate(const Dice *dice, int side) {
    int mine = 0;
    for (int i = 0; i < dice->n; ++i) mine += dice->d[i].owner == side;
    return DIE_VALUE * (mine - (dice->n - mine));
}

/* ---------------------------------------------------------------- ENGINE */
enum { WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64 };
static long long node_limit = TURN_NODES, nodes;
static bool aborted;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

static int negamax(const Dice *dice, int side, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = true; return 0; }
    /* checked at every node: one move generation can cost 0.2 ms */
    if (elapsed_ms() > TURN_MS) { fprintf(stderr, "TIMEGUARD\n"); aborted = true; return 0; }
    if (depth == 0) return evaluate(dice, side);
    MoveList moves = gen_moves(dice, side);
    if (!moves.n) { free(moves.items); return -(WIN - ply); }
    int best = -INF;
    for (int k = 0; k < moves.n; ++k) {
        Dice child = apply_move(dice, &moves.items[k]);
        int v = -negamax(&child, 1 - side, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) { best = 0; break; }
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    free(moves.items);
    return best;
}

static Move search(const Dice *dice, MoveList *moves) {
    Move best = moves->items[0];
    nodes = 0;
    aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur = 0;
        for (int k = 0; k < moves->n; ++k) {
            Dice child = apply_move(dice, &moves->items[k]);
            int v = -negamax(&child, 1, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur = k; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = moves->items[cur];
        for (int k = cur; k > 0; --k) moves->items[k] = moves->items[k - 1]; /* PV first */
        moves->items[0] = best;
        if (abs(best_v) >= WIN - MAX_DEPTH) break;
    }
    return best;
}

int main(void) {
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    int count;
    while (scanf("%d", &count) == 1) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        Dice dice = {.n = count < MAX_DICE ? count : MAX_DICE};
        for (int i = 0; i < count; ++i) {
            Die d;
            char cell[8];
            if (scanf("%d %7s", &d.owner, cell) != 2) return 0;
            for (int k = 0; k < FACES; ++k)
                if (scanf("%d", &d.faces[k]) != 1) return 0;
            d.x = cell[0] - 'A';
            d.y = atoi(cell + 1) - 1;
            if (i < MAX_DICE) dice.d[i] = d;
        }
        MoveList moves = gen_moves(&dice, 0);
        if (!moves.n) {
            printf("A1 U\n");
        } else {
            node_limit = cli_nodes > 0 ? cli_nodes : TURN_NODES;
            Move m = search(&dice, &moves);
            printf("%c%d %s\n", 'A' + dice.d[m.die].x, dice.d[m.die].y + 1, m.path);
        }
        free(moves.items);
        fflush(stdout);
    }
    return 0;
}
