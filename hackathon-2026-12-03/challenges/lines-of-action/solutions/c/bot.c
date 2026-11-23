/* Lines of Action 8x8 — C port of solutions/python/bot.py (v1 alpha-beta).
 * Index = y * 8 + x, y = 0 is the first input line (rank 8).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 8
#define CELLS (N * N)
#define MAX_MOVES (12 * 8)
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)
#define EMPTY '.'

enum { FIRST_TURN_NODES = 100000, TURN_NODES = 20000, FIRST_TURN_MS = 700, TURN_MS = 100, GROUP_WEIGHT = 10 };
static const int DX[8] = {0, 0, -1, 1, -1, 1, -1, 1}, DY[8] = {-1, 1, 0, 0, -1, 1, 1, -1};

typedef struct { int src, dst; } Move;
typedef struct { char cell[CELLS]; char me, op; } State;

static int line_count(const char *c, int x, int y, int dx, int dy) {
    int n = 1;  /* the moving checker itself */
    for (int sgn = 1; sgn >= -1; sgn -= 2)
        for (int cx = x + sgn * dx, cy = y + sgn * dy; cx >= 0 && cx < N && cy >= 0 && cy < N; cx += sgn * dx, cy += sgn * dy)
            if (c[cy * N + cx] != EMPTY) ++n;
    return n;
}

static int gen_moves(const State *s, Move *out) {
    int n = 0;
    for (int i = 0; i < CELLS; ++i) {
        if (s->cell[i] != s->me) continue;
        int x = i % N, y = i / N;
        for (int d = 0; d < 8; ++d) {
            int k = line_count(s->cell, x, y, DX[d], DY[d]);
            int tx = x + k * DX[d], ty = y + k * DY[d];
            if (tx < 0 || tx >= N || ty < 0 || ty >= N) continue;
            int t = ty * N + tx, blocked = 0;
            if (s->cell[t] == s->me) continue;
            for (int j = 1; j < k && !blocked; ++j) blocked = s->cell[(y + j * DY[d]) * N + x + j * DX[d]] == s->op;
            if (!blocked) { out[n].src = i; out[n].dst = t; ++n; }
        }
    }
    return n;
}

static State apply_move(const State *s, Move m) {
    State t = *s;
    t.cell[m.dst] = s->cell[m.src];
    t.cell[m.src] = EMPTY;
    t.me = s->op;
    t.op = s->me;
    return t;
}

static int groups(const char *c, char color) {
    int seen[CELLS] = {0}, stack[CELLS], count = 0;
    for (int i = 0; i < CELLS; ++i) {
        if (c[i] != color || seen[i]) continue;
        ++count;
        int top = 0;
        stack[top++] = i;
        seen[i] = 1;
        while (top) {
            int j = stack[--top], x = j % N, y = j / N;
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
                    int k = ny * N + nx;
                    if (!seen[k] && c[k] == color) { seen[k] = 1; stack[top++] = k; }
                }
        }
    }
    return count;
}

/* s is the position after a move by s->op: the mover wins ties */
static char winner_after_move(const State *s) {
    if (groups(s->cell, s->op) <= 1) return s->op;
    if (groups(s->cell, s->me) <= 1) return s->me;
    return 0;
}

static int side_score(const char *c, char color) {
    int cells[CELLS], n = 0, sx = 0, sy = 0, spread = 0;
    for (int i = 0; i < CELLS; ++i) if (c[i] == color) { cells[n++] = i; sx += i % N; sy += i / N; }
    if (!n) return 0;
    int cx = sx / n, cy = sy / n;
    for (int k = 0; k < n; ++k) {
        int dx = abs(cells[k] % N - cx), dy = abs(cells[k] / N - cy);
        spread += dx > dy ? dx : dy;
    }
    return -spread - GROUP_WEIGHT * (groups(c, color) - 1);
}

static int evaluate(const State *s) { return side_score(s->cell, s->me) - side_score(s->cell, s->op); }

static int order_key(const State *s, Move m) { return s->cell[m.dst] == s->op ? 1 : 0; }

static void order_moves(const State *s, Move *mv, int n) {  /* stable, captures first */
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = order_key(s, x), j = i - 1;
        while (j >= 0 && order_key(s, mv[j]) < k) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
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

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    char w = winner_after_move(s);
    if (w) return w == s->me ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    if (!n) {
        State passed = *s;
        passed.me = s->op;
        passed.op = s->me;
        return -negamax(&passed, depth - 1, -beta, -alpha, ply + 1);
    }
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

static Move search(const State *root, Move *mv, int n) {
    order_moves(root, mv, n);
    Move best = mv[0];
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

static int square(const char *t) {
    if (t[0] < 'a' || t[0] > 'h' || t[1] < '1' || t[1] > '8') return -1;
    return (N - (t[1] - '0')) * N + (t[0] - 'a');
}

int main(void) {
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    char me = line[0] == 'w' ? 'w' : 'b', op = me == 'b' ? 'w' : 'b';
    int first = 1;
    static Move legal[256];
    for (;;) {
        State s;
        for (int y = 0; y < N; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            if (y == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < N; ++x) s.cell[y * N + x] = (size_t)x < len ? line[x] : EMPTY;
        }
        s.me = me;
        s.op = op;
        if (!fgets(line, sizeof line, stdin) || !fgets(line, sizeof line, stdin)) return 0;
        int n = atoi(line), nl = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char tok[32] = {0};
            if (sscanf(line, "%31s", tok) != 1 || strlen(tok) != 4 || nl >= 256) continue;
            int a = square(tok), b = square(tok + 2);
            if (a >= 0 && b >= 0) { legal[nl].src = a; legal[nl].dst = b; ++nl; }
        }
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = first ? FIRST_TURN_MS : TURN_MS;
        first = 0;
        if (nl == 0) { printf("pass\n"); fflush(stdout); continue; }
        Move m = search(&s, legal, nl);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%c%c%c%c\n", 'a' + m.src % N, '0' + N - m.src / N, 'a' + m.dst % N, '0' + N - m.dst / N);
        fflush(stdout);
    }
}
