/* Elemental Wars — Wood 1+: predicted opponent moves + intercept + evade. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 16
#define EMAX 256

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

typedef char Grid[NMAX][NMAX];
typedef int Dist[NMAX][NMAX];

struct El { int id, x, y, prisoner; char tribe; };
struct Cell { int x, y; };

static Dist prey_dist[EMAX];
static Dist pred_dist[EMAX];

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
        int len = (int)strlen(row);
        for (int x = 0; x < n; ++x)
            grid[y][x] = x < len ? row[x] : '.';
    }

    char prey = prey_of(me), pred = pred_of(me);
    int campX = -1, campY = -1;
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x)
            if (grid[y][x] == pred) { campX = x; campY = y; }

    int sw, sf, sp;
    while (scanf("%d %d %d", &sw, &sf, &sp) == 3) {
        int count;
        if (scanf("%d", &count) != 1) break;
        struct El els[EMAX];
        int lim = count < EMAX ? count : EMAX;
        for (int i = 0; i < lim; ++i)
            if (scanf("%d %c %d %d %d", &els[i].id, &els[i].tribe,
                      &els[i].x, &els[i].y, &els[i].prisoner) != 5) return 0;

        int prisoners = 0, nmine = 0;
        int mine[EMAX];
        for (int i = 0; i < lim; ++i) {
            if (els[i].tribe != me) continue;
            if (els[i].prisoner) ++prisoners;
            else mine[nmine++] = i;
        }

        /* Distances from each free enemy elemental. */
        int nprey = 0, npred = 0;
        for (int i = 0; i < lim; ++i) {
            const struct El *e = &els[i];
            if (e->prisoner || e->tribe == me) continue;
            Dist tmp;
            bfs(grid, n, e->x, e->y, tmp);
            if (e->tribe == prey) {
                for (int y = 0; y < NMAX; ++y)
                    for (int x = 0; x < NMAX; ++x)
                        prey_dist[nprey][y][x] = tmp[y][x];
                ++nprey;
            } else if (e->tribe == pred) {
                for (int y = 0; y < NMAX; ++y)
                    for (int x = 0; x < NMAX; ++x)
                        pred_dist[npred][y][x] = tmp[y][x];
                ++npred;
            }
        }

        /* Predict each enemy's next cell: step toward its nearest free prey. */
        struct Cell prey_next[EMAX], pred_next[EMAX];
        int nprey_next = 0, npred_next = 0;
        for (int i = 0; i < lim; ++i) {
            const struct El *e = &els[i];
            if (e->prisoner || e->tribe == me) continue;
            char et = prey_of(e->tribe);
            int tx = -1, ty = -1, best = 1 << 28;
            for (int j = 0; j < lim; ++j) {
                const struct El *q = &els[j];
                if (q->prisoner || q->tribe != et) continue;
                int d = abs(q->x - e->x) + abs(q->y - e->y);
                if (d < best) { best = d; tx = q->x; ty = q->y; }
            }
            int nx = e->x, ny = e->y;
            if (tx >= 0) {
                Dist dt;
                bfs(grid, n, tx, ty, dt);
                int cur = dt[e->y][e->x];
                if (cur > 0) {
                    for (int k = 0; k < 4; ++k) {
                        int ax = e->x + DX[k], ay = e->y + DY[k];
                        if (ax < 0 || ax >= n || ay < 0 || ay >= n) continue;
                        if (grid[ay][ax] == '#' || dt[ay][ax] != cur - 1) continue;
                        nx = ax; ny = ay; break;
                    }
                }
            }
            if (e->tribe == prey) {
                prey_next[nprey_next].x = nx;
                prey_next[nprey_next].y = ny;
                ++nprey_next;
            } else {
                pred_next[npred_next].x = nx;
                pred_next[npred_next].y = ny;
                ++npred_next;
            }
        }

        Dist dcamp;
        int haveCamp = (prisoners > 0 && campX >= 0);
        if (haveCamp) bfs(grid, n, campX, campY, dcamp);
        int liberator = -1;
        if (haveCamp) {
            int best = 1 << 28;
            for (int mi = 0; mi < nmine; ++mi) {
                int i = mine[mi];
                int d = dcamp[els[i].y][els[i].x];
                if (d >= 0 && d < best) { best = d; liberator = i; }
            }
            if (best > 14) liberator = -1;
        }

        struct Cell assigned[EMAX];
        int nassigned = 0;
        for (int mi = 0; mi < nmine; ++mi) {
            int idx = mine[mi];
            const struct El *e = &els[idx];
            int lib = (idx == liberator);
            int bx = e->x, by = e->y;
            long long bestScore = -((long long)1 << 50);
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
                long long score;
                if (lib) {
                    int dc = dcamp[y][x] >= 0 ? dcamp[y][x] : 100;
                    score = -100LL * (dc < 20 ? dc : 20) + 20LL * (dq < 8 ? dq : 8);
                    if (dc == 0) score += 3000LL * prisoners;
                    if (dq == 0) score -= 20000;
                    else if (dq == 1) score -= 5000;
                } else {
                    score = -100LL * (dp < 20 ? dp : 20) + 20LL * (dq < 8 ? dq : 8);
                    for (int j = 0; j < nprey_next; ++j)
                        if (prey_next[j].x == x && prey_next[j].y == y) score += 12000;
                    for (int j = 0; j < npred_next; ++j)
                        if (pred_next[j].x == x && pred_next[j].y == y) score -= 18000;
                    if (dq == 0) score -= 20000;
                    else if (dq == 1) score -= 4000;
                }
                for (int j = 0; j < nassigned; ++j)
                    if (assigned[j].x == x && assigned[j].y == y) score -= 5000;
                if (score > bestScore || (score == bestScore &&
                    (x < bx || (x == bx && y < by)))) {
                    bestScore = score; bx = x; by = y;
                }
            }
            assigned[nassigned].x = bx;
            assigned[nassigned].y = by;
            ++nassigned;
            if (mi > 0) putchar(';');
            if (bx == e->x && by == e->y)
                printf("%d WAIT", e->id);
            else
                printf("%d MOVE %d %d", e->id, bx, by);
        }
        if (nmine == 0) fputs("MSG", stdout);
        putchar('\n');
        fflush(stdout);
    }
    return 0;
}
