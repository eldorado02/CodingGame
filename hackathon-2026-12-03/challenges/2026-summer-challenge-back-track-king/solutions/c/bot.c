/* Back Track King (Bronze+) — connexion désirée la moins chère et DISRUPT
   ciblé sur les régions où l'adversaire possède le plus de rails. */
#include <stdio.h>
#include <string.h>

#define MAXCELL 1024
#define MAXREG 1024
#define MAXTID 64
#define MAXPAIR 1024
#define INF 1000000000

static int region_at[MAXCELL], terrain[MAXCELL];
static int town_x[MAXTID], town_y[MAXTID], town_seen[MAXTID];
static unsigned char town_region[MAXREG];
static int desired_a[MAXPAIR], desired_b[MAXPAIR], desired_n;

static int is_inked(const char *s) {
    return s[0] != '0' && s[0] != 'f' && s[0] != 'F';
}

static void parse_active(const char *s, unsigned char pair[MAXTID][MAXTID],
                         int *cell_has) {
    *cell_has = 0;
    const char *p = s;
    while (*p) {
        int a = 0, b = 0, ok = 0;
        while (*p >= '0' && *p <= '9') { a = a * 10 + (*p - '0'); ++p; ok = 1; }
        if (!ok || *p != '-') { while (*p && *p != ',') ++p; if (*p == ',') ++p; continue; }
        ++p;
        ok = 0;
        while (*p >= '0' && *p <= '9') { b = b * 10 + (*p - '0'); ++p; ok = 1; }
        if (ok && a < MAXTID && b < MAXTID) {
            int lo = a < b ? a : b, hi = a < b ? b : a;
            pair[lo][hi] = 1;
            *cell_has = 1;
        }
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
    }
}

static int dijkstra(int width, int height, int cells, int si, int ti,
                    const int *owner, const unsigned char *inked) {
    static int dist[MAXCELL];
    static unsigned char done[MAXCELL];
    for (int i = 0; i < cells; ++i) { dist[i] = INF; done[i] = 0; }
    dist[si] = 0;
    for (int step = 0; step < cells; ++step) {
        int best = -1, bd = INF;
        for (int i = 0; i < cells; ++i)
            if (!done[i] && dist[i] < bd) { bd = dist[i]; best = i; }
        if (best < 0 || best == ti) break;
        done[best] = 1;
        int x = best % width, y = best / width;
        const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
            int ni = ny * width + nx;
            if (done[ni] || inked[ni]) continue;
            int nd = bd + (owner[ni] >= 0 ? 0 : terrain[ni] + 1);
            if (nd < dist[ni]) dist[ni] = nd;
        }
    }
    return dist[ti];
}

int main(void) {
    int my_id, width, height;
    if (scanf("%d %d %d", &my_id, &width, &height) != 3) return 0;
    int cells = width * height;
    if (cells > MAXCELL) return 0;
    for (int i = 0; i < cells; ++i) {
        if (scanf("%d %d", &region_at[i], &terrain[i]) != 2) return 0;
    }
    int town_count;
    if (scanf("%d", &town_count) != 1) return 0;
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        char wanted[512];
        if (scanf("%d %d %d %511s", &id, &x, &y, wanted) != 4) return 0;
        if (id >= 0 && id < MAXTID) {
            town_x[id] = x; town_y[id] = y; town_seen[id] = 1;
            int idx = y * width + x;
            if (idx >= 0 && idx < cells && region_at[idx] >= 0 &&
                region_at[idx] < MAXREG)
                town_region[region_at[idx]] = 1;
        }
        if (strcmp(wanted, "x") != 0) {
            const char *p = wanted;
            while (*p) {
                int v = 0, ok = 0;
                while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); ++p; ok = 1; }
                if (ok && desired_n < MAXPAIR) {
                    int lo = id < v ? id : v, hi = id < v ? v : id;
                    int found = 0;
                    for (int k = 0; k < desired_n; ++k)
                        if (desired_a[k] == lo && desired_b[k] == hi) { found = 1; break; }
                    if (!found) { desired_a[desired_n] = lo; desired_b[desired_n] = hi; ++desired_n; }
                }
                while (*p && *p != ',') ++p;
                if (*p == ',') ++p;
            }
        }
    }

    static int owner[MAXCELL], instability[MAXCELL];
    static unsigned char inked[MAXCELL];
    static int cell_active[MAXCELL];
    static unsigned char active_pair[MAXTID][MAXTID];
    static int mine[MAXREG], foe[MAXREG], inst[MAXREG];
    int my_score, foe_score;
    while (scanf("%d %d", &my_score, &foe_score) == 2) {
        memset(active_pair, 0, sizeof(active_pair));
        for (int i = 0; i < cells; ++i) {
            char ink[32], act[512];
            if (scanf("%d %d %31s %511s", &owner[i], &instability[i], ink, act) != 4)
                return 0;
            inked[i] = (unsigned char)is_inked(ink);
            parse_active(act, active_pair, &cell_active[i]);
        }

        int best_d = INF, best_a = -1, best_b = -1;
        for (int k = 0; k < desired_n; ++k) {
            int a = desired_a[k], b = desired_b[k];
            if (!town_seen[a] || !town_seen[b]) continue;
            if (a < MAXTID && b < MAXTID && active_pair[a][b]) continue;
            int si = town_y[a] * width + town_x[a];
            int ti = town_y[b] * width + town_x[b];
            int d = dijkstra(width, height, cells, si, ti, owner, inked);
            if (d <= 0 || d >= INF) continue;
            /* départage : distance, townId a, townId b */
            if (d < best_d || (d == best_d &&
                (a < best_a || (a == best_a && b < best_b)))) {
                best_d = d; best_a = a; best_b = b;
            }
        }

        memset(mine, 0, sizeof(mine));
        memset(foe, 0, sizeof(foe));
        memset(inst, 0, sizeof(inst));
        static unsigned char reg_active[MAXREG];
        memset(reg_active, 0, sizeof(reg_active));
        for (int i = 0; i < cells; ++i) {
            int rid = region_at[i];
            if (rid < 0 || rid >= MAXREG || town_region[rid] || inked[i]) continue;
            if (owner[i] == my_id) ++mine[rid];
            else if (owner[i] >= 0 && owner[i] != 2) ++foe[rid];
            if (instability[i] > inst[rid]) inst[rid] = instability[i];
            if (cell_active[i]) reg_active[rid] = 1;
        }
        int best_region = -1;
        int best_key[3] = {-1, -1, -1};
        for (int rid = 0; rid < MAXREG; ++rid) {
            if (foe[rid] <= mine[rid]) continue;
            int key[3] = {(int)reg_active[rid], foe[rid], inst[rid]};
            int better = 0, equal = 1;
            for (int j = 0; j < 3; ++j) {
                if (key[j] > best_key[j]) { better = 1; equal = 0; break; }
                if (key[j] < best_key[j]) { equal = 0; break; }
            }
            if (better || best_region < 0 || (equal && rid > best_region)) {
                best_key[0] = key[0]; best_key[1] = key[1]; best_key[2] = key[2];
                best_region = rid;
            }
        }

        /* Fin de partie : vol des chemins actifs par le tie-break N/E/S/O. */
        typedef struct { int n, foe, neg_terrain, y, x, idx; } Steal;
        static Steal steal[MAXCELL];
        int steal_n = 0;
        if (best_a < 0) {
            static unsigned char town_cell[MAXCELL];
            memset(town_cell, 0, sizeof(town_cell));
            for (int t = 0; t < MAXTID; ++t)
                if (town_seen[t]) town_cell[town_y[t] * width + town_x[t]] = 1;
            for (int i = 0; i < cells; ++i) {
                if (owner[i] != -1 || inked[i] || town_cell[i]) continue;
                if (best_region >= 0 && region_at[i] == best_region) continue;
                int x = i % width, y = i / width, n = 0, f = 0;
                const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
                for (int d = 0; d < 4; ++d) {
                    int nx = x + dx[d], ny = y + dy[d];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    int j = ny * width + nx;
                    if (!cell_active[j]) continue;
                    ++n;
                    if (owner[j] >= 0 && owner[j] != 2 && owner[j] != my_id) ++f;
                }
                if (n) {
                    Steal s = {n, f, -terrain[i], y, x, i};
                    steal[steal_n++] = s;
                }
            }
        }
        /* tri décroissant sur (n, foe, neg_terrain, y, x, idx) */
        for (int a = 1; a < steal_n; ++a) {
            Steal cur = steal[a];
            int b = a - 1;
            while (b >= 0) {
                int key_cur[6] = {cur.n, cur.foe, cur.neg_terrain, cur.y, cur.x, cur.idx};
                int key_prev[6] = {steal[b].n, steal[b].foe, steal[b].neg_terrain,
                                   steal[b].y, steal[b].x, steal[b].idx};
                int less = 0;
                for (int k = 0; k < 6; ++k) {
                    if (key_cur[k] > key_prev[k]) { less = 1; break; }
                    if (key_cur[k] < key_prev[k]) break;
                }
                if (!less) break;
                steal[b + 1] = steal[b];
                --b;
            }
            steal[b + 1] = cur;
        }

        int printed = 0;
        if (best_a >= 0) {
            printf("AUTOPLACE %d %d %d %d", town_x[best_a], town_y[best_a],
                   town_x[best_b], town_y[best_b]);
            printed = 1;
        }
        if (best_region >= 0) {
            if (printed) putchar(';');
            printf("DISRUPT %d", best_region);
            printed = 1;
        }
        for (int k = 0; k < steal_n && k < 3; ++k) {
            if (printed) putchar(';');
            printf("PLACE_TRACKS %d %d", steal[k].x, steal[k].y);
            printed = 1;
        }
        if (!printed) printf("WAIT");
        putchar('\n');
        fflush(stdout);
    }
    return 0;
}
