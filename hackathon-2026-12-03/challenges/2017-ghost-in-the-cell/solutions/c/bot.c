/* Ghost in the Cell (Wood 4) — heuristics time-aware + plusieurs ordres/tour. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 16
#define MAXE 256
#define INF 1000000
#define HORIZON 6

typedef struct { int owner, src, dst, cnt, eta; } Arrival;
typedef struct { int dst, eta; } BombEvent;

typedef struct {
    int owner[MAXN], units[MAXN], prod[MAXN], block[MAXN];
    BombEvent bombs[MAXE];
    int nbombs;
} State;

typedef struct { int owner, own, hostile; } Proj;

static int dista[MAXN][MAXN], through_[MAXN], N;
static long long orders[MAXE][4];
static int norders;

static Proj project_state(const State *s, int i, int t, const Arrival *a, int na) {
    int own = s->owner[i] == 1 ? s->units[i] : 0;
    int hostile = s->owner[i] != 1 ? s->units[i] : 0;
    int ownr = s->owner[i], blk = s->block[i];
    for (int turn = 1; turn <= t; ++turn) {
        for (int k = 0; k < na; ++k)
            if (a[k].dst == i && a[k].eta == turn) {
                if (a[k].owner == 1) own += a[k].cnt; else hostile += a[k].cnt;
            }
        int suppressed = blk > 0;
        for (int k = 0; k < s->nbombs; ++k)
            if (s->bombs[k].dst == i && s->bombs[k].eta <= turn &&
                turn <= s->bombs[k].eta + 4) suppressed = 1;
        if (!suppressed && ownr != 0) {
            if (ownr == 1) own += s->prod[i]; else hostile += s->prod[i];
        }
        for (int k = 0; k < s->nbombs; ++k) {
            if (s->bombs[k].dst == i && s->bombs[k].eta == turn) {
                int victim = ownr == 1 ? own : hostile;
                int loss = victim / 2; if (loss < 10) loss = 10;
                if (ownr == 1) { own -= loss; if (own < 0) own = 0; }
                else { hostile -= loss; if (hostile < 0) hostile = 0; }
            }
        }
        if (blk > 0) --blk;
        if (own > hostile) { ownr = 1; own -= hostile; hostile = 0; }
        else if (hostile > own) { ownr = -1; hostile -= own; own = 0; }
        else { own = hostile = 0; }
    }
    Proj p = {ownr, own, hostile};
    return p;
}

static int defense_need(const State *s, int i, const Arrival *a, int na) {
    if (s->owner[i] != 1) return 0;
    int need = 0;
    for (int t = 1; t <= HORIZON; ++t) {
        Proj p = project_state(s, i, t, a, na);
        if (p.hostile > p.own) {
            int candidate = p.hostile + 1 - p.own;
            if (candidate > need) need = candidate;
        }
    }
    return need;
}

static int capture_need(const State *s, int i, int eta, const Arrival *a, int na) {
    Proj p = project_state(s, i, eta, a, na);
    if (p.own > p.hostile) return 0;
    return p.hostile + 1;
}

static int used_from(const Arrival *a, int na, int src) {
    int used = 0;
    for (int k = 0; k < na; ++k) if (a[k].src == src) used += a[k].cnt;
    return used;
}

static int add_order(int kind, int a, int b, int c) {
    if (norders >= MAXE) return 0;
    orders[norders][0] = kind; orders[norders][1] = a;
    orders[norders][2] = b; orders[norders][3] = c;
    ++norders;
    return 1;
}

static void decide(State *s, const double *value, int turn, int *bombs_left,
                   Arrival *arrivals, int *narrivals) {
    int order_idx[MAXE], norder_idx = 0;
    int threatened[MAXN], nth = 0;
    for (int i = 0; i < N; ++i) if (s->owner[i] == 1) threatened[nth++] = i;
    for (int i = 1; i < nth; ++i) {
        int v = threatened[i], j = i - 1;
        while (j >= 0 && value[threatened[j]] < value[v]) {
            threatened[j + 1] = threatened[j]; --j;
        }
        threatened[j + 1] = v;
    }
    for (int h = 0; h < nth; ++h) {
        int dst = threatened[h];
        for (int rep = 0; rep < 2; ++rep) {
            int need = defense_need(s, dst, arrivals, *narrivals);
            if (need <= 0) break;
            int helpers[MAXN], nh = 0;
            for (int src = 0; src < N; ++src)
                if (src != dst && s->owner[src] == 1 && dista[src][dst] < INF &&
                    dista[src][dst] <= need + 2) helpers[nh++] = src;
            for (int i = 1; i < nh; ++i) {
                int v = helpers[i], j = i - 1;
                while (j >= 0 && (dista[helpers[j]][dst] > dista[v][dst] ||
                       (dista[helpers[j]][dst] == dista[v][dst] &&
                        s->units[helpers[j]] < s->units[v]))) {
                    helpers[j + 1] = helpers[j]; --j;
                }
                helpers[j + 1] = v;
            }
            int sent = 0;
            for (int k = 0; k < nh; ++k) {
                int src = helpers[k];
                int spare = s->units[src] - used_from(arrivals, *narrivals, src)
                            - defense_need(s, src, arrivals, *narrivals);
                if (spare <= 0) continue;
                int cnt = spare < need ? spare : need;
                if (cnt <= 0) continue;
                arrivals[*narrivals] = (Arrival){1, src, dst, cnt, dista[src][dst]};
                ++*narrivals;
                add_order(0, src, dst, cnt);
                order_idx[norder_idx++] = norders - 1;
                need -= cnt; sent = 1;
                if (need <= 0) break;
            }
            if (!sent) break;
        }
    }

    for (int rep = 0; rep < 40; ++rep) {
        double best = 0.0; int bs = -1, bd = -1, bc = 0;
        for (int src = 0; src < N; ++src) {
            if (s->owner[src] != 1) continue;
            int spare = s->units[src] - used_from(arrivals, *narrivals, src)
                        - defense_need(s, src, arrivals, *narrivals);
            if (spare <= 0) continue;
            for (int dst = 0; dst < N; ++dst) {
                if (dst == src || s->owner[dst] == 1 || dista[src][dst] >= INF) continue;
                int eta = dista[src][dst];
                int need = capture_need(s, dst, eta, arrivals, *narrivals);
                double score; int cnt;
                if (s->owner[dst] == 0) {
                    cnt = spare < need ? spare : need;
                    if (cnt < 1) continue;
                    double needd = need < 2 ? 2.0 : pow((double)need, 0.7);
                    score = value[dst] / ((eta + 0.35) * (eta + 0.35) * needd);
                } else {
                    if ((double)spare < 0.7 * need) continue;
                    cnt = spare;
                    score = value[dst] / ((eta + 0.35) * (eta + 0.35) * 8.0);
                }
                score += 0.0001 * cnt;
                if (bs < 0 || score > best) { best = score; bs = src; bd = dst; bc = cnt; }
            }
        }
        if (bs < 0) break;
        arrivals[*narrivals] = (Arrival){1, bs, bd, bc, dista[bs][bd]};
        ++*narrivals;
        add_order(0, bs, bd, bc);
        order_idx[norder_idx++] = norders - 1;
    }

    if (*bombs_left > 0) {
        int bombed[MAXE], nb = 0;
        for (int k = 0; k < s->nbombs; ++k) bombed[nb++] = s->bombs[k].dst;
        int bomb_dst = -1; double bomb_score = 0.0;
        if (turn <= 2) {
            int srcs[MAXN], ns = 0;
            for (int i = 0; i < N; ++i) if (s->owner[i] == -1) srcs[ns++] = i;
            if (ns == 0) for (int i = 0; i < N; ++i) if (s->owner[i] == 0) srcs[ns++] = i;
            for (int dst = 0; dst < N; ++dst) {
                if (s->owner[dst] == 1) continue;
                int seen = 0;
                for (int k = 0; k < nb; ++k) if (bombed[k] == dst) seen = 1;
                if (seen) continue;
                int et = INF;
                for (int k = 0; k < ns; ++k) if (dista[srcs[k]][dst] < et) et = dista[srcs[k]][dst];
                if (et >= INF || et > 10 || s->prod[dst] <= 0) continue;
                double score = (double)s->prod[dst] * s->prod[dst] * s->prod[dst] /
                               (et < 1 ? 1 : et);
                if (bomb_dst < 0 || score > bomb_score) { bomb_score = score; bomb_dst = dst; }
            }
        } else {
            for (int dst = 0; dst < N; ++dst) {
                if (s->owner[dst] != -1 || s->units[dst] < 18) continue;
                int seen = 0;
                for (int k = 0; k < nb; ++k) if (bombed[k] == dst) seen = 1;
                if (seen) continue;
                double score = s->units[dst] + 10.0 * s->prod[dst];
                if (bomb_dst < 0 || score > bomb_score) { bomb_score = score; bomb_dst = dst; }
            }
        }
        if (bomb_dst >= 0) {
            int src = -1;
            for (int s2 = 0; s2 < N; ++s2) {
                if (s->owner[s2] != 1 || dista[s2][bomb_dst] >= INF) continue;
                int blocked = 0;
                for (int k = 0; k < norder_idx; ++k)
                    if (orders[order_idx[k]][0] == 0 &&
                        orders[order_idx[k]][1] == s2 &&
                        orders[order_idx[k]][2] == bomb_dst) blocked = 1;
                if (blocked) continue;
                if (src < 0 || dista[s2][bomb_dst] < dista[src][bomb_dst]) src = s2;
            }
            if (src >= 0) { add_order(1, src, bomb_dst, 0); --*bombs_left; }
        }
    }
}

int main(void) {
    char buf[256];
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    N = (int)strtol(buf, NULL, 10);
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    int m = (int)strtol(buf, NULL, 10);
    for (int i = 0; i < MAXN; ++i)
        for (int j = 0; j < MAXN; ++j) dista[i][j] = i == j ? 0 : INF;
    for (int k = 0; k < m; ++k) {
        if (!fgets(buf, sizeof buf, stdin)) return 0;
        int a, b, w;
        if (sscanf(buf, "%d %d %d", &a, &b, &w) != 3) return 0;
        if (w < dista[a][b]) dista[a][b] = dista[b][a] = w;
    }
    for (int k = 0; k < N; ++k)
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (dista[i][k] + dista[k][j] < dista[i][j])
                    dista[i][j] = dista[i][k] + dista[k][j];
    for (int i = 0; i < N; ++i)
        for (int k = 0; k < N; ++k) {
            if (i == k) continue;
            for (int j = 0; j < N; ++j)
                if (j != i && j != k && dista[i][j] == dista[i][k] + dista[k][j])
                    ++through_[k];
        }
    int turn = 0, bombs_left = 2;
    while (fgets(buf, sizeof buf, stdin)) {
        int cnt = (int)strtol(buf, NULL, 10);
        State s; memset(&s, 0, sizeof s);
        Arrival arrivals[MAXE]; int na = 0;
        int own_bombs = 0;
        for (int e = 0; e < cnt; ++e) {
            if (!fgets(buf, sizeof buf, stdin)) return 0;
            char type[16]; int eid, a1, a2, a3, a4, a5;
            if (sscanf(buf, "%d %15s %d %d %d %d %d", &eid, type, &a1, &a2, &a3, &a4, &a5) != 7)
                return 0;
            if (strcmp(type, "FACTORY") == 0) {
                s.owner[eid] = a1; s.units[eid] = a2; s.prod[eid] = a3; s.block[eid] = a4;
            } else if (strcmp(type, "TROOP") == 0) {
                arrivals[na++] = (Arrival){a1, -1, a3, a4, a5};
            } else if (strcmp(type, "BOMB") == 0 && a1 == 1) {
                ++own_bombs;
                if (a3 >= 0 && a4 > 0) s.bombs[s.nbombs++] = (BombEvent){a3, a4};
            }
        }
        int remain = 2 - own_bombs;
        if (remain < bombs_left) bombs_left = remain;
        double value[MAXN];
        for (int i = 0; i < N; ++i)
            value[i] = s.prod[i] + 0.1 + 0.01 * sqrt((double)through_[i]);
        norders = 0;
        decide(&s, value, turn, &bombs_left, arrivals, &na);
        if (norders == 0) printf("WAIT\n");
        else {
            for (int k = 0; k < norders; ++k) {
                if (k) printf(";");
                if (orders[k][0] == 0) printf("MOVE %lld %lld %lld", orders[k][1], orders[k][2], orders[k][3]);
                else printf("BOMB %lld %lld", orders[k][1], orders[k][2]);
            }
            printf("\n");
        }
        fflush(stdout);
        ++turn;
    }
    return 0;
}
