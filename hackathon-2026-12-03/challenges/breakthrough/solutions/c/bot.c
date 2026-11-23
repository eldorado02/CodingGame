/* Breakthrough 8x8 — C port of solutions/python/bot.py (v3 bitboards).
 * Same move order, same tie-breaks, same node budget semantics.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint64_t U64;
#define WHITE 1
#define BLACK 2
#define FILE_A 0x0101010101010101ULL
#define FILE_H 0x8080808080808080ULL
#define RANKM(r) (0xFFULL << (8 * (r)))
#define MAX_MOVES 48
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64

static const int ADV[8] = {0, 4, 10, 20, 36, 60, 110, 0};
enum { PIECE = 1000, DEFENDED = 12, HOME_GUARD = 18 };

typedef struct { int from, to; } Move;
typedef struct { U64 w, b; int to_move; } State;

static U64 own(const State *s, int p) { return p == WHITE ? s->w : s->b; }
static int popc(U64 x) { return __builtin_popcountll(x); }

static U64 attacks(int p, U64 x) {
    if (p == WHITE) return ((x << 7) & ~FILE_H) | ((x << 9) & ~FILE_A);
    return ((x >> 9) & ~FILE_H) | ((x >> 7) & ~FILE_A);
}

static U64 runners(int p, U64 x) { return x & (p == WHITE ? RANKM(6) : RANKM(1)); }

static int winner(const State *s) {
    if (s->w & RANKM(7)) return WHITE;
    if (s->b & RANKM(0)) return BLACK;
    if (!s->w) return BLACK;
    if (!s->b) return WHITE;
    return 0;
}

static int gen_moves(const State *s, Move *out) {
    int p = s->to_move, d = (p == WHITE) ? 1 : -1, n = 0;
    U64 mine = own(s, p), occ = s->w | s->b, x = mine;
    while (x) {
        int sq = __builtin_ctzll(x);
        x &= x - 1;
        int r = sq >> 3, f = sq & 7, nr = r + d;
        if (nr < 0 || nr > 7) continue;
        for (int df = -1; df <= 1; ++df) {
            int nf = f + df;
            if (nf < 0 || nf > 7) continue;
            int t = nr * 8 + nf;
            U64 tb = 1ULL << t;
            if (tb & mine) continue;
            if (df == 0 && (tb & occ)) continue;
            out[n].from = sq; out[n].to = t; ++n;
        }
    }
    return n;
}

static State apply_move(const State *s, Move m) {
    U64 fb = 1ULL << m.from, tb = 1ULL << m.to;
    State t;
    if (s->to_move == WHITE) { t.w = (s->w ^ fb) | tb; t.b = s->b & ~tb; t.to_move = BLACK; }
    else { t.w = s->w & ~tb; t.b = (s->b ^ fb) | tb; t.to_move = WHITE; }
    return t;
}

static int side_score(int p, U64 x) {
    int sc = PIECE * popc(x);
    for (int r = 0; r < 8; ++r) {
        int c = popc(x & RANKM(r));
        if (c) sc += c * ADV[p == WHITE ? r : 7 - r];
    }
    sc += HOME_GUARD * popc(x & (p == WHITE ? RANKM(0) : RANKM(7)));
    sc += DEFENDED * popc(x & attacks(p, x));
    return sc;
}

static int evaluate(const State *s) {
    int p = s->to_move, o = 3 - p;
    return side_score(p, own(s, p)) - side_score(o, own(s, o));
}

static int order_key(const State *s, Move m) {
    int k = 0, goal = (s->to_move == WHITE) ? 7 : 0;
    if ((m.to >> 3) == goal) k += 4;
    if ((1ULL << m.to) & (s->w | s->b)) k += 2;
    return k;
}

static long long node_limit = 60000, nodes = 0;
static int time_ms = 75, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int runner_rule(const State *s, int ply) {
    int p = s->to_move, o = 3 - p;
    if (runners(p, own(s, p))) return WIN - (ply + 1);
    U64 r = runners(o, own(s, o));
    if (r && ((r & (r - 1)) || !(r & attacks(p, own(s, p))))) return -(WIN - (ply + 2));
    return 0;
}

static void order_moves(const State *s, Move *mv, int n) {
    int key[MAX_MOVES];
    for (int i = 0; i < n; ++i) key[i] = order_key(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int w = winner(s);
    if (w) return (w == s->to_move) ? WIN - ply : -(WIN - ply);
    int q = runner_rule(s, ply);
    if (q) return q;
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
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

static Move search(const State *root, Move *mv, int n) {
    last_depth = 0;
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
        last_depth = depth;
        Move pv = mv[cur_idx];
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

static int sq_of(const char *t) { return (t[1] - '1') * 8 + (t[0] - 'a'); }

static void name_of(Move m, char *out) {
    out[0] = (char)('a' + (m.from & 7)); out[1] = (char)('1' + (m.from >> 3));
    out[2] = (char)('a' + (m.to & 7));   out[3] = (char)('1' + (m.to >> 3));
    out[4] = 0;
}

static int valid_move_text(const char *t) {
    return strlen(t) >= 4 && t[0] >= 'a' && t[0] <= 'h' && t[1] >= '1' && t[1] <= '8'
        && t[2] >= 'a' && t[2] <= 'h' && t[3] >= '1' && t[3] <= '8';
}

/* first whitespace-separated token of line into tok (max 63 chars) */
static void first_token(const char *line, char *tok) {
    tok[0] = 0;
    if (sscanf(line, "%63s", tok) != 1) tok[0] = 0;
}

int main(void) {
    long long cli_nodes = -1;
    const char *e = getenv("CG_NODES");
    if (e) cli_nodes = atoll(e);
    State s = {0xFFFFULL, 0xFFFFULL << 48, WHITE};
    int me = 0, first = 1;
    char line[256], opp[64], tok[64];
    static char legal[256][5];
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        first_token(line, opp);
        if (!opp[0]) strcpy(opp, "None");
        if (!fgets(line, sizeof line, stdin)) break;
        int n = atoi(line), nl = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) break;
            first_token(line, tok);
            if (valid_move_text(tok) && nl < 256) { memcpy(legal[nl], tok, 4); legal[nl][4] = 0; ++nl; }
        }
        if (me == 0) me = strcmp(opp, "None") == 0 ? WHITE : BLACK;
        if (valid_move_text(opp)) {
            Move m = {sq_of(opp), sq_of(opp + 2)};
            s.to_move = 3 - me;
            s = apply_move(&s, m);
        }
        s.to_move = me;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? 600000 : 60000);
        time_ms = first ? 700 : 75;
        first = 0;
        Move all[MAX_MOVES], mine[MAX_MOVES];
        int na = gen_moves(&s, all), nm = 0;
        char nm_buf[5];
        for (int i = 0; i < na; ++i) {
            name_of(all[i], nm_buf);
            for (int j = 0; j < nl; ++j)
                if (strcmp(nm_buf, legal[j]) == 0) { mine[nm++] = all[i]; break; }
        }
        char out[5];
        if (nm == 0) {
            if (nl > 0) {
                strcpy(out, legal[0]);
                Move m = {sq_of(out), sq_of(out + 2)};
                s = apply_move(&s, m);
            } else {
                strcpy(out, "a1a2");
            }
        } else {
            Move m = search(&s, mine, nm);
            name_of(m, out);
            s = apply_move(&s, m);
        }
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%s\n", out);
        fflush(stdout);
    }
    return 0;
}
