/* Bandas 8x8 (CodinGame) — C port of solutions/python/bot.py (v1).
 * Same referee rules (scan order, recursive pushes, border trimming), search and eval.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { MAX_SIDE = 8, MAX_CELLS = MAX_SIDE * MAX_SIDE };
enum { EMPTY = 0, PAWN0 = 1, PAWN1 = 2, HOLE = 3 };
static const char *DIRECTIONS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};
static const int STEP_Y[4] = {-1, 0, 1, 0}, STEP_X[4] = {0, 1, 0, -1};
enum { WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64 };
enum { PAWN_WEIGHT = 100 };     /* material dominates */
enum { SAFETY_WEIGHT = 3 };     /* per step between a pawn and the nearest drop */
enum { SAFETY_CAP = 3 };        /* beyond this distance a pawn is considered safe */
enum { DEFAULT_NODES = 10000 }; /* CodinGame budget: ~1.8 us/node local, ~40 ms on CG */
enum { TIME_GUARD_MS = 40 };    /* 40 % of the limit */

static int H = MAX_SIDE, W = MAX_SIDE;

typedef struct { int cells[MAX_CELLS]; } Grid; /* row major, H x W */

static int is_pawn(int v) { return v == PAWN0 || v == PAWN1; }
static int at(const Grid *g, int y, int x) { return g->cells[y * W + x]; }
static void put(Grid *g, int y, int x, int v) { g->cells[y * W + x] = v; }

/* referee singleMove*: push what is in front, then move or fall. */
static void step_pawn(Grid *g, int pawn, int y, int x, int dy, int dx) {
    int ny = y + dy, nx = x + dx;
    if (ny < 0 || ny >= H || nx < 0 || nx >= W) { put(g, y, x, EMPTY); return; }
    int front = at(g, ny, nx);
    if (is_pawn(front)) {
        step_pawn(g, front, ny, nx, dy, dx);
        step_pawn(g, pawn, y, x, dy, dx);
    } else if (front == HOLE) {
        put(g, y, x, EMPTY);
    } else {
        put(g, ny, nx, pawn);
        put(g, y, x, EMPTY);
    }
}

static void step_if_own(Grid *g, int pawn, int y, int x, int dir) {
    if (at(g, y, x) == pawn) step_pawn(g, pawn, y, x, STEP_Y[dir], STEP_X[dir]);
}

/* Scan order of the referee for each direction. */
static void move_pawns(Grid *g, int player, int dir) {
    int pawn = PAWN0 + player;
    if (dir == 0) { for (int x = 0; x < W; ++x) for (int y = 0; y < H; ++y) step_if_own(g, pawn, y, x, dir); }
    else if (dir == 2) { for (int x = 0; x < W; ++x) for (int y = H - 1; y >= 0; --y) step_if_own(g, pawn, y, x, dir); }
    else if (dir == 1) { for (int y = 0; y < H; ++y) for (int x = W - 1; x >= 0; --x) step_if_own(g, pawn, y, x, dir); }
    else { for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) step_if_own(g, pawn, y, x, dir); }
}

/* A row (column = 0) or a column: 1 when it still holds a pawn. */
static int line_has_pawn(const Grid *g, int index, int column) {
    int n = column ? H : W;
    for (int k = 0; k < n; ++k) if (is_pawn(column ? at(g, k, index) : at(g, index, k))) return 1;
    return 0;
}

static void fill_line(Grid *g, int index, int column) {
    int n = column ? H : W;
    for (int k = 0; k < n; ++k) { if (column) put(g, k, index, HOLE); else put(g, index, k, HOLE); }
}

/* Pawn-free lines become holes from one side until a line with a pawn. */
static void trim(Grid *g, int column, int reverse) {
    int n = column ? W : H;
    for (int i = 0; i < n; ++i) {
        int index = reverse ? n - 1 - i : i;
        if (line_has_pawn(g, index, column)) return;
        fill_line(g, index, column);
    }
}

static void play(Grid *g, int player, int dir) {
    move_pawns(g, player, dir);
    trim(g, 0, 0);
    trim(g, 0, 1);
    trim(g, 1, 0);
    trim(g, 1, 1);
}

static int count_pawns(const Grid *g, int player) {
    int n = 0;
    for (int i = 0; i < H * W; ++i) n += g->cells[i] == PAWN0 + player;
    return n;
}

/* -1 while both sides have pawns (referee checkWinner order). */
static int winner(const Grid *g) {
    if (count_pawns(g, 0) == 0) return 1;
    if (count_pawns(g, 1) == 0) return 0;
    return -1;
}

/* Steps before this pawn would fall, in its most exposed direction. */
static int drop_distance(const Grid *g, int y, int x) {
    int best = SAFETY_CAP;
    for (int d = 0; d < 4; ++d) {
        int steps = 0, ny = y + STEP_Y[d], nx = x + STEP_X[d];
        while (steps < best && ny >= 0 && ny < H && nx >= 0 && nx < W && at(g, ny, nx) != HOLE) {
            ++steps; ny += STEP_Y[d]; nx += STEP_X[d];
        }
        if (steps < best) best = steps;
    }
    return best;
}

static int evaluate(const Grid *g, int player) {
    int score = 0;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            int v = at(g, y, x);
            if (!is_pawn(v)) continue;
            int sign = v == PAWN0 + player ? 1 : -1;
            score += sign * (PAWN_WEIGHT + SAFETY_WEIGHT * drop_distance(g, y, x));
        }
    return score;
}

/* ------------------------------------------------------------- search */
static long long node_limit = DEFAULT_NODES, nodes = 0;
static int last_depth = 0, aborted = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

static int negamax(const Grid *g, int player, int depth, int alpha, int beta, int ply);

static int move_value(const Grid *g, int player, int dir, int depth, int alpha, int beta, int ply) {
    Grid child = *g;
    play(&child, player, dir);
    int w = winner(&child);
    if (w >= 0) return w == player ? WIN - ply : -(WIN - ply);
    return -negamax(&child, 1 - player, depth - 1, -beta, -alpha, ply + 1);
}

static int negamax(const Grid *g, int player, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > TIME_GUARD_MS) { fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0; }
    if (depth == 0) return evaluate(g, player);
    int best = -INF;
    for (int dir = 0; dir < 4; ++dir) {
        int value = move_value(g, player, dir, depth, alpha, beta, ply);
        if (aborted) return 0;
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

static int search(const Grid *g, int player) {
    nodes = 0; aborted = 0; last_depth = 0;
    int order[4] = {0, 1, 2, 3}, best = order[0];
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_value = -INF, best_index = 0;
        for (int i = 0; i < 4; ++i) {
            int value = move_value(g, player, order[i], depth, alpha, INF, 1);
            if (aborted) break;
            if (value > best_value) { best_value = value; best_index = i; }
            if (value > alpha) alpha = value;
        }
        if (aborted) break;
        best = order[best_index];
        last_depth = depth;
        for (int i = best_index; i > 0; --i) order[i] = order[i - 1];
        order[0] = best;
        if (best_value >= WIN - MAX_DEPTH || best_value <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

/* ------------------------------------------------------------- I/O */
static int cell_code(const char *tok) {
    if (strcmp(tok, "-") == 0) return EMPTY;
    if (strcmp(tok, "0") == 0) return PAWN0;
    if (strcmp(tok, "1") == 0) return PAWN1;
    return HOLE;
}

int main(void) {
    const char *env = getenv("CG_NODES");
    node_limit = env ? atoll(env) : DEFAULT_NODES;
    int me;
    if (scanf("%d %d %d", &me, &H, &W) != 3) return 0;
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    while (1) {
        Grid g;
        for (int y = 0; y < H; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            if (y == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            char *tok = strtok(line, " \t\r\n");
            for (int x = 0; x < W; ++x) {
                put(&g, y, x, tok ? cell_code(tok) : HOLE);
                if (tok) tok = strtok(NULL, " \t\r\n");
            }
        }
        int dir = search(&g, me);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%s\n", DIRECTIONS[dir]);
        fflush(stdout);
    }
}
