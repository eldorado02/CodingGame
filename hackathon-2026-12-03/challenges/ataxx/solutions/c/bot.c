/* Ataxx 7x7 — C port of solutions/python/bot.py (v2 alpha-beta, bitboards).
 * Bit index = y * 7 + x, y = 0 is the first input line (row 7).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint64_t U64;
#define SIZE 7
#define CELLS (SIZE * SIZE)
#define ALL ((1ULL << CELLS) - 1)
#define MAX_MOVES (49 * 25)
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)

enum { FIRST_TURN_NODES = 600000, TURN_NODES = 120000, FIRST_TURN_MS = 700, TURN_MS = 70 };
enum { SPAWN_ORDER_BONUS = 1 };  /* spawns add a piece: try them before equal jumps */

typedef struct { int src, dst; } Move;              /* src = -1 for a spawn */
typedef struct { U64 mine, theirs, walls; } State;  /* mine = side to move */

static U64 adj[CELLS], jump[CELLS];

static void init_tables(void) {
    for (int i = 0; i < CELLS; ++i) {
        adj[i] = jump[i] = 0;
        for (int j = 0; j < CELLS; ++j) {
            int dx = abs(j % SIZE - i % SIZE), dy = abs(j / SIZE - i / SIZE);
            int d = dx > dy ? dx : dy;
            if (d == 1) adj[i] |= 1ULL << j;
            else if (d == 2) jump[i] |= 1ULL << j;
        }
    }
}

static int popc(U64 x) { return __builtin_popcountll(x); }
static U64 empty_of(const State *s) { return ALL & ~(s->mine | s->theirs | s->walls); }

static int gen_moves(const State *s, Move *out) {
    U64 empty = empty_of(s), spawn_targets = 0;
    for (U64 x = s->mine; x; x &= x - 1) spawn_targets |= adj[__builtin_ctzll(x)];
    int n = 0;
    for (U64 t = spawn_targets & empty; t; t &= t - 1) { out[n].src = -1; out[n].dst = __builtin_ctzll(t); ++n; }
    for (U64 x = s->mine; x; x &= x - 1) {
        int p = __builtin_ctzll(x);
        for (U64 t = jump[p] & empty; t; t &= t - 1) { out[n].src = p; out[n].dst = __builtin_ctzll(t); ++n; }
    }
    return n;
}

static State apply_move(const State *s, Move m) {
    U64 mine = s->mine | (1ULL << m.dst);
    if (m.src >= 0) mine &= ~(1ULL << m.src);
    U64 flipped = adj[m.dst] & s->theirs;
    State t = {s->theirs & ~flipped, mine | flipped, s->walls};  /* point of view switches */
    return t;
}

static int order_key(const State *s, Move m) {
    return 2 * popc(adj[m.dst] & s->theirs) + (m.src < 0 ? SPAWN_ORDER_BONUS : 0);
}

static int evaluate(const State *s) { return popc(s->mine) - popc(s->theirs); }

static void order_moves(const State *s, Move *mv, int n) {
    static int key[MAX_MOVES];
    for (int i = 0; i < n; ++i) key[i] = order_key(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
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

static int final_score(const State *s, int ply) {
    int diff = evaluate(s);
    return diff > 0 ? WIN - ply : diff < 0 ? -(WIN - ply) : 0;
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    if (!s->mine || !s->theirs) return final_score(s, ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    if (n == 0) {
        State passed = {s->theirs, s->mine, s->walls};
        if (!gen_moves(&passed, mv)) return final_score(s, ply);
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
    if (t[0] < 'a' || t[0] > 'g' || t[1] < '1' || t[1] > '7') return -1;
    return (7 - (t[1] - '0')) * SIZE + (t[0] - 'a');
}

static int parse_action(const char *t, Move *m) {
    size_t len = strlen(t);
    if (len == 2) { m->src = -1; m->dst = square(t); return m->dst >= 0; }
    if (len == 4) { m->src = square(t); m->dst = square(t + 2); return m->src >= 0 && m->dst >= 0; }
    return 0;
}

static void print_square(int i) { printf("%c%c", 'a' + i % SIZE, '0' + 7 - i / SIZE); }

int main(void) {
    init_tables();
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    char me = line[0];
    int first = 1;
    static Move legal[MAX_MOVES];
    for (;;) {
        State s = {0, 0, 0};
        for (int y = 0; y < SIZE; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            if (y == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < SIZE; ++x) {
                char ch = (size_t)x < len ? line[x] : '#';
                U64 bit = 1ULL << (y * SIZE + x);
                if (ch == '#') s.walls |= bit;
                else if (ch == me) s.mine |= bit;
                else if (ch == '0' || ch == '1') s.theirs |= bit;
            }
        }
        if (!fgets(line, sizeof line, stdin) || !fgets(line, sizeof line, stdin)) return 0;
        int n = atoi(line), nl = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char tok[32] = {0};
            if (sscanf(line, "%31s", tok) == 1 && nl < MAX_MOVES && parse_action(tok, &legal[nl])) ++nl;
        }
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = first ? FIRST_TURN_MS : TURN_MS;
        first = 0;
        if (nl == 0) { printf("random\n"); fflush(stdout); continue; }
        Move best = search(&s, legal, nl);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        if (best.src >= 0) print_square(best.src);
        print_square(best.dst);
        printf("\n");
        fflush(stdout);
    }
}
