/* Checkers (English draughts) — C port of solutions/python/bot.py (v1).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { EMPTY = 0, BM = 1, BK = 2, RM = 3, RK = 4 };
enum { SIDE_B = 0, SIDE_R = 1 };
enum { MAN_V = 100, KING_V = 160 };
#define MAX_PATH 16
#define MAX_MOVES 96
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64
static const int DDX[4] = {-1, 1, -1, 1}, DDY[4] = {-1, -1, 1, 1};

typedef struct { int len; int sq[MAX_PATH]; } Move;
typedef struct { int cell[64]; int side; } State;

static int side_of(int v) { return (v == BM || v == BK) ? SIDE_B : SIDE_R; }
static int is_king(int v) { return v == BK || v == RK; }
static int crown_row(int v) { return side_of(v) == SIDE_B ? 0 : 7; }
static void dir_range(int v, int *d0, int *d1) {
    if (is_king(v)) { *d0 = 0; *d1 = 4; }
    else if (side_of(v) == SIDE_B) { *d0 = 0; *d1 = 2; }
    else { *d0 = 2; *d1 = 4; }
}

static int n_out;
static Move *out_buf;

static void jumps_from(int *c, int sq, int v, Move *path) {
    int x0 = sq % 8, y0 = sq / 8, d0, d1, found = 0;
    dir_range(v, &d0, &d1);
    for (int d = d0; d < d1; ++d) {
        int mx = x0 + DDX[d], my = y0 + DDY[d];
        int tx = x0 + 2 * DDX[d], ty = y0 + 2 * DDY[d];
        if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
        int mid = my * 8 + mx, to = ty * 8 + tx, mv = c[mid];
        if (mv == EMPTY || side_of(mv) == side_of(v) || c[to] != EMPTY) continue;
        if (path->len >= MAX_PATH - 1) continue;
        found = 1;
        c[mid] = EMPTY;
        path->sq[path->len++] = to;
        if (!is_king(v) && ty == crown_row(v)) {
            if (n_out < MAX_MOVES) out_buf[n_out++] = *path;
        } else {
            c[to] = v;
            jumps_from(c, to, v, path);
            c[to] = EMPTY;
        }
        --path->len;
        c[mid] = mv;
    }
    if (!found && path->len > 1 && n_out < MAX_MOVES) out_buf[n_out++] = *path;
}

static int gen_moves(State *s, Move *out) {
    int *c = s->cell;
    n_out = 0; out_buf = out;
    for (int sq = 0; sq < 64; ++sq) {
        int v = c[sq];
        if (v == EMPTY || side_of(v) != s->side) continue;
        c[sq] = EMPTY;
        Move p; p.len = 1; p.sq[0] = sq;
        jumps_from(c, sq, v, &p);
        c[sq] = v;
    }
    if (n_out) return n_out;
    int n = 0;
    for (int sq = 0; sq < 64; ++sq) {
        int v = c[sq];
        if (v == EMPTY || side_of(v) != s->side) continue;
        int x0 = sq % 8, y0 = sq / 8, d0, d1;
        dir_range(v, &d0, &d1);
        for (int d = d0; d < d1; ++d) {
            int x = x0 + DDX[d], y = y0 + DDY[d];
            if (x >= 0 && x < 8 && y >= 0 && y < 8 && c[y * 8 + x] == EMPTY && n < MAX_MOVES) {
                out[n].len = 2; out[n].sq[0] = sq; out[n].sq[1] = y * 8 + x; ++n;
            }
        }
    }
    return n;
}

static State apply_move(const State *s, const Move *m) {
    State t = *s;
    int *c = t.cell;
    int v = c[m->sq[0]];
    c[m->sq[0]] = EMPTY;
    for (int i = 0; i + 1 < m->len; ++i) {
        int a = m->sq[i], b = m->sq[i + 1];
        int ax = a % 8, ay = a / 8, bx = b % 8, by = b / 8;
        if (abs(bx - ax) == 2) c[((ay + by) / 2) * 8 + (ax + bx) / 2] = EMPTY;
    }
    int end = m->sq[m->len - 1];
    if (!is_king(v) && end / 8 == crown_row(v)) v = (v == BM) ? BK : RK;
    c[end] = v;
    t.side = 1 - s->side;
    return t;
}

static int evaluate(const State *s) {
    int sc = 0;
    for (int sq = 0; sq < 64; ++sq) {
        int v = s->cell[sq];
        if (v == EMPTY) continue;
        int y = sq / 8, val;
        if (is_king(v)) val = KING_V;
        else {
            int adv = side_of(v) == SIDE_B ? 7 - y : y;
            val = MAN_V + 2 * adv;
            if (adv == 0) val += 6;
        }
        sc += side_of(v) == SIDE_B ? val : -val;
    }
    return s->side == SIDE_B ? sc : -sc;
}

static long long node_limit = 25000, nodes = 0;
static int time_ms = 70, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static void order_moves(Move *mv, int n) {
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int j = i - 1;
        while (j >= 0 && mv[j].len < x.len) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

static int negamax(const State *s0, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    State s = *s0;
    Move mv[MAX_MOVES];
    int n = gen_moves(&s, mv);
    if (n == 0) return -(WIN - ply);
    if (depth == 0) return evaluate(&s);
    order_moves(mv, n);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        State t = apply_move(&s, &mv[i]);
        int v = -negamax(&t, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

static Move search(const State *root, Move *mv, int n) {
    last_depth = 0;
    order_moves(mv, n);
    Move best = mv[0];
    nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        Move cur = mv[0];
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, &mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur = mv[i]; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        last_depth = depth;
        Move pv = mv[cur_idx];
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

static int parse_path(const char *txt, Move *m) {
    char t[64];
    int k = 0;
    for (const char *p = txt; *p && k < 63; ++p)
        if (*p != ' ' && *p != '\r' && *p != '\n' && *p != '\t') t[k++] = (char)toupper((unsigned char)*p);
    t[k] = 0;
    if (k < 4 || k % 2 || k / 2 > MAX_PATH) return 0;
    m->len = 0;
    for (int i = 0; i < k; i += 2) {
        char col = t[i], row = t[i + 1];
        if (col < 'A' || col > 'H' || row < '1' || row > '8') return 0;
        m->sq[m->len++] = (8 - (row - '0')) * 8 + (col - 'A');
    }
    return 1;
}

int main(void) {
    long long cli_nodes = -1;
    const char *e = getenv("CG_NODES");
    if (e) cli_nodes = atoll(e);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int side = SIDE_R;
    for (char *p = line; *p; ++p) if (*p != ' ') { side = tolower((unsigned char)*p) == 'b' ? SIDE_B : SIDE_R; break; }
    static Move given[512];
    for (;;) {
        State s;
        int ok = 1;
        for (int y = 0; y < 8; ++y) {
            if (!fgets(line, sizeof line, stdin)) { ok = 0; break; }
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < 8; ++x) {
                char ch = (size_t)x < len ? line[x] : '.';
                s.cell[y * 8 + x] = ch == 'b' ? BM : ch == 'B' ? BK : ch == 'r' ? RM : ch == 'R' ? RK : EMPTY;
            }
        }
        if (!ok) break;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if (!fgets(line, sizeof line, stdin)) break;
        int n = atoi(line), ng = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) break;
            char tok[64] = {0};
            if (sscanf(line, "%63s", tok) == 1 && ng < 512 && parse_path(tok, &given[ng])) ++ng;
        }
        s.side = side;
        node_limit = cli_nodes > 0 ? cli_nodes : 25000;
        if (ng == 0) { printf("A1B2\n"); fflush(stdout); continue; }
        if (ng > MAX_MOVES) ng = MAX_MOVES;
        Move m = search(&s, given, ng);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        for (int i = 0; i < m.len; ++i) printf("%c%c", 'A' + m.sq[i] % 8, '0' + 8 - m.sq[i] / 8);
        printf("\n");
        fflush(stdout);
    }
    return 0;
}
