/* Brandubh 7x7 — C port of solutions/python/bot.py (v9).
 * Same move order, captures, tie-breaks and node budget semantics.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { EMPTY = 0, ATT = 1, DEF = 2, KING = 3 };
enum { SIDE_A = 0, SIDE_D = 1 };
#define THRONE 24
#define MAX_MOVES 128
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64

static const int CORNER_DIAG[4] = {8, 12, 36, 40};
static const int DX[4] = {1, -1, 0, 0}, DY[4] = {0, 0, 1, -1};
static const int KDIST[5] = {0, -600, -250, -80, 0};

typedef struct { int from, to; } Move;
typedef struct { int cell[49]; int side; } State;

static int is_corner(int i) { return i == 0 || i == 6 || i == 42 || i == 48; }
static int side_of(int v) { return v == ATT ? SIDE_A : SIDE_D; }

static int gen_moves(const State *s, Move *out) {
    int n = 0;
    for (int i = 0; i < 49; ++i) {
        int v = s->cell[i];
        if (v == EMPTY || side_of(v) != s->side) continue;
        int x0 = i % 7, y0 = i / 7;
        for (int d = 0; d < 4; ++d) {
            int x = x0 + DX[d], y = y0 + DY[d];
            while (x >= 0 && x < 7 && y >= 0 && y < 7) {
                int j = y * 7 + x;
                if (s->cell[j] != EMPTY) break;
                if (v == KING || !(j == THRONE || is_corner(j))) { out[n].from = i; out[n].to = j; ++n; }
                x += DX[d]; y += DY[d];
            }
        }
    }
    return n;
}

static int hostile(const int *c, int x, int y, int cap_type, int cap_king) {
    if (x < 0 || x > 6 || y < 0 || y > 6) return 0;
    int j = y * 7 + x;
    if (is_corner(j)) return 1;
    int target_att = cap_type != ATT;
    if (j == THRONE) {
        if (c[THRONE] == EMPTY) return !cap_king;
        return target_att;
    }
    int p = c[j];
    if (p != EMPTY) return target_att ? p != ATT : p == ATT;
    return 0;
}

static int is_enemy(int a, int b) { return a == ATT ? b != ATT : b == ATT; }

static State apply_move(const State *s, Move m) {
    State t = *s;
    int *c = t.cell;
    int v = c[m.from];
    c[m.from] = EMPTY;
    c[m.to] = v;
    int ex = m.to % 7, ey = m.to / 7;
    for (int d = 0; d < 4; ++d) {
        int ax = ex + DX[d], ay = ey + DY[d];
        if (ax < 0 || ax > 6 || ay < 0 || ay > 6) continue;
        int a = ay * 7 + ax;
        if (c[a] == EMPTY || !is_enemy(v, c[a])) continue;
        if (c[a] == KING && a == THRONE) {
            int all = 1;
            for (int e = 0; e < 4; ++e)
                if (!hostile(c, 3 + DX[e], 3 + DY[e], ATT, 1)) { all = 0; break; }
            if (all) c[a] = EMPTY;
        } else if (hostile(c, ax + DX[d], ay + DY[d], v, c[a] == KING)) {
            c[a] = EMPTY;
        }
    }
    t.side = 1 - s->side;
    return t;
}

static int encircled(const int *c) {
    int seen[49] = {0}, queue[49], qt = 0, h = 0;
    for (int i = 0; i < 49; ++i) {
        int x = i % 7, y = i / 7;
        if (x == 0 || x == 6 || y == 0 || y == 6) {
            if (c[i] == KING || c[i] == DEF) return 0;
            if (c[i] != ATT) { seen[i] = 1; queue[qt++] = i; }
        }
    }
    while (h < qt) {
        int i = queue[h++];
        if (c[i] == KING || c[i] == DEF) return 0;
        int x = i % 7, y = i / 7;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx >= 0 && nx < 7 && ny >= 0 && ny < 7) {
                int j = ny * 7 + nx;
                if (!seen[j] && c[j] != ATT) { seen[j] = 1; queue[qt++] = j; }
            }
        }
    }
    return 1;
}

static int king_pos(const int *c) {
    for (int i = 0; i < 49; ++i) if (c[i] == KING) return i;
    return -1;
}

static int winner(const State *s) {
    int k = king_pos(s->cell);
    if (k < 0) return SIDE_A;
    if (is_corner(k)) return SIDE_D;
    if (encircled(s->cell)) return SIDE_A;
    return -1;
}

static int king_lines(const int *c, int k) {
    int n = 0, x0 = k % 7, y0 = k / 7;
    for (int d = 0; d < 4; ++d) {
        int x = x0 + DX[d], y = y0 + DY[d];
        while (x >= 0 && x < 7 && y >= 0 && y < 7) {
            int j = y * 7 + x;
            if (c[j] != EMPTY) break;
            if (is_corner(j)) ++n;
            x += DX[d]; y += DY[d];
        }
    }
    return n;
}

static int king_dist(const int *c, int k) {
    int dist[49], queue[49], qt = 0, h = 0;
    for (int i = 0; i < 49; ++i) dist[i] = 9;
    dist[k] = 0; queue[qt++] = k;
    while (h < qt) {
        int i = queue[h++];
        if (is_corner(i)) return dist[i] < 4 ? dist[i] : 4;
        if (dist[i] >= 3) continue;
        int x0 = i % 7, y0 = i / 7;
        for (int d = 0; d < 4; ++d) {
            int x = x0 + DX[d], y = y0 + DY[d];
            while (x >= 0 && x < 7 && y >= 0 && y < 7) {
                int j = y * 7 + x;
                if (c[j] != EMPTY) break;
                if (dist[j] == 9) { dist[j] = dist[i] + 1; queue[qt++] = j; }
                x += DX[d]; y += DY[d];
            }
        }
    }
    return 4;
}

static int evaluate(const State *s) {
    const int *c = s->cell;
    int k = king_pos(c), a = 0, d = 0;
    for (int i = 0; i < 49; ++i) { if (c[i] == ATT) ++a; else if (c[i] == DEF) ++d; }
    int sc = 100 * a - 150 * d;
    int kx = k % 7, ky = k / 7;
    for (int e = 0; e < 4; ++e) {
        int x = kx + DX[e], y = ky + DY[e];
        if (x >= 0 && x < 7 && y >= 0 && y < 7 && c[y * 7 + x] == ATT) sc += 60;
    }
    for (int i = 0; i < 4; ++i) if (c[CORNER_DIAG[i]] == ATT) sc += 60;
    sc += KDIST[king_dist(c, k)];
    if (s->side == SIDE_A && king_lines(c, k) >= 2) sc -= 3000;
    return s->side == SIDE_A ? sc : -sc;
}

static int order_key(const State *s, Move m) {
    int v = s->cell[m.from];
    if (v == KING) return is_corner(m.to) ? 10 : 1;
    if (v == ATT) {
        int k = king_pos(s->cell);
        int dx = abs(m.to % 7 - k % 7), dy = abs(m.to / 7 - k / 7);
        return dx + dy == 1 ? 2 : 0;
    }
    return 0;
}

static long long node_limit = 12000, nodes = 0;
static int time_ms = 35, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
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
    if ((nodes & 255) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int w = winner(s);
    if (w >= 0) return (w == s->side) ? WIN - ply : -(WIN - ply);
    if (s->side == SIDE_D && king_lines(s->cell, king_pos(s->cell))) return WIN - (ply + 1);
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

/* repetition history: (board, side) keys in insertion order, linear search */
#define HMAX 512
static char hkey[HMAX][51];
static int hcnt[HMAX], hn = 0;

static void key_of(const State *s, char *k) {
    for (int i = 0; i < 49; ++i) k[i] = ".ADK"[s->cell[i]];
    k[49] = (char)('0' + s->side);
    k[50] = 0;
}

static int hist_get(const char *k) {
    for (int i = 0; i < hn; ++i) if (strcmp(hkey[i], k) == 0) return hcnt[i];
    return 0;
}

static void hist_add(const char *k) {
    for (int i = 0; i < hn; ++i) if (strcmp(hkey[i], k) == 0) { ++hcnt[i]; return; }
    if (hn < HMAX) { strcpy(hkey[hn], k); hcnt[hn] = 1; ++hn; }
}

int main(void) {
    long long cli_nodes = -1;
    const char *e = getenv("CG_NODES");
    if (e) cli_nodes = atoll(e);
    char line[256];
    int first = 1;
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if (!strpbrk(line, "0123456789")) break;
        int role = atoi(line);
        if (!fgets(line, sizeof line, stdin)) break;
        State s;
        for (int y = 0; y < 7; ++y) {
            if (!fgets(line, sizeof line, stdin)) line[0] = 0;
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < 7; ++x) {
                char ch = (size_t)x < len ? line[x] : '.';
                s.cell[y * 7 + x] = ch == 'A' ? ATT : ch == 'D' ? DEF : ch == 'K' ? KING : EMPTY;
            }
        }
        s.side = role == 0 ? SIDE_A : SIDE_D;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? 200000 : 12000);
        time_ms = first ? 700 : 35;
        first = 0;
        char k[51];
        key_of(&s, k);
        hist_add(k);
        Move all[MAX_MOVES], pool[MAX_MOVES];
        int na = gen_moves(&s, all), np = 0;
        for (int i = 0; i < na; ++i) {
            State t = apply_move(&s, all[i]);
            key_of(&t, k);
            if (hist_get(k) < 2) pool[np++] = all[i];
        }
        if (np == 0) { memcpy(pool, all, sizeof(Move) * (size_t)na); np = na; }
        if (np == 0) { printf("a1a1\n"); fflush(stdout); continue; }
        Move m = search(&s, pool, np);
        State t = apply_move(&s, m);
        key_of(&t, k);
        hist_add(k);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%c%c%c%c\n", 'a' + m.from % 7, '7' - m.from / 7, 'a' + m.to % 7, '7' - m.to / 7);
        fflush(stdout);
    }
    return 0;
}
