/* Elemental Wars (Wood) — BFS multi-source : poursuivre la proie, fuir le prédateur. */
#include <stdio.h>
#include <string.h>

#define MAXN 32
#define MAXU 64

static int dx4[4] = {-1, 0, 1, 0};
static int dy4[4] = {0, 1, 0, -1};
static char grid[MAXN][MAXN];
static int size_n;

static char prey_of(char t) { return t == 'W' ? 'F' : (t == 'F' ? 'P' : 'W'); }
static char pred_of(char t) { return t == 'W' ? 'P' : (t == 'F' ? 'W' : 'F'); }

/* BFS multi-source ; -1 = mur ou inaccessible. */
static void distances(int sx[], int sy[], int ns, int dist[MAXN][MAXN]) {
    int qx[MAXN * MAXN], qy[MAXN * MAXN], head = 0, tail = 0;
    int i, j;
    for (i = 0; i < size_n; i++)
        for (j = 0; j < size_n; j++) dist[i][j] = -1;
    for (i = 0; i < ns; i++) {
        if (sx[i] >= 0 && sx[i] < size_n && sy[i] >= 0 && sy[i] < size_n
            && dist[sy[i]][sx[i]] < 0) {
            dist[sy[i]][sx[i]] = 0;
            qx[tail] = sx[i]; qy[tail] = sy[i]; tail++;
        }
    }
    while (head < tail) {
        int x = qx[head], y = qy[head]; head++;
        for (j = 0; j < 4; j++) {
            int nx = x + dx4[j], ny = y + dy4[j];
            if (nx >= 0 && nx < size_n && ny >= 0 && ny < size_n
                && grid[ny][nx] != '#' && dist[ny][nx] < 0) {
                dist[ny][nx] = dist[y][x] + 1;
                qx[tail] = nx; qy[tail] = ny; tail++;
            }
        }
    }
}

int main(void) {
    char tribe[4];
    if (scanf("%3s", tribe) != 1) return 0;
    if (scanf("%d", &size_n) != 1) return 0;
    int y;
    for (y = 0; y < size_n; y++)
        if (scanf("%31s", grid[y]) != 1) return 0;

    for (;;) {
        int sw, sf, sp, count;
        if (scanf("%d %d %d", &sw, &sf, &sp) != 3) return 0;
        if (scanf("%d", &count) != 1) return 0;
        int my_id[MAXU], my_x[MAXU], my_y[MAXU], my_n = 0;
        int px[MAXU], py[MAXU], pn = 0, qx[MAXU], qy[MAXU], qn = 0;
        int i;
        for (i = 0; i < count; i++) {
            int id, x, yy, prisoner;
            char t[4];
            if (scanf("%d %3s %d %d %d", &id, t, &x, &yy, &prisoner) != 5) return 0;
            if (prisoner) continue;
            if (t[0] == tribe[0]) {
                my_id[my_n] = id; my_x[my_n] = x; my_y[my_n] = yy; my_n++;
            } else if (t[0] == prey_of(tribe[0])) {
                px[pn] = x; py[pn] = yy; pn++;
            } else if (t[0] == pred_of(tribe[0])) {
                qx[qn] = x; qy[qn] = yy; qn++;
            }
        }
        int dp[MAXN][MAXN], dd[MAXN][MAXN];
        distances(px, py, pn, dp);
        distances(qx, qy, qn, dd);
        int first = 1;
        for (i = 0; i < my_n; i++) {
            int bestScore = -1000000000, bestOrder = 0, bestX = my_x[i], bestY = my_y[i];
            int bx = my_x[i], by = my_y[i];
            int order;
            for (order = 0; order < 5; order++) {
                int ax = order == 0 ? 0 : dx4[order - 1];
                int ay = order == 0 ? 0 : dy4[order - 1];
                int nx = bx + ax, ny = by + ay;
                if (nx < 0 || nx >= size_n || ny < 0 || ny >= size_n) continue;
                if (order != 0 && grid[ny][nx] == '#') continue;
                int score = 0, k;
                for (k = 0; k < pn; k++)
                    if (px[k] == nx && py[k] == ny) score += 1000;
                if (dp[ny][nx] >= 0) score -= 2 * dp[ny][nx];
                if (dd[ny][nx] >= 0) {
                    score += 3 * dd[ny][nx];
                    if (dd[ny][nx] <= 1) score -= 50;
                }
                /* départage : score décroissant, puis ordre croissant, puis y,x décroissants */
                if (score > bestScore ||
                    (score == bestScore &&
                     (order < bestOrder ||
                      (order == bestOrder && (ny > bestY || (ny == bestY && nx > bestX)))))) {
                    bestScore = score; bestOrder = order; bestX = nx; bestY = ny;
                }
            }
            if (!first) printf(";");
            first = 0;
            if (bestOrder == 0)
                printf("%d WAIT", my_id[i]);
            else
                printf("%d MOVE %d %d", my_id[i], bestX, bestY);
        }
        if (first) printf("WAIT");
        printf("\n");
        fflush(stdout);
    }
    return 0;
}
