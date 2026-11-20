/* Hypersonic Wood — BFS vers les boîtes, bombes sur cibles rentables. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define W 13
#define H 11
#define RANGE 3

static const int DX[4] = {1, -1, 0, 0};
static const int DY[4] = {0, 0, 1, -1};

typedef struct { int x, y; } Point;

static int blast_boxes(const char grid[H][W], int x, int y) {
    int total = 0;
    for (int d = 0; d < 4; d++) {
        for (int s = 1; s <= RANGE; s++) {
            int nx = x + DX[d] * s, ny = y + DY[d] * s;
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) break;
            if (grid[ny][nx] == '0') { total++; break; }
        }
    }
    return total;
}

/* Premier pas BFS. mode 0=boite, 1=centre, 2=fuite hors portee 3. */
static bool bfs_next(const char grid[H][W], int sx, int sy,
                     const bool blocked[H][W], int mode, Point *out) {
    Point q[H * W];
    int parent[H * W];
    bool seen[H][W] = {false};
    int head = 0, tail = 0;
    q[tail++] = (Point){sx, sy};
    seen[sy][sx] = true;
    for (int i = 0; i < H * W; i++) parent[i] = -1;

    while (head < tail) {
        Point cur = q[head++];
        for (int d = 0; d < 4; d++) {
            int nx = cur.x + DX[d], ny = cur.y + DY[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (seen[ny][nx] || blocked[ny][nx] || grid[ny][nx] == '0') continue;
            seen[ny][nx] = true;
            parent[ny * W + nx] = cur.y * W + cur.x;
            bool done = false;
            if (mode == 0) {
                for (int k = 0; k < 4; k++) {
                    int ax = nx + DX[k], ay = ny + DY[k];
                    if (ax >= 0 && ax < W && ay >= 0 && ay < H && grid[ay][ax] == '0') {
                        done = true;
                        break;
                    }
                }
            } else if (mode == 1) {
                done = nx == W / 2 && ny == H / 2;
            } else {
                int dist = (nx > sx ? nx - sx : sx - nx) + (ny > sy ? ny - sy : sy - ny);
                done = dist > 3;
            }
            if (done) {
                int idx = ny * W + nx;
                while (parent[idx] != sy * W + sx && parent[idx] != -1) idx = parent[idx];
                *out = (Point){idx % W, idx / W};
                return true;
            }
            q[tail++] = (Point){nx, ny};
        }
    }
    return false;
}

int main(void) {
    int width, height, my_id;
    if (scanf("%d %d %d", &width, &height, &my_id) != 3) return 0;

    for (;;) {
        char grid[H][W];
        for (int r = 0; r < H; r++) {
            char line[32];
            if (scanf(" %31s", line) != 1) return 0;
            for (int c = 0; c < W; c++) grid[r][c] = line[c];
        }
        int n;
        if (scanf("%d", &n) != 1) return 0;
        int mx = -1, my = -1, bombs = 0;
        bool blocked[H][W] = {false};
        for (int i = 0; i < n; i++) {
            int kind, owner, x, y, p1, p2;
            if (scanf("%d %d %d %d %d %d", &kind, &owner, &x, &y, &p1, &p2) != 6) return 0;
            if (kind == 0 && owner == my_id) { mx = x; my = y; bombs = p1; }
            else if (kind == 1 && x >= 0 && x < W && y >= 0 && y < H) blocked[y][x] = true;
        }
        if (mx < 0) {
            printf("MOVE %d %d\n", W / 2, H / 2);
            fflush(stdout);
            continue;
        }

        Point next = {mx, my};
        if (bombs > 0 && blast_boxes(grid, mx, my) > 0) {
            bool b2[H][W];
            memcpy(b2, blocked, sizeof b2);
            b2[my][mx] = true;
            if (bfs_next(grid, mx, my, b2, 2, &next)) printf("BOMB %d %d\n", next.x, next.y);
            else printf("BOMB %d %d\n", mx, my);
        } else {
            if (!bfs_next(grid, mx, my, blocked, 0, &next) &&
                !bfs_next(grid, mx, my, blocked, 1, &next)) next = (Point){W / 2, H / 2};
            printf("MOVE %d %d\n", next.x, next.y);
        }
        fflush(stdout);
    }
}
