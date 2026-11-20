/* Xmas Rush — BFS sur les tuiles, poussée qui minimise la distance à la quête. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 7
#define CELLS (N * N)
#define MAXSTEP 20
#define NAMELEN 64

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};
static const char *DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

static int mask_of(const char *s) {
    int m = 0;
    for (int d = 0; d < 4; d++) if (s[d] == '1') m |= 1 << d;
    return m;
}

static int wrap(int v) { v %= N; return v < 0 ? v + N : v; }

/* Distance BFS ; -1 si aucun chemin. */
static int dist_bfs(const int g[N][N], int sx, int sy, int tx, int ty) {
    if (sx == tx && sy == ty) return 0;
    int prev[CELLS][3];
    for (int i = 0; i < CELLS; i++) prev[i][2] = -1;
    int q[CELLS], head = 0, tail = 0;
    int s = sy * N + sx;
    prev[s][0] = s; prev[s][2] = 0;
    q[tail++] = s;
    while (head < tail) {
        int cur = q[head++];
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; d++) {
            if (!(g[y][x] & (1 << d))) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!(g[ny][nx] & (1 << ((d + 2) % 4)))) continue;
            int ni = ny * N + nx;
            if (prev[ni][2] >= 0) continue;
            prev[ni][0] = cur; prev[ni][1] = d; prev[ni][2] = prev[cur][2] + 1;
            if (nx == tx && ny == ty) return prev[ni][2];
            q[tail++] = ni;
        }
    }
    return -1;
}

/* Chemin BFS (au plus MAXSTEP) ; -1 si aucun chemin. */
static int path_bfs(const int g[N][N], int sx, int sy, int tx, int ty, int out[MAXSTEP]) {
    if (sx == tx && sy == ty) return 0;
    int prev[CELLS][2];
    for (int i = 0; i < CELLS; i++) prev[i][1] = -1;
    int q[CELLS], head = 0, tail = 0;
    int s = sy * N + sx, t = ty * N + tx;
    prev[s][0] = s; prev[s][1] = -2;
    q[tail++] = s;
    while (head < tail) {
        int cur = q[head++];
        if (cur == t) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; d++) {
            if (!(g[y][x] & (1 << d))) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!(g[ny][nx] & (1 << ((d + 2) % 4)))) continue;
            int ni = ny * N + nx;
            if (prev[ni][1] >= 0) continue;
            prev[ni][0] = cur; prev[ni][1] = d;
            q[tail++] = ni;
        }
    }
    if (prev[t][1] == -1) return -1;
    int rev[MAXSTEP], n = 0, cur = t;
    while (cur != s && n < MAXSTEP) {
        rev[n++] = prev[cur][1];
        cur = prev[cur][0];
    }
    for (int i = 0; i < n; i++) out[i] = rev[n - 1 - i];
    return n;
}

static void simulate(const int in[N][N], int tile, int px, int py,
                     int ix, int iy, int line, int d,
                     int out[N][N], int *nxp, int *nyp, int *nix, int *niy) {
    for (int y = 0; y < N; y++) for (int x = 0; x < N; x++) out[y][x] = in[y][x];
    int insx, insy, popx, popy;
    if (d == 1) {
        insx = 0; insy = line; popx = N - 1; popy = line;
        for (int x = N - 1; x > 0; x--) out[x][line] = out[x - 1][line];
    } else if (d == 3) {
        insx = N - 1; insy = line; popx = 0; popy = line;
        for (int x = 0; x < N - 1; x++) out[x][line] = out[x + 1][line];
    } else if (d == 0) {
        insx = line; insy = N - 1; popx = line; popy = 0;
        for (int y = 0; y < N - 1; y++) out[line][y] = out[line][y + 1];
    } else {
        insx = line; insy = 0; popx = line; popy = N - 1;
        for (int y = N - 1; y > 0; y--) out[line][y] = out[line][y - 1];
    }
    out[insy][insx] = tile;
    *nix = ix; *niy = iy;
    if (ix < 0) { *nix = insx; *niy = insy; }
    else if (ix == popx && iy == popy) { *nix = -1; *niy = -1; }
    else if (((d == 1 || d == 3) && iy == line) ||
             ((d == 0 || d == 2) && ix == line)) {
        *nix = wrap(ix + DX[d]); *niy = wrap(iy + DY[d]);
    }
    *nxp = px; *nyp = py;
    if (((d == 1 || d == 3) && py == line) ||
        ((d == 0 || d == 2) && px == line)) {
        *nxp = wrap(px + DX[d]); *nyp = wrap(py + DY[d]);
    }
}

static char linebuf[512];
static char *next_line(void) {
    if (!fgets(linebuf, sizeof(linebuf), stdin)) return NULL;
    size_t n = strlen(linebuf);
    while (n && (linebuf[n - 1] == '\n' || linebuf[n - 1] == '\r')) linebuf[--n] = 0;
    return linebuf;
}

static void solve(int turn) {
    int g[N][N];
    for (int y = 0; y < N; y++) {
        char *p = next_line();
        if (!p) return;
        char save[512];
        strcpy(save, p);
        char *tok = strtok(save, " ");
        for (int x = 0; x < N; x++) {
            g[y][x] = mask_of(tok);
            tok = strtok(NULL, " ");
        }
    }
    int px = 0, py = 0, tile = 0;
    for (int i = 0; i < 2; i++) {
        char *p = next_line();
        if (!p) return;
        int cards, x, y;
        char t[8];
        if (sscanf(p, "%d %d %d %7s", &cards, &x, &y, t) != 4) return;
        if (i == 0) { px = x; py = y; tile = mask_of(t); }
    }

    int nitems = atoi(next_line());
    char myitem[NAMELEN] = "";
    int ix = -1, iy = -1;
    for (int i = 0; i < nitems; i++) {
        char *p = next_line();
        if (!p) return;
        char name[NAMELEN];
        int x, y, pid;
        if (sscanf(p, "%63s %d %d %d", name, &x, &y, &pid) != 4) return;
        if (pid == 0) {
            strcpy(myitem, name);
            ix = x; iy = y;
        }
    }
    int nquests = atoi(next_line());
    bool found = false;
    for (int i = 0; i < nquests; i++) {
        char *p = next_line();
        if (!p) return;
        char name[NAMELEN];
        int pid;
        if (sscanf(p, "%63s %d", name, &pid) != 2) return;
        if (pid == 0 && strcmp(name, myitem) == 0) found = true;
    }
    if (!found) { ix = -1; iy = -1; }
    if (ix < 0) { ix = -1; iy = -1; }

    if (turn == 1) {
        if (ix < 0) { puts("PASS"); return; }
        int path[MAXSTEP];
        int n = path_bfs(g, px, py, ix, iy, path);
        if (n <= 0) { puts("PASS"); return; }
        printf("MOVE");
        for (int i = 0; i < n && i < MAXSTEP; i++) printf(" %s", DIRS[path[i]]);
        putchar('\n');
        return;
    }

    int best = 0x7fffffff, bestline = 0, bestd = 0;
    for (int horizontal = 1; horizontal >= 0; horizontal--) {
        for (int line = 0; line < N; line++) {
            int dirs[2] = {horizontal ? 1 : 0, horizontal ? 3 : 2};
            for (int k = 0; k < 2; k++) {
                int d = dirs[k];
                int ng[N][N], nx, ny, nix, niy;
                simulate(g, tile, px, py, ix, iy, line, d, ng, &nx, &ny, &nix, &niy);
                int dist;
                if (nix < 0) dist = 1000;
                else if (nix == nx && niy == ny) dist = -1;
                else {
                    int dd = dist_bfs(ng, nx, ny, nix, niy);
                    dist = dd < 0 ? 100 : dd;
                }
                if (dist < best || (dist == best &&
                    (line < bestline || (line == bestline && d < bestd)))) {
                    best = dist; bestline = line; bestd = d;
                }
            }
        }
    }
    printf("PUSH %d %s\n", bestline, DIRS[bestd]);
}

int main(void) {
    char *first;
    while ((first = next_line()) != NULL) {
        if (!*first) continue;
        solve(atoi(first));
        fflush(stdout);
    }
    return 0;
}
