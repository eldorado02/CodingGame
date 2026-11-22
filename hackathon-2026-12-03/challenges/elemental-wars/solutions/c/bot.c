/* Elemental Wars — fuir le prédateur, poursuivre la proie. */
#include <stdio.h>
#include <string.h>

#define NMAX 16
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

static char prey_of(char t) {
    if (t == 'W') return 'F';
    if (t == 'F') return 'P';
    return 'W';
}
static char pred_of(char t) {
    if (t == 'W') return 'P';
    if (t == 'F') return 'W';
    return 'F';
}

static void bfs(char grid[NMAX][NMAX], int n, int sx, int sy, int dist[NMAX][NMAX]) {
    int qx[256], qy[256], head = 0, tail = 0;
    for (int y = 0; y < n; ++y) for (int x = 0; x < n; ++x) dist[y][x] = -1;
    if (sx < 0 || sx >= n || sy < 0 || sy >= n || grid[sy][sx] == '#') return;
    dist[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; ++tail;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (grid[ny][nx] == '#' || dist[ny][nx] >= 0) continue;
            dist[ny][nx] = dist[y][x] + 1;
            qx[tail] = nx; qy[tail] = ny; ++tail;
        }
    }
}

int main(void) {
    char tribe[4];
    if (scanf("%3s", tribe) != 1) return 0;
    int n;
    if (scanf("%d", &n) != 1) return 0;
    char grid[NMAX][NMAX];
    for (int y = 0; y < n; ++y) {
        char row[NMAX];
        scanf("%63s", row);
        for (int x = 0; x < n; ++x) grid[y][x] = row[x];
    }
    int sw, sf, sp;
    while (scanf("%d %d %d", &sw, &sf, &sp) == 3) {
        int count;
        if (scanf("%d", &count) != 1) break;
        int uid[8]; char ut[8]; int ux[8], uy[8], up[8];
        int my = -1, py = -1, qy = -1;
        for (int i = 0; i < count; ++i) {
            scanf("%d %c %d %d %d", &uid[i], &ut[i], &ux[i], &uy[i], &up[i]);
            if (ut[i] == tribe[0]) my = i;
            if (ut[i] == prey_of(tribe[0])) py = i;
            if (ut[i] == pred_of(tribe[0])) qy = i;
        }
        if (my < 0 || py < 0) { printf("0 WAIT\n"); fflush(stdout); continue; }
        if (up[my] || up[py]) {
            printf("%d WAIT\n", uid[my]); fflush(stdout); continue;
        }
        int px = py >= 0 ? ux[py] : 100, pyy = py >= 0 ? uy[py] : 100;
        int qx = qy >= 0 && !up[qy] ? ux[qy] : 100;
        int qyy = qy >= 0 && !up[qy] ? uy[qy] : 100;
        int dprey[NMAX][NMAX], dpred[NMAX][NMAX];
        bfs(grid, n, px, pyy, dprey);
        bfs(grid, n, qx, qyy, dpred);
        int bestx = ux[my], besty = uy[my], bestScore = -1000000000;
        for (int k = -1; k < 4; ++k) {
            int x = ux[my], y = uy[my];
            if (k >= 0) { x += DX[k]; y += DY[k]; }
            if (x < 0 || x >= n || y < 0 || y >= n || grid[y][x] == '#') continue;
            int dp = dprey[y][x] >= 0 ? dprey[y][x] : 100;
            int dq = dpred[y][x] >= 0 ? dpred[y][x] : 100;
            int score = -100 * dp + 20 * (dq < 6 ? dq : 6);
            if (x == px && y == pyy && !(x == qx && y == qyy)) score += 10000;
            if (x == qx && y == qyy) score -= 10000;
            if (dq <= 1 && dp > 1) score -= 2000;
            if (score > bestScore ||
                (score == bestScore && (x < bestx || (x == bestx && y < besty)))) {
                bestScore = score; bestx = x; besty = y;
            }
        }
        printf("%d MOVE %d %d\n", uid[my], bestx, besty);
        fflush(stdout);
    }
    return 0;
}
