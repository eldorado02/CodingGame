/* Spring Challenge 2020 (Wood 4) — BFS + meilleur pellet, évitement collision. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXC 64
#define MAXR 64

static int W, H;
static char grid[MAXR][MAXC];
static int dista[MAXR][MAXC];
static int prevx[MAXR][MAXC], prevy[MAXR][MAXC];
static int target_x = -1, target_y = -1;
static char line[8192];

static char *rd(void) {
    if (!fgets(line, sizeof(line), stdin)) return NULL;
    size_t n = strlen(line);
    while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = 0;
    return line;
}

static int wrapx(int x) {
    x %= W;
    if (x < 0) x += W;
    return x;
}

static void bfs(int sx, int sy) {
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            dista[y][x] = -1;
            prevx[y][x] = prevy[y][x] = -1;
        }
    int qx[MAXR * MAXC], qy[MAXR * MAXC], head = 0, tail = 0;
    dista[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; tail++;
    static const int DX[4] = {0, 1, 0, -1};
    static const int DY[4] = {-1, 0, 1, 0};
    while (head < tail) {
        int x = qx[head], y = qy[head]; head++;
        for (int k = 0; k < 4; k++) {
            int nx = wrapx(x + DX[k]), ny = y + DY[k];
            if (nx < 0 || ny < 0 || ny >= H) continue;
            if (grid[ny][nx] == '#' || dista[ny][nx] != -1) continue;
            dista[ny][nx] = dista[y][x] + 1;
            prevx[ny][nx] = x; prevy[ny][nx] = y;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
}

static int first_step(int sx, int sy, int tx, int ty, int *out_x, int *out_y) {
    if (sx == tx && sy == ty) return 0;
    int x = tx, y = ty, guard = 0;
    while (!(x == sx && y == sy)) {
        int px = prevx[y][x], py = prevy[y][x];
        if (px < 0 || ++guard > W * H) return 0;
        if (px == sx && py == sy) {
            *out_x = x; *out_y = y;
            return 1;
        }
        x = px; y = py;
    }
    return 0;
}

typedef struct {
    int penalty, cost, negval, d, tx, ty;
} Key;

static int better(const Key *a, const Key *b) {
    if (b->tx < 0) return 1;
    if (a->penalty != b->penalty) return a->penalty < b->penalty;
    if (a->cost != b->cost) return a->cost < b->cost;
    if (a->negval != b->negval) return a->negval < b->negval;
    if (a->d != b->d) return a->d < b->d;
    if (a->tx != b->tx) return a->tx < b->tx;
    return a->ty < b->ty;
}

int main(void) {
    char *p = rd();
    if (!p) return 0;
    if (sscanf(p, "%d %d", &W, &H) != 2) return 0;
    for (int y = 0; y < H; y++) {
        char *row = rd();
        if (!row) return 0;
        strncpy(grid[y], row, MAXC - 1);
        grid[y][MAXC - 1] = 0;
        for (int x = strlen(grid[y]); x < W; x++) grid[y][x] = ' ';
    }

    while ((p = rd()) != NULL) {
        if (!*p) break;
        int mys, ops;
        if (sscanf(p, "%d %d", &mys, &ops) != 2) break;
        int np = atoi(rd());
        int px = -1, py = -1, ox = -1, oy = -1;
        for (int i = 0; i < np; i++) {
            char pacline[8192];
            strcpy(pacline, rd());
            int id, mine, x, y;
            sscanf(pacline, "%d %d %d %d", &id, &mine, &x, &y);
            if (mine) { px = x; py = y; }
            else if (ox < 0 || id == 0) { ox = x; oy = y; }
        }
        int npel = atoi(rd());
        int pellets[MAXR * MAXC][3];
        int n = 0;
        for (int i = 0; i < npel && n < MAXR * MAXC; i++) {
            char pl[8192];
            strcpy(pl, rd());
            int x, y, v;
            if (sscanf(pl, "%d %d %d", &x, &y, &v) == 3) {
                pellets[n][0] = x; pellets[n][1] = y; pellets[n][2] = v; n++;
            }
        }
        if (px < 0) { printf("MOVE 0 0 0\n"); fflush(stdout); continue; }

        bfs(px, py);
        bool target_ok = false;
        for (int i = 0; i < n; i++)
            if (pellets[i][0] == target_x && pellets[i][1] == target_y &&
                dista[target_y][target_x] >= 0) target_ok = true;
        if (target_x == px && target_y == py) target_ok = false;
        if (target_ok) {
            printf("MOVE 0 %d %d\n", target_x, target_y);
            fflush(stdout);
            continue;
        }
        if (n == 0) { printf("MOVE 0 %d %d\n", px, py); fflush(stdout); continue; }

        int osx = -1, osy = -1;
        if (ox >= 0 && oy >= 0 && ox < W && oy < H) {
            int od[MAXR][MAXC], opx[MAXR][MAXC], opy[MAXR][MAXC];
            /* bfs() écrit dans les tableaux globaux : sauvegarder les nôtres. */
            int sd[MAXR][MAXC], sx_[MAXR][MAXC], sy_[MAXR][MAXC];
            memcpy(sd, dista, sizeof(sd));
            memcpy(sx_, prevx, sizeof(sx_));
            memcpy(sy_, prevy, sizeof(sy_));
            (void)od; (void)opx; (void)opy;
            bfs(ox, oy);
            Key bo = {0, 0, 0, 0, -1, -1};
            for (int i = 0; i < n; i++) {
                int tx = pellets[i][0], ty = pellets[i][1], v = pellets[i][2];
                if (tx < 0 || tx >= W || ty < 0 || ty >= H || dista[ty][tx] < 0) continue;
                int d = dista[ty][tx];
                Key k = {0, d, -v, d, tx, ty};
                if (better(&k, &bo)) bo = k;
            }
            if (bo.tx >= 0 && first_step(ox, oy, bo.tx, bo.ty, &osx, &osy)) {
                /* osx/osy = premier pas adverse */
            } else { osx = osy = -1; }
            memcpy(dista, sd, sizeof(dista));
            memcpy(prevx, sx_, sizeof(prevx));
            memcpy(prevy, sy_, sizeof(prevy));
        }

        Key best = {0, 0, 0, 0, -1, -1};
        for (int i = 0; i < n; i++) {
            int tx = pellets[i][0], ty = pellets[i][1], v = pellets[i][2];
            if (tx < 0 || tx >= W || ty < 0 || ty >= H || dista[ty][tx] < 0) continue;
            int sx1, sy1;
            if (!first_step(px, py, tx, ty, &sx1, &sy1)) continue;
            int penalty = (osx >= 0 && sx1 == osx && sy1 == osy) ? 1 : 0;
            int d = dista[ty][tx];
            Key k = {penalty, d * 10 / v, -v, d, tx, ty};
            if (better(&k, &best)) best = k;
        }
        if (best.tx < 0) { printf("MOVE 0 %d %d\n", px, py); fflush(stdout); continue; }
        target_x = best.tx; target_y = best.ty;
        printf("MOVE 0 %d %d\n", target_x, target_y);
        fflush(stdout);
    }
    return 0;
}
