/* Hex 11x11 (CodinGame) — C port of solutions/python/bot.py (v1).
 * Cell = row*11+col. Neighbours (-1,0)(-1,1)(0,-1)(0,1)(1,-1)(1,0).
 * MCTS + RAVE, random-fill playouts, iteration budget (env CG_NODES).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 11
#define NC (N * N)
#define RED 1
#define BLUE 2
#define RAVE_B 0.04
#define MAX_NODES 2000000
#define EXPAND_N 3
#define OPENING (1 * N + 2)

static int NB[NC][6], NBN[NC];
static int T_cell[MAX_NODES], T_first[MAX_NODES], T_nch[MAX_NODES];
static int T_n[MAX_NODES], T_w[MAX_NODES], T_an[MAX_NODES], T_aw[MAX_NODES];
static int T_size = 0;
static uint64_t rng_s;
static struct timespec t0;
static int time_ms;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

static uint64_t rng_next(void) {
    rng_s ^= rng_s << 13; rng_s ^= rng_s >> 7; rng_s ^= rng_s << 17;
    return rng_s;
}

static void init_nb(void) {
    const int dr[6] = {-1, -1, 0, 0, 1, 1}, dc[6] = {0, 1, -1, 1, -1, 0};
    for (int c = 0; c < NC; ++c) {
        int r = c / N, q = c % N, k = 0;
        for (int i = 0; i < 6; ++i) {
            int a = r + dr[i], b = q + dc[i];
            if (a >= 0 && a < N && b >= 0 && b < N) NB[c][k++] = a * N + b;
        }
        NBN[c] = k;
    }
}

static int connected(const int8_t *board, int p) {
    int stack[NC], sp = 0;
    char seen[NC];
    memset(seen, 0, sizeof seen);
    if (p == RED) { for (int c = 0; c < N; ++c) if (board[c] == RED) { stack[sp++] = c; seen[c] = 1; } }
    else { for (int r = 0; r < N; ++r) if (board[r * N] == BLUE) { stack[sp++] = r * N; seen[r * N] = 1; } }
    while (sp) {
        int c = stack[--sp];
        if ((p == RED && c / N == N - 1) || (p == BLUE && c % N == N - 1)) return 1;
        for (int i = 0; i < NBN[c]; ++i) {
            int d = NB[c][i];
            if (!seen[d] && board[d] == p) { seen[d] = 1; stack[sp++] = d; }
        }
    }
    return 0;
}

static int new_node(int c) {
    int i = T_size++;
    T_cell[i] = c; T_first[i] = -1; T_nch[i] = 0; T_n[i] = 0; T_w[i] = 0; T_an[i] = 0; T_aw[i] = 0;
    return i;
}

static void expand(int node, const int8_t *board) {
    if (T_size + NC > MAX_NODES) return;
    T_first[node] = T_size;
    int k = 0;
    for (int c = 0; c < NC; ++c)
        if (board[c] == 0) { new_node(c); ++k; }
    T_nch[node] = k;
}

static int select_child(int node) {
    int best = -1;
    double bestv = -1.0;
    int f = T_first[node], e = f + T_nch[node];
    for (int i = f; i < e; ++i) {
        int ni = T_n[i], ai = T_an[i];
        double v;
        if (ai == 0) v = ni > 0 ? (double)T_w[i] / ni : 0.5;
        else if (ni == 0) v = (double)T_aw[i] / ai;
        else {
            double beta = (double)ai / ((double)ai + (double)ni + RAVE_B * (double)ai * (double)ni);
            v = (1.0 - beta) * ((double)T_w[i] / ni) + beta * ((double)T_aw[i] / ai);
        }
        if (v > bestv) { bestv = v; best = i; }
    }
    return best;
}

static int mcts(const int8_t *root, int me, int iters, int *done) {
    T_size = 0;
    rng_s = 0x9E3779B97F4A7C15ULL;
    new_node(-1);
    expand(0, root);
    int8_t board[NC];
    int path[NC + 2], empt[NC];
    *done = 0;
    for (int it = 0; it < iters; ++it) {
        if ((it & 255) == 0 && it > 0 && elapsed_ms() > time_ms) {
            fprintf(stderr, "TIMEGUARD\n");
            break;
        }
        memcpy(board, root, NC);
        int node = 0, p = me, plen = 0;
        path[plen++] = 0;
        while (T_first[node] >= 0 && T_nch[node] > 0) {
            node = select_child(node);
            board[T_cell[node]] = (int8_t)p;
            p = 3 - p;
            path[plen++] = node;
        }
        if (T_first[node] < 0 && T_n[node] >= EXPAND_N) {
            expand(node, board);
            if (T_first[node] >= 0 && T_nch[node] > 0) {
                node = T_first[node];
                board[T_cell[node]] = (int8_t)p;
                p = 3 - p;
                path[plen++] = node;
            }
        }
        int k = 0;
        for (int c = 0; c < NC; ++c) if (board[c] == 0) empt[k++] = c;
        for (int i = k - 1; i > 0; --i) {
            int j = (int)(((rng_next() >> 32) * (uint64_t)(i + 1)) >> 32);
            int t = empt[i]; empt[i] = empt[j]; empt[j] = t;
        }
        int q = p;
        for (int i = 0; i < k; ++i) { board[empt[i]] = (int8_t)q; q = 3 - q; }
        int winner = connected(board, RED) ? RED : BLUE;
        int mover = me, pl = me;
        for (int d = 0; d < plen; ++d) {
            int nd = path[d];
            T_n[nd]++;
            if (d > 0) { if (winner == mover) T_w[nd]++; mover = 3 - mover; }
            int f = T_first[nd];
            if (f >= 0) {
                int win = winner == pl ? 1 : 0, e = f + T_nch[nd];
                for (int i = f; i < e; ++i)
                    if (board[T_cell[i]] == pl) { T_an[i]++; T_aw[i] += win; }
            }
            pl = 3 - pl;
        }
        ++*done;
    }
    int f = T_first[0], best = -1, bn = -1;
    for (int i = f; i < f + T_nch[0]; ++i) if (T_n[i] > bn) { bn = T_n[i]; best = i; }
    return T_cell[best];
}

static void name_of(int c, char *out) { sprintf(out, "%c%d", 'a' + c % N, c / N + 1); }

static void choose(int8_t *board, int me, const char *last, int iters, char *out) {
    int reds = 0, blues = 0;
    for (int c = 0; c < NC; ++c) { reds += board[c] == RED; blues += board[c] == BLUE; }
    if (reds == 0 && blues == 0) { name_of(OPENING, out); fprintf(stderr, "opening\n"); return; }
    if (me == BLUE && reds == 1 && blues == 0 && strcmp(last, "empty") != 0) {
        int c = 0;
        while (board[c] != RED) ++c;
        int r = c / N, q = c % N;
        if (r >= 1 && r <= N - 2 && q >= 1 && q <= N - 2) { strcpy(out, "swap-pieces"); fprintf(stderr, "swap\n"); return; }
    }
    int opp = 3 - me;
    for (int c = 0; c < NC; ++c)
        if (board[c] == 0) {
            board[c] = (int8_t)me; int ok = connected(board, me); board[c] = 0;
            if (ok) { name_of(c, out); fprintf(stderr, "win\n"); return; }
        }
    for (int c = 0; c < NC; ++c)
        if (board[c] == 0) {
            board[c] = (int8_t)opp; int ok = connected(board, opp); board[c] = 0;
            if (ok) { name_of(c, out); fprintf(stderr, "block\n"); return; }
        }
    int done;
    int c = mcts(board, me, iters, &done);
    name_of(c, out);
    fprintf(stderr, "mcts it=%d nodes=%d ms=%ld\n", done, T_size, elapsed_ms());
}

int main(void) {
    init_nb();
    char line[512];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int me = strstr(line, "red") ? RED : BLUE;
    const char *e = getenv("CG_NODES");
    long cli = e ? atol(e) : -1;
    int turn = 0;
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        char last[64] = "empty";
        sscanf(line, "%63s", last);
        int8_t board[NC];
        for (int r = 0; r < N; ++r) {
            char raw[512], row[512];
            int k = 0;
            if (!fgets(raw, sizeof raw, stdin)) raw[0] = 0;
            for (int i = 0; raw[i]; ++i)
                if (raw[i] != ' ' && raw[i] != '\r' && raw[i] != '\n' && raw[i] != '\t') row[k++] = raw[i];
            for (int q = 0; q < N; ++q) {
                char ch = q < k ? row[q] : '.';
                board[r * N + q] = (int8_t)(ch == 'r' ? RED : ch == 'b' ? BLUE : 0);
            }
        }
        int iters = cli > 0 ? (int)cli : (turn == 0 ? 30000 : 2600);
        time_ms = turn == 0 ? 700 : 70;
        char out[32];
        choose(board, me, last, iters, out);
        printf("%s\n", out);
        fflush(stdout);
        ++turn;
    }
    return 0;
}
