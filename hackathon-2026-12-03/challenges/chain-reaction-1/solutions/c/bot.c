/* Chain Reaction 6x6 — C port of solutions/python/bot.py (v1).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 36
#define MAX_ROUNDS 200
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64

static int NB[N][4], NBN[N], CRIT[N];
typedef struct { int own[N]; int orb[N]; int side; } State;

static void init_tables(void) {
    static const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
    for (int i = 0; i < N; ++i) {
        int x = i % 6, y = i / 6, n = 0;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < 6 && ny >= 0 && ny < 6) NB[i][n++] = ny * 6 + nx;
        }
        NBN[i] = n;
        CRIT[i] = n;
    }
}

static int gen_moves(const State *s, int *out) {
    int n = 0;
    for (int i = 0; i < N; ++i) if (s->own[i] == 0 || s->own[i] == s->side) out[n++] = i;
    return n;
}

static State apply_move(const State *s, int i) {
    State t = *s;
    int p = s->side, o = 3 - p, crit[N];
    t.own[i] = p;
    t.orb[i] += 1;
    for (int rounds = 0; rounds < MAX_ROUNDS; ++rounds) {
        int nc = 0;
        for (int j = 0; j < N; ++j) if (t.orb[j] >= CRIT[j]) crit[nc++] = j;
        if (!nc) break;
        for (int c = 0; c < nc; ++c) {
            int j = crit[c];
            t.orb[j] -= CRIT[j];
            if (t.orb[j] == 0) t.own[j] = 0;
            for (int k = 0; k < NBN[j]; ++k) { t.orb[NB[j][k]] += 1; t.own[NB[j][k]] = p; }
        }
        int any = 0;
        for (int j = 0; j < N; ++j) if (t.own[j] == o) { any = 1; break; }
        if (!any) break;
    }
    t.side = o;
    return t;
}

static int winner(const State *s) {
    int total = 0, r = 0, b = 0;
    for (int i = 0; i < N; ++i) {
        total += s->orb[i];
        if (s->own[i] == 1) ++r; else if (s->own[i] == 2) ++b;
    }
    if (total < 2) return 0;
    if (r == 0 && b > 0) return 2;
    if (b == 0 && r > 0) return 1;
    return 0;
}

static int side_score(const State *s, int p) {
    int o = 3 - p, sc = 0;
    for (int i = 0; i < N; ++i) {
        if (s->own[i] != p) continue;
        sc += s->orb[i];
        int vulnerable = 0;
        for (int k = 0; k < NBN[i]; ++k) {
            int q = NB[i][k];
            if (s->own[q] == o && s->orb[q] == CRIT[q] - 1) { vulnerable = 1; sc -= 5 - CRIT[i]; }
        }
        if (!vulnerable) {
            if (CRIT[i] == 2) sc += 3;
            else if (CRIT[i] == 3) sc += 2;
            if (s->orb[i] == CRIT[i] - 1) sc += 2;
        }
    }
    return sc;
}

static int evaluate(const State *s) { return side_score(s, s->side) - side_score(s, 3 - s->side); }

static int order_key(const State *s, int i) {
    int k = 0;
    if (s->own[i] == s->side && s->orb[i] == CRIT[i] - 1) k += 4;
    return k + 4 - CRIT[i];
}

static long long node_limit = 50000, nodes = 0;
static int time_ms = 70, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static void order_moves(const State *s, int *mv, int n) {
    int key[N];
    for (int i = 0; i < n; ++i) key[i] = order_key(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        int x = mv[i], k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int w = winner(s);
    if (w) return (w == s->side) ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    int mv[N];
    int n = gen_moves(s, mv);
    if (n == 0) return -(WIN - ply);
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

static int search(const State *root, int *mv, int n) {
    last_depth = 0;
    order_moves(root, mv, n);
    int best = mv[0];
    nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0, cur = mv[0];
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur = mv[i]; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        last_depth = depth;
        int pv = mv[cur_idx];
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

int main(void) {
    init_tables();
    long long cli_nodes = -1;
    const char *e = getenv("CG_NODES");
    if (e) cli_nodes = atoll(e);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int me = 1;
    for (char *p = line; *p; ++p) if (*p != ' ') { me = (*p == 'b' || *p == 'B') ? 2 : 1; break; }
    int first = 1;
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        State s;
        int ok = 1;
        for (int y = 0; y < 6; ++y) {
            if (!fgets(line, sizeof line, stdin)) { ok = 0; break; }
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < 6; ++x) {
                int i = y * 6 + x;
                s.own[i] = 0; s.orb[i] = 0;
                if (len >= (size_t)(2 * x + 2)) {
                    char a = line[2 * x], b = line[2 * x + 1];
                    if ((a == 'r' || a == 'b') && b >= '1' && b <= '3') {
                        s.own[i] = a == 'r' ? 1 : 2;
                        s.orb[i] = b - '0';
                    }
                }
            }
        }
        if (!ok) break;
        s.side = me;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? 250000 : 50000);
        time_ms = first ? 700 : 70;
        first = 0;
        int all[N];
        int n = gen_moves(&s, all);
        if (!n) { printf("random\n"); fflush(stdout); continue; }
        int m = search(&s, all, n);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%c%c\n", 'a' + m % 6, '0' + 6 - m / 6);
        fflush(stdout);
    }
    return 0;
}
