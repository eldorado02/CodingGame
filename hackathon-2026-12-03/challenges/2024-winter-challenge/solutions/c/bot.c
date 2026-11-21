/* Cellularena (Wood 3+) — HARVESTER puis croissance greedy.
   Les sources déjà harvesées sont bloquées dans le BFS pour ne pas les absorber. */
#include <stdio.h>
#include <string.h>

#define MAXW 32
#define MAXH 32
#define MAXCELL (MAXW * MAXH)
#define MAXORG 512

static int W, H;
static unsigned char wall[MAXH][MAXW], taken[MAXH][MAXW], prot[MAXH][MAXW];
static unsigned char harvest[MAXH][MAXW];

static int seen[MAXH][MAXW], parent_id[MAXH][MAXW];
static int first_x[MAXH][MAXW], first_y[MAXH][MAXW], dista[MAXH][MAXW];
static int qx[MAXCELL], qy[MAXCELL];

static const int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
static const char *const DNAME[4] = {"N", "E", "S", "W"};
/* Ordre lexicographique des noms : E < N < S < W. */
static const int DRANK[4] = {1, 0, 2, 3};

static int dir_index(const char *s) {
    for (int k = 0; k < 4; ++k)
        if (s[0] == DNAME[k][0]) return k;
    return -1;
}

static int better_dist(int nx, int ny, int bx, int by) {
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
        memset(harvest, 0, sizeof(harvest));
        int ox[MAXORG], oy[MAXORG], oid[MAXORG], no = 0;

        for (int i = 0; i < n; ++i) {
            int x, y, owner, id, pdir, proot;
            char kind[32], dir[8];
            if (scanf("%d %d %31s %d %d %7s %d %d", &x, &y, kind, &owner,
                      &id, dir, &pdir, &proot) != 8) return 0;
            if (x < 0 || x >= W || y < 0 || y >= H) continue;
            if (!strcmp(kind, "WALL")) {
                wall[y][x] = 1;
            } else if (kind[1] == '\0' && kind[0] >= 'A' && kind[0] <= 'D') {
                prot[y][x] = (unsigned char)kind[0];
            } else {
                taken[y][x] = 1;
                if (owner == 1 && no < MAXORG) {
                    ox[no] = x; oy[no] = y; oid[no] = id; ++no;
                    if (!strcmp(kind, "HARVESTER")) {
                        int k = dir_index(dir);
                        if (k >= 0) {
                            int sx = x + DX[k], sy = y + DY[k];
                            if (sx >= 0 && sx < W && sy >= 0 && sy < H)
                                harvest[sy][sx] = 1;
                        }
                    }
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

        memset(seen, 0, sizeof(seen));
        int head = 0, tail = 0;
        if (required > 0 && no > 0) {
            for (int i = 0; i < no; ++i) {
                for (int k = 0; k < 4; ++k) {
                    int nx = ox[i] + DX[k], ny = oy[i] + DY[k];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (wall[ny][nx] || taken[ny][nx] || harvest[ny][nx] || seen[ny][nx]) continue;
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
                    int nx = x + DX[k], ny = y + DY[k];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (wall[ny][nx] || taken[ny][nx] || harvest[ny][nx] || seen[ny][nx]) continue;
                    seen[ny][nx] = 1;
                    parent_id[ny][nx] = parent_id[y][x];
                    first_x[ny][nx] = first_x[y][x];
                    first_y[ny][nx] = first_y[y][x];
                    dista[ny][nx] = dista[y][x] + 1;
                    qx[tail] = nx; qy[tail] = ny; ++tail;
                }
            }
        }

        int answer = 0;  /* 0 = WAIT, 1 = HARVESTER, 2 = BASIC */
        int a_id = 0, a_x = 0, a_y = 0, a_dir = 0;
        /* HARVESTER : C et D disponibles. Clé = (prio, y, x, rang(dir)). */
        if (my_c > 0 && my_d > 0) {
            int best_prio = 9, best_y = 0, best_x = 0, best_rank = 9;
            for (int y = 0; y < H; ++y) {
                for (int x = 0; x < W; ++x) {
                    if (!seen[y][x] || dista[y][x] != 1) continue;
                    for (int k = 0; k < 4; ++k) {
                        int sx = x + DX[k], sy = y + DY[k];
                        if (sx < 0 || sx >= W || sy < 0 || sy >= H) continue;
                        if (!prot[sy][sx] || harvest[sy][sx]) continue;
                        int prio = prot[sy][sx] == 'A' ? 0 : 1;
                        if (prio < best_prio || (prio == best_prio &&
                            (y < best_y || (y == best_y &&
                            (x < best_x || (x == best_x && DRANK[k] < best_rank)))))) {
                            best_prio = prio; best_y = y; best_x = x;
                            best_rank = DRANK[k];
                            a_id = parent_id[y][x]; a_x = x; a_y = y; a_dir = k;
                            answer = 1;
                        }
                    }
                }
            }
        }
        /* BASIC : source A, sinon n'importe quelle source, sinon case libre. */
        if (!answer && my_a > 0) {
            int bx = -1, by = -1, phase;
            for (phase = 0; phase < 3 && bx < 0; ++phase) {
                for (int y = 0; y < H; ++y) {
                    for (int x = 0; x < W; ++x) {
                        if (!seen[y][x]) continue;
                        if (phase == 0 && prot[y][x] != 'A') continue;
                        if (phase == 1 && !prot[y][x]) continue;
                        if (bx < 0 || better_dist(x, y, bx, by)) { bx = x; by = y; }
                    }
                }
            }
            if (bx >= 0) {
                a_id = parent_id[by][bx];
                a_x = first_x[by][bx]; a_y = first_y[by][bx];
                answer = 2;
            }
        }

        if (answer == 1)
            printf("GROW %d %d %d HARVESTER %s\n", a_id, a_x, a_y, DNAME[a_dir]);
        else if (answer == 2)
            printf("GROW %d %d %d BASIC\n", a_id, a_x, a_y);
        else
            puts("WAIT");
        for (int i = 1; i < required; ++i) puts("WAIT");
        fflush(stdout);
    }
    return 0;
}
