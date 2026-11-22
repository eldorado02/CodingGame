/* Elemental Wars — Wood 2+: commande toutes les elementaux, fuit le predateur,
   poursuit la proie. Distances BFS precalculees par ennemi. */
#include <stdio.h>
#include <string.h>

#define NMAX 16
#define EMAX 32
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

typedef char Grid[NMAX][NMAX];
typedef int Dist[NMAX][NMAX];

static char prey_of(char t) {
    return t == 'W' ? 'F' : (t == 'F' ? 'P' : 'W');
}
static char pred_of(char t) {
    return t == 'W' ? 'P' : (t == 'F' ? 'W' : 'F');
}

static void bfs(const Grid g, int n, int sx, int sy, Dist d) {
    int qx[NMAX * NMAX], qy[NMAX * NMAX], head = 0, tail = 0;
    for (int y = 0; y < NMAX; ++y)
        for (int x = 0; x < NMAX; ++x)
            d[y][x] = -1;
    if (sx < 0 || sx >= n || sy < 0 || sy >= n || g[sy][sx] == '#') return;
    d[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; ++tail;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        for (int k = 0; k < 4; ++k) {
            int nx = x + DX[k], ny = y + DY[k];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (g[ny][nx] == '#' || d[ny][nx] >= 0) continue;
            d[ny][nx] = d[y][x] + 1;
            qx[tail] = nx; qy[tail] = ny; ++tail;
        }
    }
}

struct El { int id, x, y, prisoner; char tribe; };

static Dist prey_dist[EMAX];
static Dist pred_dist[EMAX];

int main(void) {
    char me;
    int n;
    if (scanf(" %c %d", &me, &n) != 2) return 0;

    Grid grid;
    for (int y = 0; y < NMAX; ++y)
        for (int x = 0; x < NMAX; ++x)
            grid[y][x] = '.';
    char row[256];
    for (int y = 0; y < n; ++y) {
        if (scanf("%255s", row) != 1) return 0;
        size_t len = strlen(row);
        for (int x = 0; x < n; ++x)
            grid[y][x] = x < (int)len ? row[x] : '.';
    }

    int sw, sf, sp;
    while (scanf("%d %d %d", &sw, &sf, &sp) == 3) {
        int count;
        if (scanf("%d", &count) != 1) break;
        struct El els[EMAX];
        int lim = count < EMAX ? count : EMAX;
        for (int i = 0; i < lim; ++i) {
            if (scanf("%d %c %d %d %d", &els[i].id, &els[i].tribe,
                      &els[i].x, &els[i].y, &els[i].prisoner) != 5) return 0;
        }

        char prey = prey_of(me), pred = pred_of(me);
        int nprey = 0, npred = 0;
        for (int i = 0; i < lim; ++i) {
            const struct El *e = &els[i];
            if (e->prisoner || e->tribe == me) continue;
            Dist tmp;
            bfs(grid, n, e->x, e->y, tmp);
            if (e->tribe == prey) {
                memcpy(prey_dist[nprey], tmp, sizeof(tmp));
                ++nprey;
            } else if (e->tribe == pred) {
                memcpy(pred_dist[npred], tmp, sizeof(tmp));
                ++npred;
            }
        }

        int any = 0;
        for (int i = 0; i < lim; ++i) {
            const struct El *e = &els[i];
            if (e->tribe != me || e->prisoner) continue;
            int bx = e->x, by = e->y, best = -1000000000;
            for (int k = -1; k < 4; ++k) {
                int x = e->x, y = e->y;
                if (k >= 0) { x += DX[k]; y += DY[k]; }
                if (x < 0 || x >= n || y < 0 || y >= n || grid[y][x] == '#') continue;
                int dp = 100, dq = 100;
                for (int j = 0; j < nprey; ++j) {
                    int v = prey_dist[j][y][x];
                    if (v >= 0 && v < dp) dp = v;
                }
                for (int j = 0; j < npred; ++j) {
                    int v = pred_dist[j][y][x];
                    if (v >= 0 && v < dq) dq = v;
                }
                int score = -100 * (dp < 20 ? dp : 20) + 15 * (dq < 8 ? dq : 8);
                if (dp == 0) score += 10000;
                if (dq == 0) score -= 12000;
                if (dq <= 1 && dp > 1) score -= 2000;
                if (score > best || (score == best &&
                    (x < bx || (x == bx && y < by)))) {
                    best = score; bx = x; by = y;
                }
            }
            if (any) putchar(';');
            any = 1;
            if (bx == e->x && by == e->y)
                printf("%d WAIT", e->id);
            else
                printf("%d MOVE %d %d", e->id, bx, by);
        }
        if (!any) fputs("MSG", stdout);
        putchar('\n');
        fflush(stdout);
    }
    return 0;
}
