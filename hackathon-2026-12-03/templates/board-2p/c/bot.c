/* TEMPLATE board-2p — C port of cpp/bot.cpp (same engine, same tie-breaks).
 * Replace the "GAME:" blocks only. Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------ GAME: state */
#define MAX_MOVES 9
typedef struct { int r, c; } Move;
typedef struct { int cell[9]; int to_move; } State;

static const int LINES[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
static const int WEIGHT[9] = {3,2,3,2,4,2,3,2,3};

static int winner(const State *s) {
    for (int i = 0; i < 8; ++i) {
        int a = s->cell[LINES[i][0]];
        if (a && a == s->cell[LINES[i][1]] && a == s->cell[LINES[i][2]]) return a;
    }
    for (int i = 0; i < 9; ++i) if (!s->cell[i]) return 0;
    return 3;
}

static int gen_moves(const State *s, Move *out) {
    int n = 0;
    for (int i = 0; i < 9; ++i) if (!s->cell[i]) { out[n].r = i / 3; out[n].c = i % 3; ++n; }
    return n;
}

static State apply_move(const State *s, Move m) {
    State t = *s;
    t.cell[m.r * 3 + m.c] = s->to_move;
    t.to_move = 3 - s->to_move;
    return t;
}

static int evaluate(const State *s) {
    int sc = 0;
    for (int i = 0; i < 9; ++i) {
        if (s->cell[i] == s->to_move) sc += WEIGHT[i];
        else if (s->cell[i]) sc -= WEIGHT[i];
    }
    return sc;
}

static int order_key(const State *s, Move m) { (void)s; return (m.r == 1 && m.c == 1) ? 1 : 0; }

/* ------------------------------------------------------------ ENGINE */
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64
#define TIME_MS 80
static long long node_limit = 200000, nodes = 0;
static int aborted = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

/* stable insertion sort, key descending (same result as std::stable_sort) */
static void order_moves(const State *s, Move *mv, int n) {
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = order_key(s, x), j = i - 1;
        while (j >= 0 && order_key(s, mv[j]) < k) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > TIME_MS) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int w = winner(s);
    if (w == 3) return 0;
    if (w) return (w == s->to_move) ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    if (n == 0) return evaluate(s);
    order_moves(s, mv, n);
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

static Move search(const State *root) {
    Move mv[MAX_MOVES];
    int n = gen_moves(root, mv);
    order_moves(root, mv, n);
    Move best = mv[0];
    nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        Move cur = mv[0];
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur = mv[i]; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        Move pv = mv[cur_idx];
        memmove(mv + 1, mv, sizeof(Move) * (size_t)cur_idx);
        mv[0] = pv;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
        if (depth >= 9) break;
    }
    return best;
}

/* ------------------------------------------------------------ GAME: I/O */
int main(void) {
    const char *e = getenv("CG_NODES");
    if (e) node_limit = atoll(e);
    State s;
    memset(&s, 0, sizeof s);
    int orow, ocol;
    while (scanf("%d %d", &orow, &ocol) == 2) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if (orow >= 0 && orow < 3 && ocol >= 0 && ocol < 3) s.cell[orow * 3 + ocol] = 2;
        int k;
        if (scanf("%d", &k) != 1) break;
        for (int i = 0; i < k; ++i) { int r, c; if (scanf("%d %d", &r, &c) != 2) return 0; }
        s.to_move = 1;
        Move m = search(&s);
        s.cell[m.r * 3 + m.c] = 1;
        printf("%d %d\n", m.r, m.c);
        fflush(stdout);
    }
    return 0;
}
