/* Troll Farm (Wood) — récolte de l'arbre fruitier le plus proche, puis retour
 * au plus près de la cabane pour déposer. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXW 32
#define MAXH 16
#define MAXCELL (MAXW * MAXH)
#define MAXTREE 128
#define MAXTROLL 32
#define INF 1000000000

static int W, H;
static char grid[MAXH][MAXW];
static int dista[MAXH][MAXW], firstx[MAXH][MAXW], firsty[MAXH][MAXW];
static int qx[MAXCELL], qy[MAXCELL];
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

static int walkable(int x, int y) {
    /* Cell.isWalkable() : seule la case GRASS est praticable, pas la cabane. */
    return x >= 0 && x < W && y >= 0 && y < H && grid[y][x] == '.';
}

int main(void) {
    if (scanf("%d %d", &W, &H) != 2) return 0;
    int sx = -1, sy = -1;
    for (int y = 0; y < H; ++y) {
        if (scanf("%31s", grid[y]) != 1) return 0;
        for (int x = 0; x < W; ++x) {
            if (grid[y][x] == '0') { sx = x; sy = y; }
        }
    }

    for (;;) {
        for (int i = 0; i < 12; ++i) {
            int v;
            if (scanf("%d", &v) != 1) return 0;
        }
        int ntree;
        if (scanf("%d", &ntree) != 1) return 0;
        if (ntree > MAXTREE) ntree = MAXTREE;
        int tx[MAXTREE], ty[MAXTREE], fruits[MAXTREE];
        for (int i = 0; i < ntree; ++i) {
            char kind[16];
            int _health, _cd;
            if (scanf("%15s %d %d %d %d %d %d", kind, &tx[i], &ty[i],
                      &_health, &_health, &fruits[i], &_cd) != 7) return 0;
        }
        int ntroll;
        if (scanf("%d", &ntroll) != 1) return 0;
        if (ntroll > MAXTROLL) ntroll = MAXTROLL;
        int id = -1, px = -1, py = -1, carry = 0;
        for (int i = 0; i < ntroll; ++i) {
            int tid, owner, x, y, speed, capacity, power, chop;
            int c[6];
            if (scanf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                      &tid, &owner, &x, &y, &speed, &capacity, &power, &chop,
                      &c[0], &c[1], &c[2], &c[3], &c[4], &c[5]) != 14) return 0;
            if (owner == 0) {
                id = tid; px = x; py = y;
                carry = c[0] + c[1] + c[2] + c[3];
            }
        }
        if (id < 0) { puts("WAIT"); fflush(stdout); continue; }

        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                dista[y][x] = firstx[y][x] = firsty[y][x] = -1;
        int head = 0, tail = 0;
        dista[py][px] = 0; firstx[py][px] = px; firsty[py][px] = py;
        qx[tail] = px; qy[tail] = py; ++tail;
        while (head < tail) {
            int x = qx[head], y = qy[head]; ++head;
            for (int k = 0; k < 4; ++k) {
                int nx = x + DX[k], ny = y + DY[k];
                if (!walkable(nx, ny) || dista[ny][nx] >= 0) continue;
                dista[ny][nx] = dista[y][x] + 1;
                if (dista[y][x] == 0) {
                    firstx[ny][nx] = nx; firsty[ny][nx] = ny;
                } else {
                    firstx[ny][nx] = firstx[y][x];
                    firsty[ny][nx] = firsty[y][x];
                }
                qx[tail] = nx; qy[tail] = ny; ++tail;
            }
        }

        int bx = -1, by = -1, bd = INF, bf = 0;
        if (carry > 0) {
            if (abs(px - sx) + abs(py - sy) == 1) {
                printf("DROP %d\n", id);
                fflush(stdout);
                continue;
            }
            for (int k = 0; k < 4; ++k) {
                int nx = sx + DX[k], ny = sy + DY[k];
                if (nx < 0 || nx >= W || ny < 0 || ny >= H || dista[ny][nx] < 0) continue;
                int d = dista[ny][nx];
                if (d < bd || (d == bd && (ny < by || (ny == by && nx < bx)))) {
                    bd = d; bx = nx; by = ny;
                }
            }
        } else {
            for (int i = 0; i < ntree; ++i) {
                if (fruits[i] <= 0) continue;
                if (tx[i] == px && ty[i] == py) {
                    printf("HARVEST %d\n", id);
                    fflush(stdout);
                    goto next_turn;
                }
                int d = (tx[i] >= 0 && tx[i] < W && ty[i] >= 0 && ty[i] < H)
                            ? dista[ty[i]][tx[i]] : INF;
                if (d < 0) d = INF;
                int f = -fruits[i];
                if (d < bd || (d == bd && (f < bf ||
                    (f == bf && (ty[i] < by || (ty[i] == by && tx[i] < bx)))))) {
                    bd = d; bf = f; bx = tx[i]; by = ty[i];
                }
            }
        }
        if (bx >= 0 && bx < W && by >= 0 && by < H && dista[by][bx] >= 0) {
            printf("MOVE %d %d %d\n", id, firstx[by][bx], firsty[by][bx]);
        } else {
            puts("WAIT");
        }
    next_turn:
        fflush(stdout);
    }
    return 0;
}
