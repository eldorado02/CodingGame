/* Mad Knights — C port of solutions/python/bot.py (v1, paranoid alpha-beta).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint64_t U64;
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64
static const int KX[8] = {1, 2, 2, 1, -1, -2, -2, -1}, KY[8] = {2, 1, -1, -2, -2, -1, 1, 2};
static int TG[64][8], TGN[64];
static int ME = 0;

typedef struct { U64 blocked; int pos[3]; int alive[3]; int turn; } State;

static void init_tables(void) {
    for (int i = 0; i < 64; ++i) {
        int x = i % 8, y = i / 8, n = 0;
        for (int k = 0; k < 8; ++k) {
            int nx = x + KX[k], ny = y + KY[k];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) TG[i][n++] = ny * 8 + nx;
        }
        TGN[i] = n;
    }
}

static int moves_of(U64 b, int sq, int *out) {
    int n = 0;
    for (int k = 0; k < TGN[sq]; ++k) if (!((b >> TG[sq][k]) & 1)) out[n++] = TG[sq][k];
    return n;
}

static int mob(U64 b, int sq) {
    int n = 0;
    for (int k = 0; k < TGN[sq]; ++k) if (!((b >> TG[sq][k]) & 1)) ++n;
    return n;
}

static int next_alive(const int *alive, int turn) {
    for (int k = 1; k < 4; ++k) { int j = (turn + k) % 3; if (alive[j]) return j; }
    return turn;
}

static State apply_move(const State *s, int t) {
    State r = *s;
    r.pos[s->turn] = t;
    r.blocked = s->blocked | (1ULL << t);
    r.turn = next_alive(s->alive, s->turn);
    return r;
}

static State eliminate(const State *s) {
    State r = *s;
    r.alive[s->turn] = 0;
    r.turn = next_alive(r.alive, s->turn);
    return r;
}

static int evaluate(const State *s) {
    int sc = 0, mv[8];
    int n = moves_of(s->blocked, s->pos[ME], mv);
    for (int i = 0; i < n; ++i) sc += 10 + mob(s->blocked | (1ULL << mv[i]), mv[i]);
    for (int j = 0; j < 3; ++j) if (j != ME && s->alive[j]) sc -= 6 * mob(s->blocked, s->pos[j]);
    return sc;
}

static void order_moves(const State *s, int *mv, int n) {
    int key[64];
    for (int i = 0; i < n; ++i) key[i] = mob(s->blocked | (1ULL << mv[i]), mv[i]);
    for (int i = 1; i < n; ++i) {
        int x = mv[i], k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

static long long node_limit = 60000, nodes = 0;
static int time_ms = 70, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int minimax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    if (!s->alive[ME]) return -(WIN - ply);
    if (s->alive[0] + s->alive[1] + s->alive[2] == 1) return WIN - ply;
    int mv[8];
    int n = moves_of(s->blocked, s->pos[s->turn], mv);
    if (n == 0) { State e = eliminate(s); return minimax(&e, depth, alpha, beta, ply); }
    if (depth == 0) return evaluate(s);
    order_moves(s, mv, n);
    if (s->turn == ME) {
        int best = -INF;
        for (int i = 0; i < n; ++i) {
            State t = apply_move(s, mv[i]);
            int v = minimax(&t, depth - 1, alpha, beta, ply + 1);
            if (aborted) return 0;
            if (v > best) best = v;
            if (v > alpha) alpha = v;
            if (alpha >= beta) break;
        }
        return best;
    }
    int best = INF;
    for (int i = 0; i < n; ++i) {
        State t = apply_move(s, mv[i]);
        int v = minimax(&t, depth - 1, alpha, beta, ply + 1);
        if (aborted) return 0;
        if (v < best) best = v;
        if (v < beta) beta = v;
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
        int alpha = -INF, best_v = -INF, cur = mv[0], cur_idx = 0;
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
            int v = minimax(&t, depth - 1, alpha, INF, 1);
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

static int color_idx(char c) { return c == 'r' ? 0 : c == 'g' ? 1 : c == 'b' ? 2 : -1; }

int main(void) {
    init_tables();
    long long cli_nodes = -1;
    const char *e = getenv("CG_NODES");
    if (e) cli_nodes = atoll(e);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    for (char *p = line; *p; ++p) if (*p != ' ') { int c = color_idx(*p); ME = c < 0 ? 0 : c; break; }
    int first = 1;
    for (;;) {
        State s;
        s.alive[0] = s.alive[1] = s.alive[2] = 1;
        int ok = 1;
        for (int k = 0; k < 3; ++k) {
            if (!fgets(line, sizeof line, stdin)) { ok = 0; break; }
            if (k == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            char col[16] = {0};
            int st = 1;
            if (sscanf(line, "%15s %d", col, &st) == 2) {
                int c = color_idx(col[0]);
                if (c >= 0 && col[1] == 0) s.alive[c] = st == 1;
            }
        }
        if (!ok) break;
        s.blocked = 0; s.pos[0] = s.pos[1] = s.pos[2] = 0;
        for (int y = 0; y < 8; ++y) {
            if (!fgets(line, sizeof line, stdin)) line[0] = 0;
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < 8; ++x) {
                char ch = (size_t)x < len ? line[x] : '#';
                if (ch != '.') s.blocked |= 1ULL << (y * 8 + x);
                int c = color_idx(ch);
                if (c >= 0) s.pos[c] = y * 8 + x;
            }
        }
        if (!fgets(line, sizeof line, stdin)) break;
        int n = atoi(line), nl = 0, legal[64];
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) break;
            char tok[16] = {0};
            if (sscanf(line, "%15s", tok) == 1 && tok[0] >= 'a' && tok[0] <= 'h' && tok[1] >= '1' && tok[1] <= '8' && nl < 64)
                legal[nl++] = (8 - (tok[1] - '0')) * 8 + (tok[0] - 'a');
        }
        s.alive[ME] = 1;
        s.turn = ME;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? 400000 : 60000);
        time_ms = first ? 700 : 70;
        first = 0;
        if (nl == 0) { printf("random\n"); fflush(stdout); continue; }
        int m = search(&s, legal, nl);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%c%c\n", 'a' + m % 8, '0' + 8 - m / 8);
        fflush(stdout);
    }
    return 0;
}
