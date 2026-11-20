/* A Code of Ice and Fire (Wood 3) — rush du QG adverse. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 12
#define H 12
#define INF 1000000
#define MAXU 256

typedef struct { int d, x, y; } Cand;

static char linebuf[512];

static char *next_line(void) {
    if (!fgets(linebuf, sizeof(linebuf), stdin)) return NULL;
    size_t n = strlen(linebuf);
    while (n && (linebuf[n - 1] == '\n' || linebuf[n - 1] == '\r'))
        linebuf[--n] = 0;
    return linebuf;
}

static int cmp_cand(const void *a, const void *b) {
    const Cand *p = a, *q = b;
    if (p->d != q->d) return p->d - q->d;
    if (p->x != q->x) return p->x - q->x;
    return p->y - q->y;
}

int main(void) {
    char *p = next_line();
    if (!p) return 0;
    int mines = atoi(p);
    for (int i = 0; i < mines; i++) next_line();

    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    while ((p = next_line()) != NULL) {
        if (!*p) break;
        int gold = atoi(p);
        next_line(); /* income */
        next_line(); /* opponent gold */
        next_line(); /* opponent income */

        char grid[H][W + 1];
        for (int y = 0; y < H; y++) {
            char *g = next_line();
            if (!g) return 0;
            strncpy(grid[y], g, W);
            grid[y][W] = 0;
        }

        int myhx = -1, myhy = -1, enhx = -1, enhy = -1;
        int nb = atoi(next_line());
        for (int i = 0; i < nb; i++) {
            int owner, kind, x, y;
            if (sscanf(next_line(), "%d %d %d %d", &owner, &kind, &x, &y) != 4)
                return 0;
            if (kind == 0) {
                if (owner == 0) { myhx = x; myhy = y; }
                else { enhx = x; enhy = y; }
            }
        }

        int uid[MAXU], nu = 0;
        int count = atoi(next_line());
        for (int i = 0; i < count; i++) {
            int owner, id, level, x, y;
            if (sscanf(next_line(), "%d %d %d %d %d", &owner, &id, &level, &x, &y) != 5)
                return 0;
            (void)level; (void)x; (void)y;
            if (owner == 0 && nu < MAXU) {
                uid[nu++] = id;
            }
        }

        if (enhx < 0) {
            puts("WAIT");
            fflush(stdout);
            continue;
        }

        int dist[H][W];
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) dist[y][x] = INF;
        int qx[H * W], qy[H * W], head = 0, tail = 0;
        dist[enhy][enhx] = 0;
        qx[tail] = enhx; qy[tail] = enhy; tail++;
        while (head < tail) {
            int x = qx[head], y = qy[head]; head++;
            int nd = dist[y][x] + 1;
            for (int k = 0; k < 4; k++) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                if (grid[ny][nx] == '#') continue;
                if (dist[ny][nx] != INF) continue;
                dist[ny][nx] = nd;
                qx[tail] = nx; qy[tail] = ny; tail++;
            }
        }

        int first = 1;
        for (int i = 0; i < nu; i++) {
            if (!first) putchar(';');
            printf("MOVE %d %d %d", uid[i], enhx, enhy);
            first = 0;
        }

        Cand cand[H * W];
        int nc = 0;
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                if (grid[y][x] == '#') continue;
                int playable = (grid[y][x] == 'O');
                if (!playable) {
                    for (int k = 0; k < 4 && !playable; k++) {
                        int nx = x + dx[k], ny = y + dy[k];
                        if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                        if (grid[ny][nx] == 'O') playable = 1;
                    }
                }
                if (!playable) continue;
                if (myhx >= 0 && x == myhx && y == myhy) continue;
                if (dist[y][x] == INF) continue;
                cand[nc].d = dist[y][x];
                cand[nc].x = x;
                cand[nc].y = y;
                nc++;
            }
        }
        qsort(cand, (size_t)nc, sizeof(cand[0]), cmp_cand);

        int ntrain = gold >= 10 ? gold / 10 : 0;
        for (int i = 0; i < ntrain && i < nc; i++) {
            if (!first) putchar(';');
            printf("TRAIN 1 %d %d", cand[i].x, cand[i].y);
            first = 0;
        }
        if (first) puts("WAIT");
        else putchar('\n');
        fflush(stdout);
    }
    return 0;
}
