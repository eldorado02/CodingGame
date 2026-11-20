/* Ghost in the Cell Wood 3 — forces effectives + meilleure action globale. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAXN 15
#define INF 1000000
#define RESERVE 5

static int dist_mat[MAXN][MAXN];

int main(void) {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) dist_mat[i][j] = (i == j) ? 0 : INF;
    for (int i = 0; i < m; i++) {
        int a, b, d;
        if (scanf("%d %d %d", &a, &b, &d) != 3) return 0;
        dist_mat[a][b] = dist_mat[b][a] = d;
    }
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist_mat[i][k] + dist_mat[k][j] < dist_mat[i][j])
                    dist_mat[i][j] = dist_mat[i][k] + dist_mat[k][j];

    for (;;) {
        int count;
        if (scanf("%d", &count) != 1) return 0;
        int owner[MAXN] = {0}, cyborgs[MAXN] = {0}, prod[MAXN] = {0};
        int t_owner[MAXN * 4], t_src[MAXN * 4], t_dst[MAXN * 4], t_count[MAXN * 4], nt = 0;
        for (int i = 0; i < count; i++) {
            int id;
            char kind[16];
            int a, b, c, d, e;
            if (scanf("%d %15s %d %d %d %d %d", &id, kind, &a, &b, &c, &d, &e) != 7) return 0;
            if (strcmp(kind, "FACTORY") == 0) {
                owner[id] = a; cyborgs[id] = b; prod[id] = c;
            } else if (strcmp(kind, "TROOP") == 0) {
                t_owner[nt] = a; t_src[nt] = b; t_dst[nt] = c; t_count[nt] = d; nt++;
            }
        }
        for (int i = 0; i < nt; i++) cyborgs[t_dst[i]] += (t_owner[i] == 1) ? t_count[i] : -t_count[i];
        (void)t_src;
        for (int i = 0; i < n; i++) if (owner[i] == 0) cyborgs[i] = 0;

        int best_score = 0, best_src = -1, best_dst = -1, best_count = 0;
        for (int i = 0; i < n; i++) {
            if (owner[i] != 1 || cyborgs[i] >= 0) continue;
            for (int j = 0; j < n; j++) {
                if (owner[j] != 1 || cyborgs[j] <= -cyborgs[i] + RESERVE) continue;
                int cnt = cyborgs[j] - RESERVE;
                if (cnt > -cyborgs[i] + 2) cnt = -cyborgs[i] + 2;
                int score = 1000 - dist_mat[j][i];
                if (score > best_score) {
                    best_score = score; best_src = j; best_dst = i; best_count = cnt;
                }
            }
        }
        if (best_src < 0) {
            for (int src = 0; src < n; src++) {
                if (owner[src] != 1 || cyborgs[src] <= RESERVE) continue;
                for (int dst = 0; dst < n; dst++) {
                    if (owner[dst] == 1 || dist_mat[src][dst] >= INF) continue;
                    int need = cyborgs[dst] + 1;
                    if (need <= 0 || cyborgs[src] - RESERVE < need) continue;
                    int score = (owner[dst] == 0 ? 100 : 0) + prod[dst] * 20
                                - dist_mat[src][dst] * 2 - need;
                    if (score > best_score) {
                        best_score = score; best_src = src; best_dst = dst; best_count = need;
                    }
                }
            }
        }
        if (best_src < 0) printf("WAIT\n");
        else printf("MOVE %d %d %d\n", best_src, best_dst, best_count);
        fflush(stdout);
    }
}
