/* Shobu — minimax déterministe à budget de travail (traduction C11 de shobu_work22.cpp). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN 1000000000
#define ROOT_BEAM 24
#define NODE_BEAM 16
#define WORK_LIMIT 20000
#define MAX_MOVES 2048
#define MAX_DEPTH 4

typedef signed char Cells[64];

typedef struct {
    int px, py, ax, ay, dx, dy;
} Move;

typedef struct {
    int score;
    Move move;
    Cells cells;
    int win;
} ScoredMove;

static const int DIRS[8][2] = {
    {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}
};

static int work = 0;
static int aborted_global = 0;

/* Mémoires fixes, indexées par profondeur pour les appels imbriqués. */
static Move gen_buf[MAX_MOVES];
static Move all_buf[MAX_MOVES];
static ScoredMove scored_buf[MAX_DEPTH + 1][MAX_MOVES];

static int board_of(int x, int y) { return 2 * (y / 4) + x / 4; }
static int is_dark(int b) { return b % 2 == 0; }
static int is_home(int b, int color) { return color == 0 ? b >= 2 : b < 2; }

static int same_board(int x, int y, int ux, int uy, int k) {
    int nx = x + ux * k, ny = y + uy * k;
    return nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && nx / 4 == x / 4 && ny / 4 == y / 4;
}

static int move_cmp(const Move *a, const Move *b) {
    if (a->px != b->px) return a->px < b->px ? -1 : 1;
    if (a->py != b->py) return a->py < b->py ? -1 : 1;
    if (a->ax != b->ax) return a->ax < b->ax ? -1 : 1;
    if (a->ay != b->ay) return a->ay < b->ay ? -1 : 1;
    if (a->dx != b->dx) return a->dx < b->dx ? -1 : 1;
    if (a->dy != b->dy) return a->dy < b->dy ? -1 : 1;
    return 0;
}

static int move_eq(const Move *a, const Move *b) {
    return a->px == b->px && a->py == b->py && a->ax == b->ax &&
           a->ay == b->ay && a->dx == b->dx && a->dy == b->dy;
}

static int gen_moves(const Cells c, int me, Move *out) {
    int n = 0, op = 1 - me;
    for (int py = 0; py < 8; ++py) {
        for (int px = 0; px < 8; ++px) {
            if (c[py*8+px] != me || !is_home(board_of(px,py), me)) continue;
            int passive_dark = is_dark(board_of(px,py));
            for (int di = 0; di < 8; ++di) {
                int ux = DIRS[di][0], uy = DIRS[di][1];
                for (int k = 1; k <= 2; ++k) {
                    if (!same_board(px,py,ux,uy,k) || c[(py+uy*k)*8 + px+ux*k] != -1) break;
                    for (int ay = 0; ay < 8; ++ay) {
                        for (int ax = 0; ax < 8; ++ax) {
                            if (c[ay*8+ax] != me || is_dark(board_of(ax,ay)) == passive_dark) continue;
                            if (!same_board(ax,ay,ux,uy,k)) continue;
                            int cnt = 0, ok = 1;
                            for (int j = 1; j <= k; ++j) {
                                int v = c[(ay+uy*j)*8 + ax+ux*j];
                                if (v == me) { ok = 0; break; }
                                if (v == op) ++cnt;
                            }
                            if (!ok || cnt > 1) continue;
                            if (cnt == 1 && same_board(ax,ay,ux,uy,k+1)) {
                                if (c[(ay+uy*(k+1))*8 + ax+ux*(k+1)] != -1) continue;
                            }
                            out[n].px = px;
                            out[n].py = py;
                            out[n].ax = ax;
                            out[n].ay = ay;
                            out[n].dx = ux * k;
                            out[n].dy = uy * k;
                            ++n;
                        }
                    }
                }
            }
        }
    }
    return n;
}

static void apply_step(const Cells cells, int me, const Move *m, Cells g, int *win) {
    memcpy(g, cells, sizeof(Cells));
    int op = 1 - me;
    int k = m->dx < 0 ? -m->dx : m->dx;
    int ady = m->dy < 0 ? -m->dy : m->dy;
    if (ady > k) k = ady;
    int ux = (m->dx > 0) - (m->dx < 0);
    int uy = (m->dy > 0) - (m->dy < 0);
    g[m->py*8+m->px] = -1;
    g[(m->py+m->dy)*8 + m->px+m->dx] = (signed char)me;
    for (int j = 1; j <= k; ++j) {
        int x = m->ax + ux*j, y = m->ay + uy*j;
        if (g[y*8+x] == op) {
            g[y*8+x] = -1;
            int tx = m->ax + ux*(k+1), ty = m->ay + uy*(k+1);
            if (same_board(m->ax,m->ay,ux,uy,k+1)) g[ty*8+tx] = (signed char)op;
            break;
        }
    }
    g[m->ay*8+m->ax] = -1;
    g[(m->ay+m->dy)*8 + m->ax+m->dx] = (signed char)me;
    *win = 0;
    for (int b = 0; b < 4; ++b) {
        int bx = (b % 2) * 4, by = (b / 2) * 4, cnt = 0;
        for (int y = by; y < by+4; ++y)
            for (int x = bx; x < bx+4; ++x)
                if (g[y*8+x] == op) ++cnt;
        if (cnt == 0) { *win = 1; break; }
    }
}

static int stones(const Cells g, int b, int color) {
    int bx = (b % 2) * 4, by = (b / 2) * 4, n = 0;
    for (int y = by; y < by+4; ++y)
        for (int x = bx; x < bx+4; ++x)
            if (g[y*8+x] == color) ++n;
    return n;
}

static int evaluate(const Cells g, int me) {
    int op = 1 - me, om[4], mm[4];
    int mn_o = 99, mn_m = 99;
    for (int b = 0; b < 4; ++b) {
        om[b] = stones(g, b, op);
        mm[b] = stones(g, b, me);
        if (om[b] < mn_o) mn_o = om[b];
        if (mm[b] < mn_m) mn_m = mm[b];
    }
    if (mn_o == 0) return WIN;
    if (mn_m == 0) return -WIN;
    int sum_m = mm[0] + mm[1] + mm[2] + mm[3];
    int sum_o = om[0] + om[1] + om[2] + om[3];
    int score = 35 * (sum_m - sum_o);
    score += 420 * (4 - mn_o) * (4 - mn_o);
    score -= 320 * (4 - mn_m) * (4 - mn_m);
    for (int b = 0; b < 4; ++b) score += 18 * (mm[b] - om[b]);
    int our_edge = 0, op_edge = 0, our_corner = 0, op_corner = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int c = g[y*8+x];
            if (c == -1) continue;
            int lx = x % 4, ly = y % 4;
            int edge = (lx == 0 || lx == 3 || ly == 0 || ly == 3);
            int corner = ((lx == 0 || lx == 3) && (ly == 0 || ly == 3));
            if (c == me) {
                our_edge += edge;
                our_corner += corner;
            } else {
                op_edge += edge;
                op_corner += corner;
            }
        }
    }
    score += 30 * (op_edge - our_edge) + 45 * (op_corner - our_corner);
    return score;
}

/* score décroissant, puis Move lexicographique px,py,ax,ay,dx,dy */
static int scored_cmp(const void *a, const void *b) {
    const ScoredMove *x = (const ScoredMove *)a, *y = (const ScoredMove *)b;
    if (x->score != y->score) return x->score > y->score ? -1 : 1;
    return move_cmp(&x->move, &y->move);
}

static int ordered_moves(const Cells g, int me, int beam, ScoredMove *out) {
    int nm = gen_moves(g, me, gen_buf);
    int ne = 0;
    for (int i = 0; i < nm; ++i) {
        if (work >= WORK_LIMIT) { aborted_global = 1; break; }
        ++work;
        out[ne].move = gen_buf[i];
        apply_step(g, me, &gen_buf[i], out[ne].cells, &out[ne].win);
        out[ne].score = out[ne].win ? WIN : evaluate(out[ne].cells, me);
        ++ne;
    }
    qsort(out, (size_t)ne, sizeof(*out), scored_cmp);
    return ne < beam ? ne : beam;
}

static int search(const Cells g, int me, int depth, int alpha, int beta) {
    if (aborted_global) return evaluate(g, me);
    if (depth <= 0) return evaluate(g, me);
    ScoredMove *moves = scored_buf[depth];
    int nm = ordered_moves(g, me, NODE_BEAM, moves);
    if (nm == 0) return -WIN;
    int best = -2 * WIN;
    for (int i = 0; i < nm; ++i) {
        const ScoredMove *sm = &moves[i];
        int val = sm->win ? WIN - depth
                          : -search(sm->cells, 1 - me, depth - 1, -beta, -alpha);
        if (val > best) best = val;
        if (best > alpha) alpha = best;
        if (alpha >= beta || aborted_global) break;
    }
    return best;
}

static Move choose(const Cells g, int me) {
    int na = gen_moves(g, me, all_buf);
    Move zero;
    zero.px = zero.py = zero.ax = zero.ay = zero.dx = zero.dy = 0;
    if (na == 0) return zero;

    Move best = all_buf[0];
    int best_val = -2 * WIN;
    work = 0;
    aborted_global = 0;
    for (int depth = 1; depth <= 4; ++depth) {
        ScoredMove *moves = scored_buf[MAX_DEPTH];
        int nm = ordered_moves(g, me, ROOT_BEAM, moves);
        int local_best = -2 * WIN;
        Move local_move = best;
        int aborted = aborted_global;
        for (int i = 0; i < nm; ++i) {
            if (aborted_global) { aborted = 1; break; }
            const ScoredMove *sm = &moves[i];
            int val = sm->win ? WIN - depth
                              : -search(sm->cells, 1 - me, depth - 1, -2 * WIN,
                                        local_best == -2 * WIN ? 2 * WIN : -local_best);
            if (val > local_best) {
                local_best = val;
                local_move = sm->move;
            }
        }
        if (!aborted) {
            best = local_move;
            best_val = local_best;
        }
        if (best_val >= WIN / 2 || aborted) break;
    }
    for (int i = 0; i < na; ++i) {
        if (move_eq(&all_buf[i], &best)) return best;
    }
    return all_buf[0];
}

int main(void) {
    int round_no, my_color;
    while (scanf("%d %d", &round_no, &my_color) == 2) {
        Cells cells;
        for (int y = 0; y < 8; ++y) {
            char row[64];
            if (scanf("%63s", row) != 1) return 0;
            for (int x = 0; x < 8; ++x)
                cells[y*8+x] = row[x] == 'b' ? 0 : (row[x] == 'w' ? 1 : -1);
        }
        char last[64];
        if (scanf("%63s", last) != 1) return 0;
        if (strcmp(last, "-") && strcmp(last, "SWAP")) {
            char tmp[64];
            for (int i = 0; i < 5; ++i)
                if (scanf("%63s", tmp) != 1) return 0;
        }
        int n;
        if (scanf("%d", &n) != 1) return 0;
        Move actions[MAX_MOVES];
        int na = 0, can_swap = 0;
        for (int i = 0; i < n; ++i) {
            char a[64];
            if (scanf("%63s", a) != 1) return 0;
            if (!strcmp(a, "SWAP")) {
                can_swap = 1;
            } else {
                Move m;
                m.px = atoi(a);
                if (scanf("%d %d %d %d %d", &m.py, &m.ax, &m.ay, &m.dx, &m.dy) != 5) return 0;
                actions[na++] = m;
            }
        }
        if (can_swap) {
            printf("SWAP\n");
            fflush(stdout);
            continue;
        }
        Move m = choose(cells, my_color);
        int found = 0;
        for (int i = 0; i < na; ++i) {
            if (move_eq(&actions[i], &m)) { found = 1; break; }
        }
        if (!found && na > 0) m = actions[0];
        printf("%d %d %d %d %d %d\n", m.px, m.py, m.ax, m.ay, m.dx, m.dy);
        fflush(stdout);
    }
    return 0;
}
