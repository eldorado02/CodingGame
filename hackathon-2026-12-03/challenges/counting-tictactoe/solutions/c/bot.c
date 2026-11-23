/* Counting TicTacToe 10x10 — C port of solutions/python/bot.py (v2).
 * Score = sliding windows of 3 same-colour cells; incremental pattern score;
 * selective alpha-beta (inner nodes keep the BEAM best moves by immediate gain).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10
#define CELLS (N * N)
#define MAX_WINDOWS 300
#define MAX_PER_CELL 12
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)

enum { TURN_NODES = 2500, TURN_MS = 70, BEAM = 10, MINE = 1, THEIRS = 2 };
static const int WEIGHTS[4] = {0, 1, 10, 100};  /* window value by own stones when unopposed */

typedef struct { int a, b, c; } Window;
typedef struct { int cell[CELLS]; int me; int score; } State;  /* score for player 1 */

static Window windows[MAX_WINDOWS];
static int window_count = 0, windows_of[CELLS][MAX_PER_CELL], windows_of_count[CELLS];

static void build_windows(void) {
    static const int DR[4] = {1, 0, 1, 1}, DC[4] = {0, 1, 1, -1};
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            for (int d = 0; d < 4; ++d) {
                int r2 = r + 2 * DR[d], c2 = c + 2 * DC[d];
                if (r2 < 0 || r2 >= N || c2 < 0 || c2 >= N) continue;
                Window w = {r * N + c, (r + DR[d]) * N + c + DC[d], r2 * N + c2};
                windows[window_count] = w;
                windows_of[w.a][windows_of_count[w.a]++] = window_count;
                windows_of[w.b][windows_of_count[w.b]++] = window_count;
                windows_of[w.c][windows_of_count[w.c]++] = window_count;
                ++window_count;
            }
}

static int window_value(const int *c, const Window *w) {
    int ones = (c[w->a] == 1) + (c[w->b] == 1) + (c[w->c] == 1);
    int twos = (c[w->a] == 2) + (c[w->b] == 2) + (c[w->c] == 2);
    if (!twos) return WEIGHTS[ones];
    if (!ones) return -WEIGHTS[twos];
    return 0;
}

static int full_score(const int *c) {
    int sc = 0;
    for (int k = 0; k < window_count; ++k) sc += window_value(c, &windows[k]);
    return sc;
}

static int gen_moves(const State *s, int *out) {
    int n = 0;
    for (int i = 0; i < CELLS; ++i) if (!s->cell[i]) out[n++] = i;
    return n;
}

/* only the windows through cell i change: update the score by delta */
static State apply_move(const State *s, int i) {
    State t = *s;
    int before = 0, after = 0;
    for (int k = 0; k < windows_of_count[i]; ++k) before += window_value(t.cell, &windows[windows_of[i][k]]);
    t.cell[i] = s->me;
    for (int k = 0; k < windows_of_count[i]; ++k) after += window_value(t.cell, &windows[windows_of[i][k]]);
    t.score = s->score + after - before;
    t.me = 3 - s->me;
    return t;
}

static int evaluate(const State *s) { return s->me == 1 ? s->score : -s->score; }

static int final_triples(const State *s) {
    int diff = 0;
    for (int k = 0; k < window_count; ++k) {
        int a = s->cell[windows[k].a];
        if (a && a == s->cell[windows[k].b] && a == s->cell[windows[k].c]) diff += a == s->me ? 1 : -1;
    }
    return diff;
}

/* moves sorted by the mover's immediate gain (stable), at most `limit` */
static int ordered_moves(const State *s, int *out, int limit) {
    int mv[CELLS], gain[CELLS];
    int n = gen_moves(s, mv);
    for (int i = 0; i < n; ++i) { State t = apply_move(s, mv[i]); gain[i] = -evaluate(&t); }
    for (int i = 1; i < n; ++i) {
        int x = mv[i], g = gain[i], j = i - 1;
        while (j >= 0 && gain[j] < g) { mv[j + 1] = mv[j]; gain[j + 1] = gain[j]; --j; }
        mv[j + 1] = x; gain[j + 1] = g;
    }
    if (n > limit) n = limit;
    memcpy(out, mv, sizeof(int) * (size_t)n);
    return n;
}

static long long node_limit = TURN_NODES, nodes = 0;
static int aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > TURN_MS) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int mv[CELLS];
    int n = gen_moves(s, mv);
    if (!n) {
        int diff = final_triples(s);
        return diff > 0 ? WIN - ply : diff < 0 ? -(WIN - ply) : 0;
    }
    if (depth == 0) return evaluate(s);
    n = ordered_moves(s, mv, BEAM);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        State t = apply_move(s, mv[i]);
        int v = -negamax(&t, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

static int search(const State *root) {
    int mv[CELLS];
    int n = ordered_moves(root, mv, CELLS), best = mv[0];
    last_depth = 0; nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[cur_idx];
        last_depth = depth;
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];  /* PV first */
        mv[0] = best;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

int main(void) {
    build_windows();
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    int my_cells[CELLS] = {0}, orow, ocol;
    while (scanf("%d %d", &orow, &ocol) == 2) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        int k;
        if (scanf("%d", &k) != 1) return 0;
        int empty[CELLS] = {0}, any_empty = 0, new_match = 0;
        for (int i = 0; i < k; ++i) {
            int r, c;
            if (scanf("%d %d", &r, &c) != 2) return 0;
            if (r >= 0 && r < N && c >= 0 && c < N) empty[r * N + c] = 1;
        }
        for (int i = 0; i < CELLS; ++i) if (my_cells[i] && empty[i]) { new_match = 1; break; }
        if (new_match) memset(my_cells, 0, sizeof my_cells);  /* the board was cleared */
        State s;
        s.me = MINE;
        for (int i = 0; i < CELLS; ++i) {
            s.cell[i] = empty[i] ? 0 : (my_cells[i] ? MINE : THEIRS);
            any_empty |= empty[i];
        }
        if (!any_empty) { printf("0 0\n"); fflush(stdout); continue; }
        s.score = full_score(s.cell);
        node_limit = cli_nodes > 0 ? cli_nodes : TURN_NODES;
        int m = search(&s);
        my_cells[m] = 1;
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%d %d\n", m / N, m % N);
        fflush(stdout);
    }
    return 0;
}
