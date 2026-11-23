/* Dots and Boxes — C port of solutions/python/bot.py (v1 alpha-beta).
 * Edges: horizontal h(x, line) = line * n + x, vertical H + y * (n + 1) + line.
 * Closing a box scores it and the same player moves again (no sign flip).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N 10
#define MAX_EDGES (2 * MAX_N * (MAX_N + 1))
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)
enum { FIRST_TURN_NODES = 200000, TURN_NODES = 20000, FIRST_TURN_MS = 700, TURN_MS = 70 };
enum { CAPTURABLE_WEIGHT = 1 };  /* a box with one free side is ours to take */
static const char SIDE_LETTERS[] = "LTRB";

static int n = 2, edge_count = 0;
static int box_edges[MAX_N * MAX_N][4], edge_boxes[MAX_EDGES][2], edge_box_count[MAX_EDGES];

typedef struct { char drawn[MAX_EDGES]; int diff; } State;  /* diff = side to move - other */

static void init_board(int size) {
    n = size;
    int h_count = n * (n + 1);
    edge_count = 2 * n * (n + 1);
    memset(edge_box_count, 0, sizeof edge_box_count);
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x) {
            int b = y * n + x, left = h_count + y * (n + 1) + x;
            box_edges[b][0] = left;
            box_edges[b][1] = (y + 1) * n + x;
            box_edges[b][2] = left + 1;
            box_edges[b][3] = y * n + x;
        }
    for (int b = 0; b < n * n; ++b)
        for (int k = 0; k < 4; ++k) {
            int e = box_edges[b][k];
            edge_boxes[e][edge_box_count[e]++] = b;
        }
}

static int free_sides(const char *drawn, int b) {
    int f = 0;
    for (int k = 0; k < 4; ++k) f += !drawn[box_edges[b][k]];
    return f;
}

static int gen_moves(const State *s, int *out) {
    int k = 0;
    for (int e = 0; e < edge_count; ++e) if (!s->drawn[e]) out[k++] = e;
    return k;
}

/* returns 1 when the same player moves again (a box was closed) */
static int apply_move(const State *s, int e, State *child) {
    *child = *s;
    child->drawn[e] = 1;
    int closed = 0;
    for (int k = 0; k < edge_box_count[e]; ++k) closed += free_sides(child->drawn, edge_boxes[e][k]) == 0;
    if (closed) { child->diff = s->diff + closed; return 1; }
    child->diff = -s->diff;
    return 0;
}

static int evaluate(const State *s) {
    int capturable = 0;
    for (int b = 0; b < n * n; ++b) capturable += free_sides(s->drawn, b) == 1;
    return s->diff + CAPTURABLE_WEIGHT * capturable;
}

/* 2 = closes a box, 1 = safe (gives no third side), 0 = gift */
static int order_key(const State *s, int e) {
    int closes = 0, gift = 0;
    for (int k = 0; k < edge_box_count[e]; ++k) {
        int f = free_sides(s->drawn, edge_boxes[e][k]);
        if (f == 1) closes = 1;
        else if (f == 2) gift = 1;
    }
    return closes ? 2 : (gift ? 0 : 1);
}

static void order_moves(const State *s, int *mv, int k) {
    int key[MAX_EDGES];
    for (int i = 0; i < k; ++i) key[i] = order_key(s, mv[i]);
    for (int i = 1; i < k; ++i) {
        int x = mv[i], kk = key[i], j = i - 1;
        while (j >= 0 && key[j] < kk) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = kk;
    }
}

static long long node_limit = TURN_NODES, nodes = 0;
static int time_ms = TURN_MS, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int value(const State *s, int depth, int alpha, int beta) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int mv[MAX_EDGES];
    int k = gen_moves(s, mv);
    if (!k) return s->diff > 0 ? WIN + s->diff : (s->diff < 0 ? -WIN + s->diff : 0);
    if (depth == 0) return evaluate(s);
    order_moves(s, mv, k);
    int best = -INF;
    State child;
    for (int i = 0; i < k; ++i) {
        int again = apply_move(s, mv[i], &child);
        int v = again ? value(&child, depth - 1, alpha, beta) : -value(&child, depth - 1, -beta, -alpha);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

static int search(const State *root) {
    int mv[MAX_EDGES];
    int k = gen_moves(root, mv);
    order_moves(root, mv, k);
    int best = mv[0];
    last_depth = 0; nodes = 0; aborted = 0;
    State child;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        for (int i = 0; i < k; ++i) {
            int again = apply_move(root, mv[i], &child);
            int v = again ? value(&child, depth - 1, alpha, INF) : -value(&child, depth - 1, -INF, -alpha);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[cur_idx];
        last_depth = depth;
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];  /* PV first */
        mv[0] = best;
        if (depth >= k + 1) break;
    }
    return best;
}

int main(void) {
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int size = atoi(line);
    if (size < 1 || size > MAX_N) size = 2;
    init_board(size);
    if (!fgets(line, sizeof line, stdin)) return 0;  /* our id: state given in our point of view */
    int first = 1;
    static char owner_name[MAX_EDGES][8];
    static char owner_side[MAX_EDGES];
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        int mine = 0, theirs = 0;
        sscanf(line, "%d %d", &mine, &theirs);
        if (!fgets(line, sizeof line, stdin)) return 0;
        int k = atoi(line), any = 0;
        State s;
        memset(s.drawn, 1, sizeof s.drawn);
        memset(owner_side, 0, sizeof owner_side);
        for (int i = 0; i < k; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char name[16] = {0}, sides[16] = {0};
            if (sscanf(line, "%15s %15s", name, sides) != 2 || strlen(name) < 2) continue;
            int x = name[0] - 'A', y = atoi(name + 1) - 1;
            if (x < 0 || x >= n || y < 0 || y >= n) continue;
            for (char *p = sides; *p; ++p) {
                const char *at = strchr(SIDE_LETTERS, *p);
                if (!at) continue;
                int e = box_edges[y * n + x][at - SIDE_LETTERS];
                s.drawn[e] = 0;
                if (!owner_side[e]) { snprintf(owner_name[e], sizeof owner_name[e], "%s", name); owner_side[e] = *p; any = 1; }
            }
        }
        s.diff = mine - theirs;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = first ? FIRST_TURN_MS : TURN_MS;
        first = 0;
        if (!any) { printf("A1 L\n"); fflush(stdout); continue; }
        int e = search(&s);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%s %c\n", owner_name[e], owner_side[e]);
        fflush(stdout);
    }
    return 0;
}
