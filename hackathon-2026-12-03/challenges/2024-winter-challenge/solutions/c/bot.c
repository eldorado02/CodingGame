/* Cellularena (Wood) — BFS multi-source vers la protéine A la plus proche. */
#include <stdio.h>
#include <string.h>

#define MAXW 32
#define MAXH 32
#define MAXCELL (MAXW * MAXH)
#define MAXORG 512

static int W, H;
static unsigned char wall[MAXH][MAXW], taken[MAXH][MAXW], prot[MAXH][MAXW];

static int seen[MAXH][MAXW], parent_id[MAXH][MAXW];
static int first_x[MAXH][MAXW], first_y[MAXH][MAXW], dista[MAXH][MAXW];
static int qx[MAXCELL], qy[MAXCELL];

static int better(int nx, int ny, int bx, int by) {
    if (dista[ny][nx] != dista[by][bx]) return dista[ny][nx] < dista[by][bx];
    if (ny != by) return ny < by;
    return nx < bx;
}

int main(void) {
    if (scanf("%d %d", &W, &H) != 2) return 0;
    for (;;) {
        int n;
        if (scanf("%d", &n) != 1) return 0;
        memset(wall, 0, sizeof(wall));
        memset(taken, 0, sizeof(taken));
        memset(prot, 0, sizeof(prot));
        int ox[MAXORG], oy[MAXORG], oid[MAXORG], no = 0;

        for (int i = 0; i < n; ++i) {
            int x, y, owner, id, pdir, proot;
            char kind[32], dir[4];
            if (scanf("%d %d %31s %d %d %3s %d %d", &x, &y, kind, &owner,
                      &id, dir, &pdir, &proot) != 8) return 0;
            if (x < 0 || x >= W || y < 0 || y >= H) continue;
            if (!strcmp(kind, "WALL")) {
                wall[y][x] = 1;
            } else if (kind[1] == '\0' && kind[0] >= 'A' && kind[0] <= 'D') {
                prot[y][x] = 1;
            } else {
                taken[y][x] = 1;
                if (owner == 1 && no < MAXORG) {
                    ox[no] = x; oy[no] = y; oid[no] = id; ++no;
                }
            }
        }
        int my_a, my_b, my_c, my_d;
        if (scanf("%d %d %d %d", &my_a, &my_b, &my_c, &my_d) != 4) return 0;
        for (int i = 0; i < 4; ++i) {
            int v;
            if (scanf("%d", &v) != 1) return 0;
        }
        int required;
        if (scanf("%d", &required) != 1) return 0;

        const char *answer = "WAIT";
        int grow_id = 0, grow_x = 0, grow_y = 0;
        if (my_a > 0 && no > 0) {
            memset(seen, 0, sizeof(seen));
            int head = 0, tail = 0;
            const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
            for (int i = 0; i < no; ++i) {
                for (int k = 0; k < 4; ++k) {
                    int nx = ox[i] + dx[k], ny = oy[i] + dy[k];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (wall[ny][nx] || taken[ny][nx] || seen[ny][nx]) continue;
                    seen[ny][nx] = 1;
                    parent_id[ny][nx] = oid[i];
                    first_x[ny][nx] = nx; first_y[ny][nx] = ny;
                    dista[ny][nx] = 1;
                    qx[tail] = nx; qy[tail] = ny; ++tail;
                }
            }
            while (head < tail) {
                int x = qx[head], y = qy[head]; ++head;
                for (int k = 0; k < 4; ++k) {
                    int nx = x + dx[k], ny = y + dy[k];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (wall[ny][nx] || taken[ny][nx] || seen[ny][nx]) continue;
                    seen[ny][nx] = 1;
                    parent_id[ny][nx] = parent_id[y][x];
                    first_x[ny][nx] = first_x[y][x];
                    first_y[ny][nx] = first_y[y][x];
                    dista[ny][nx] = dista[y][x] + 1;
                    qx[tail] = nx; qy[tail] = ny; ++tail;
                }
            }
            int bx = -1, by = -1;
            for (int y = 0; y < H; ++y) {
                for (int x = 0; x < W; ++x) {
                    if (!seen[y][x]) continue;
                    if (prot[y][x]) {
                        if (bx < 0 || better(x, y, bx, by)) { bx = x; by = y; }
                    }
                }
            }
            if (bx < 0) {
                for (int y = 0; y < H; ++y) {
                    for (int x = 0; x < W; ++x) {
                        if (seen[y][x] && (bx < 0 || better(x, y, bx, by))) {
                            bx = x; by = y;
                        }
                    }
                }
            }
            if (bx >= 0) {
                grow_id = parent_id[by][bx];
                grow_x = first_x[by][bx];
                grow_y = first_y[by][bx];
                answer = "GROW";
            }
        }
        if (answer[0] == 'G') {
            printf("GROW %d %d %d BASIC\n", grow_id, grow_x, grow_y);
        } else {
            puts("WAIT");
        }
        for (int i = 1; i < required; ++i) puts("WAIT");
        fflush(stdout);
    }
    return 0;
}
