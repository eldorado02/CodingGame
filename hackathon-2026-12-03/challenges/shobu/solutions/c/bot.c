/* Shobu — évaluation 1 ply + meilleure réponse adverse + position bord. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define WIN 1000000000
#define BEAM 24
#define NMOVE 2048

typedef struct { int px, py, ax, ay, dx, dy; } Move;
typedef int Cells[64];

static const int DIRS[8][2] = {{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}};

static int board_of(int x, int y) { return 2 * (y / 4) + x / 4; }
static int is_dark(int b) { return b % 2 == 0; }
static int is_home(int b, int color) { return color == 0 ? b >= 2 : b < 2; }

static int same_board(int x, int y, int ux, int uy, int k) {
    int nx = x + ux * k, ny = y + uy * k;
    return nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && nx / 4 == x / 4 && ny / 4 == y / 4;
}

static int move_cmp(const void *a, const void *b) {
    const Move *x = a, *y = b;
    if (x->px != y->px) return x->px - y->px;
    if (x->py != y->py) return x->py - y->py;
    if (x->ax != y->ax) return x->ax - y->ax;
    if (x->ay != y->ay) return x->ay - y->ay;
    if (x->dx != y->dx) return x->dx - y->dx;
    return x->dy - y->dy;
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
                            out[n++] = (Move){px,py,ax,ay,ux*k,uy*k};
                        }
                    }
                }
            }
        }
    }
    return n;
}

static void apply_move(const Cells cells, int me, Move m, Cells g) {
    memcpy(g, cells, sizeof(Cells));
    int op = 1 - me;
    int k = m.dx > m.dy ? m.dx : m.dy;
    if (m.dx < 0) k = -m.dx > k ? -m.dx : k;
    if (m.dy < 0) k = -m.dy > k ? -m.dy : k;
    int ux = (m.dx > 0) - (m.dx < 0), uy = (m.dy > 0) - (m.dy < 0);
    g[m.py*8+m.px] = -1;
    g[(m.py+m.dy)*8 + m.px+m.dx] = me;
    for (int j = 1; j <= k; ++j) {
        int x = m.ax + ux*j, y = m.ay + uy*j;
        if (g[y*8+x] == op) {
            g[y*8+x] = -1;
            int tx = m.ax + ux*(k+1), ty = m.ay + uy*(k+1);
            if (same_board(m.ax,m.ay,ux,uy,k+1)) g[ty*8+tx] = op;
            break;
        }
    }
    g[m.ay*8+m.ax] = -1;
    g[(m.ay+m.dy)*8 + m.ax+m.dx] = me;
}

static int stones(const Cells g, int b, int color) {
    int bx = (b % 2) * 4, by = (b / 2) * 4, n = 0;
    for (int y = by; y < by+4; ++y)
        for (int x = bx; x < bx+4; ++x)
            if (g[y*8+x] == color) ++n;
    return n;
}

static int evaluate(const Cells g, int me) {
    int op = 1 - me, om[4], mm[4], mno = 99, mnm = 99;
    for (int b = 0; b < 4; ++b) {
        om[b] = stones(g, b, op); mm[b] = stones(g, b, me);
        if (om[b] < mno) mno = om[b];
        if (mm[b] < mnm) mnm = mm[b];
    }
    if (mno == 0) return WIN;
    if (mnm == 0) return -WIN;
    int sm = 0, so = 0;
    for (int b = 0; b < 4; ++b) { sm += mm[b]; so += om[b]; }
    int score = 40 * (sm - so);
    score += 300 * ((4 - mno) * (4 - mno));
    score -= 250 * ((4 - mnm) * (4 - mnm));
    for (int b = 0; b < 4; ++b) score += 15 * (mm[b] - om[b]);
    int our_edge = 0, op_edge = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int c = g[y*8+x];
            if (c == -1) continue;
            int lx = x % 4, ly = y % 4;
            if (lx == 0 || lx == 3 || ly == 0 || ly == 3) {
                if (c == me) ++our_edge; else ++op_edge;
            }
        }
    }
    score += 25 * (op_edge - our_edge);
    return score;
}

typedef struct { int s; Move m; Cells ng; } Scored;
typedef struct { int s; Move m; } Rep;

static int scored_cmp(const void *a, const void *b) {
    const Scored *x = a, *y = b;
    if (x->s != y->s) return y->s - x->s;
    return move_cmp(&x->m, &y->m);
}
static int rep_cmp(const void *a, const void *b) {
    const Rep *x = a, *y = b;
    if (x->s != y->s) return x->s - y->s;
    return move_cmp(&x->m, &y->m);
}

static Move choose(const Cells g, int me) {
    Move moves[NMOVE];
    int nm = gen_moves(g, me, moves);
    if (nm == 0) return (Move){0,0,1,1,0,1};
    Scored *scored = malloc(sizeof(Scored) * nm);
    for (int i = 0; i < nm; ++i) {
        apply_move(g, me, moves[i], scored[i].ng);
        scored[i].m = moves[i];
        scored[i].s = evaluate(scored[i].ng, me);
    }
    qsort(scored, nm, sizeof(Scored), scored_cmp);
    Move best = moves[0];
    int best_score = -2 * WIN, have = 0;
    int lim = nm < BEAM ? nm : BEAM;
    for (int idx = 0; idx < lim; ++idx) {
        int s = scored[idx].s;
        Move m = scored[idx].m;
        if (s >= WIN) { free(scored); return m; }
        Move replies[NMOVE];
        int nr = gen_moves(scored[idx].ng, 1 - me, replies);
        if (nr == 0) { free(scored); return m; }
        int worst = WIN;
        for (int i = 0; i < nr && i < BEAM; ++i) {
            Cells rg;
            apply_move(scored[idx].ng, 1 - me, replies[i], rg);
            int s = evaluate(rg, me);
            if (s < worst) worst = s;
        }
        if (!have || worst > best_score || (worst == best_score && move_cmp(&m,&best) < 0)) {
            best_score = worst; best = m; have = 1;
        }
    }
    free(scored);
    return best;
}

int main(void) {
    int round_no, my_color;
    while (scanf("%d %d", &round_no, &my_color) == 2) {
        Cells cells;
        for (int y = 0; y < 8; ++y) {
            char row[64];
            scanf("%63s", row);
            for (int x = 0; x < 8; ++x)
                cells[y*8+x] = row[x] == 'b' ? 0 : (row[x] == 'w' ? 1 : -1);
        }
        char last[64];
        scanf("%63s", last);
        if (strcmp(last, "-") && strcmp(last, "SWAP")) {
            char tmp[64];
            for (int i = 0; i < 5; ++i) scanf("%63s", tmp);
        }
        int n;
        scanf("%d", &n);
        Move actions[NMOVE];
        int na = 0, can_swap = 0;
        for (int i = 0; i < n; ++i) {
            char a[64];
            scanf("%63s", a);
            if (!strcmp(a, "SWAP")) {
                can_swap = 1;
            } else {
                Move m;
                m.px = atoi(a);
                scanf("%d %d %d %d %d", &m.py, &m.ax, &m.ay, &m.dx, &m.dy);
                actions[na++] = m;
            }
        }
        if (can_swap) { printf("SWAP\n"); fflush(stdout); continue; }
        Move m = choose(cells, my_color);
        int found = 0;
        for (int i = 0; i < na; ++i) if (!move_cmp(&actions[i], &m)) { found = 1; break; }
        if (!found && na > 0) m = actions[0];
        printf("%d %d %d %d %d %d\n", m.px, m.py, m.ax, m.ay, m.dx, m.dy);
        fflush(stdout);
    }
    return 0;
}
