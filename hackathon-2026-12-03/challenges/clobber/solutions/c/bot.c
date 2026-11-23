/* Clobber — C port of solutions/python/bot.py (v1 alpha-beta, mobility eval).
 * Bit index = y * 8 + x, y = 0 is the first input line (top row).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint64_t U64;
#define STRIDE 8
#define MAX_MOVES (64 * 4)
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)
#define COL0 0x0101010101010101ULL
#define COL7 0x8080808080808080ULL

enum { FIRST_TURN_NODES = 400000, TURN_NODES = 60000, FIRST_TURN_MS = 700, TURN_MS = 100 };
static const int DX[4] = {0, 0, -1, 1}, DY[4] = {-1, 1, 0, 0};  /* same order as Python DIRS */

typedef struct { int src, dst; } Move;
typedef struct { U64 mine, theirs; } State;  /* mine = side to move */

static int board_size = 8, cell_count = 0, cells[64], nb[64][4], nb_count[64];
static U64 board_mask = 0;

static void init_board(int n) {
    board_size = n;
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x) {
            int i = y * STRIDE + x, k = 0;
            cells[cell_count++] = i;
            board_mask |= 1ULL << i;
            for (int d = 0; d < 4; ++d) {
                int nx = x + DX[d], ny = y + DY[d];
                if (nx >= 0 && nx < n && ny >= 0 && ny < n) nb[i][k++] = ny * STRIDE + nx;
            }
            nb_count[i] = k;
        }
}

static int gen_moves(const State *s, Move *out) {
    int n = 0;
    for (int c = 0; c < cell_count; ++c) {
        int i = cells[c];
        if (!((s->mine >> i) & 1)) continue;
        for (int k = 0; k < nb_count[i]; ++k)
            if ((s->theirs >> nb[i][k]) & 1) { out[n].src = i; out[n].dst = nb[i][k]; ++n; }
    }
    return n;
}

/* one shift per direction, then popcount; column masks remove row wrap */
static int count_moves(U64 own, U64 other) {
    U64 up = own >> STRIDE, down = (own << STRIDE) & board_mask;
    U64 left = (own >> 1) & ~COL7 & board_mask, right = (own << 1) & ~COL0 & board_mask;
    return __builtin_popcountll(up & other) + __builtin_popcountll(down & other)
         + __builtin_popcountll(left & other) + __builtin_popcountll(right & other);
}

static State apply_move(const State *s, Move m) {
    U64 mine = (s->mine & ~(1ULL << m.src)) | (1ULL << m.dst);
    State t = {s->theirs & ~(1ULL << m.dst), mine};  /* side to move switches */
    return t;
}

static int evaluate(const State *s) { return count_moves(s->mine, s->theirs) - count_moves(s->theirs, s->mine); }

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
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    if (n == 0) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
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

static void print_square(int i) { printf("%c%d", 'a' + i % STRIDE, board_size - i / STRIDE); }

int main(void) {
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int n = atoi(line);
    if (n > STRIDE) n = STRIDE;
    if (n < 1) n = 8;
    init_board(n);
    if (!fgets(line, sizeof line, stdin)) return 0;
    char me = 'w';
    for (char *p = line; *p; ++p) if (*p != ' ') { me = *p; break; }
    int first = 1;
    for (;;) {
        State s = {0, 0};
        for (int y = 0; y < n; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            if (y == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < n && (size_t)x < len; ++x) {
                U64 bit = 1ULL << (y * STRIDE + x);
                if (line[x] == me) s.mine |= bit;
                else if (line[x] == 'w' || line[x] == 'b') s.theirs |= bit;
            }
        }
        if (!fgets(line, sizeof line, stdin) || !fgets(line, sizeof line, stdin)) return 0;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = first ? FIRST_TURN_MS : TURN_MS;
        first = 0;
        Move tmp[MAX_MOVES];
        if (!gen_moves(&s, tmp)) { printf("random\n"); fflush(stdout); continue; }
        Move m = search(&s);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        print_square(m.src);
        print_square(m.dst);
        printf("\n");
        fflush(stdout);
    }
}
