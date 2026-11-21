/* Tron Battle — Voronoi 1-ply + greedy primé. Même logique que Python. */
#include <stdio.h>
#include <string.h>

#define W 30
#define H 20
#define MAXP 4
#define INF 99

static const char *const DIR_NAME[4] = {"UP", "DOWN", "LEFT", "RIGHT"};
static const int DX[4] = {0, 0, -1, 1};
static const int DY[4] = {-1, 1, 0, 0};

typedef struct { int x, y; } Cell;

/* Voisins précalculés */
static int neigh[H][W][4];
static int neigh_n[H][W];

static void init_neigh(void) {
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        int n = 0;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                neigh[y][x][n++] = nx + ny * W;
            }
        }
        neigh_n[y][x] = n;
    }
}

/* BFS depuis une tête, dans un plateau blocké. dist[y][x] = distance. */
static void bfs(int sx, int sy, const unsigned char blocked[H][W], int dist[H][W]) {
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) dist[y][x] = INF;
    if (sx < 0) return; /* la tête est occupée mais reste origine légitime */
    int q[H * W], qx = 0, qn = 0;
    q[qn++] = sx + sy * W;
    dist[sy][sx] = 0;
    while (qx < qn) {
        int cell = q[qx++];
        int x = cell % W, y = cell / W;
        int nd = dist[y][x] + 1;
        for (int i = 0; i < neigh_n[y][x]; ++i) {
            int nx = neigh[y][x][i] % W, ny = neigh[y][x][i] / W;
            if (!blocked[ny][nx] && dist[ny][nx] == INF) {
                dist[ny][nx] = nd;
                q[qn++] = neigh[y][x][i];
            }
        }
    }
}

static int greedy_fill(int sx, int sy, unsigned char blocked[H][W], int limit) {
    unsigned char local[H][W];
    memcpy(local, blocked, sizeof local);
    int x = sx, y = sy, count = 0;
    while (count < limit) {
        int best = -1, best_free = 999, best_nx = 0, best_ny = 0;
        for (int i = 0; i < neigh_n[y][x]; ++i) {
            int nx = neigh[y][x][i] % W, ny = neigh[y][x][i] / W;
            if (local[ny][nx]) continue;
            int free_next = 0;
            for (int j = 0; j < neigh_n[ny][nx]; ++j) {
                int ax = neigh[ny][nx][j] % W, ay = neigh[ny][nx][j] / W;
                if (!local[ay][ax]) free_next++;
            }
            /* départage : free_next croissant, puis -ny, puis -nx */
            if (free_next < best_free ||
                (free_next == best_free && (ny > best_ny ||
                (ny == best_ny && nx > best_nx)))) {
                best_free = free_next; best_nx = nx; best_ny = ny; best = 1;
            }
        }
        if (best < 0) break;
        x = best_nx; y = best_ny; local[y][x] = 1; count++;
    }
    return count;
}

int main(void) {
    init_neigh();
    int n, p;
    if (scanf("%d %d", &n, &p) != 2) return 0;
    unsigned char blocked[H][W];
    memset(blocked, 0, sizeof blocked);
    int heads[MAXP][2];
    unsigned char seen[MAXP][H][W];
    int initialized = 0;

    while (1) {
        if (!initialized) {
            /* première ligne déjà lue */
        } else if (scanf("%d %d", &n, &p) != 2) {
            return 0;
        }
        for (int i = 0; i < n; ++i) {
            int x0, y0, x1, y1;
            if (scanf("%d %d %d %d", &x0, &y0, &x1, &y1) != 4) return 0;
            heads[i][0] = x1; heads[i][1] = y1;
        }
        if (!initialized) {
            for (int i = 0; i < n; ++i) memset(seen[i], 0, sizeof seen[i]);
            initialized = 1;
        }
        /* reconstruit les bandes (les têtes sont mémorisées dans seen) */
        memset(blocked, 0, sizeof blocked);
        for (int i = 0; i < n; ++i) {
            if (heads[i][0] < 0) { memset(seen[i], 0, sizeof seen[i]); continue; }
            seen[i][heads[i][1]][heads[i][0]] = 1;
            for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
                if (seen[i][y][x]) blocked[y][x] = 1;
        }
        int me = heads[p][0], my = heads[p][1];
        if (me < 0) { printf("UP\n"); fflush(stdout); continue; }

        unsigned char trial[H][W];
        int best_score = 0, best_dir = 0, best_ny = H, best_nx = W, have = 0;
        int dm[H][W], ds[MAXP][H][W];
        for (int d = 0; d < 4; ++d) {
            int nx = me + DX[d], ny = my + DY[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H || blocked[ny][nx]) continue;
            memcpy(trial, blocked, sizeof trial);
            trial[my][me] = 1; trial[ny][nx] = 1;
            bfs(nx, ny, trial, dm);
            int nf = 0;
            for (int i = 0; i < n; ++i)
                if (i != p && heads[i][0] >= 0) bfs(heads[i][0], heads[i][1], trial, ds[nf++]);
            int voronoi = 0, reach = 0;
            for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
                if (trial[y][x] || dm[y][x] == INF) continue;
                reach++;
                int b = INF;
                for (int k = 0; k < nf; ++k) if (ds[k][y][x] < b) b = ds[k][y][x];
                if (dm[y][x] < b) voronoi++;
                else if (dm[y][x] > b) voronoi--;
            }
            int mob = 0;
            for (int i = 0; i < neigh_n[ny][nx]; ++i) {
                int ax = neigh[ny][nx][i] % W, ay = neigh[ny][nx][i] / W;
                if (!trial[ay][ax]) mob++;
            }
            int fill = greedy_fill(nx, ny, trial, 600);
            int score = 100000 * voronoi + 100 * reach + 10 * mob + fill;
            /* départage identique à Python : (score, -ny, -nx, direction) */
            if (!have || score > best_score ||
                (score == best_score && ny < best_ny) ||
                (score == best_score && ny == best_ny && nx < best_nx) ||
                (score == best_score && ny == best_ny && nx == best_nx && d < best_dir)) {
                best_score = score; best_dir = d; best_ny = ny; best_nx = nx; have = 1;
            }
        }
        printf("%s\n", !have ? "UP" : DIR_NAME[best_dir]);
        fflush(stdout);
    }
    return 0;
}
