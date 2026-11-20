/* A*Craft — flèches parcimonieuses, un seul tour de sortie. */
#include <stdio.h>
#include <string.h>

#define W 19
#define H 10
#define CELLS (W * H)
#define LIMIT (4 * W * H)
#define MAXR 10
#define MAXOUT 256

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};
static const char CH[5] = "URDL";

static int wrap(int v, int m) {
    if (v < 0) v += m;
    if (v >= m) v -= m;
    return v;
}

static int board[H][W], isvoid[H][W], isstart[H][W];
static int rx[MAXR], ry[MAXR], rd[MAXR];
static int initial[MAXOUT][3], ninitial;

/* Construit les flèches pour un ordre donné ; renvoie le score. */
static int build(const int *order, int rc, int out[MAXOUT][3], int *nout) {
    int b[H][W], vis[CELLS][4];
    memcpy(b, board, sizeof(b));
    memset(vis, 0, sizeof(vis));
    *nout = 0;
    int total = 0;

    for (int k = 0; k < rc; k++) {
        int ri = order[k];
        int x = rx[ri], y = ry[ri], d = rd[ri];
        vis[y * W + x][d] = 1;
        for (int step = 0; step < LIMIT; step++) {
            int nx = wrap(x + DX[d], W), ny = wrap(y + DY[d], H);
            if (isvoid[ny][nx]) break;
            int nd;
            if (b[ny][nx] != -1) {
                nd = b[ny][nx];
            } else if (isstart[ny][nx]) {
                nd = d;
            } else {
                int best_look = 0, best_d = d, found = 0;
                for (int d2 = 0; d2 < 4; d2++) {
                    if (vis[ny * W + nx][d2]) continue;
                    int tx = wrap(nx + DX[d2], W), ty = wrap(ny + DY[d2], H);
                    int look = 99;
                    if (!isvoid[ty][tx]) {
                        look = vis[ty * W + tx][0] + vis[ty * W + tx][1]
                             + vis[ty * W + tx][2] + vis[ty * W + tx][3];
                    }
                    if (!found || look < best_look) {
                        best_look = look;
                        best_d = d2;
                        found = 1;
                    }
                }
                if (!found) break;
                nd = best_d;
                if (nd != d) {
                    b[ny][nx] = nd;
                    out[(*nout)][0] = nx;
                    out[(*nout)][1] = ny;
                    out[(*nout)][2] = nd;
                    (*nout)++;
                }
            }
            if (vis[ny * W + nx][nd]) break;
            vis[ny * W + nx][nd] = 1;
            total++;
            x = nx; y = ny; d = nd;
        }
    }
    return total;
}

int main(void) {
    for (int y = 0; y < H; y++) {
        char line[32];
        if (scanf("%31s", line) != 1) return 0;
        for (int x = 0; x < W; x++) {
            char c = line[x];
            board[y][x] = -1;
            isvoid[y][x] = 0;
            isstart[y][x] = 0;
            if (c == '#') isvoid[y][x] = 1;
            else if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
            for (int d = 0; d < 4; d++) {
                if (c == CH[d]) board[y][x] = d;
            }
        }
    }
    int rc;
    if (scanf("%d", &rc) != 1) return 0;
    if (rc > MAXR) rc = MAXR;
    for (int i = 0; i < rc; i++) {
        char dir[4];
        if (scanf("%d %d %3s", &rx[i], &ry[i], dir) != 3) return 0;
        if (dir[0] >= 'a' && dir[0] <= 'z') dir[0] = (char)(dir[0] - 'a' + 'A');
        rd[i] = 0;
        for (int d = 0; d < 4; d++) if (dir[0] == CH[d]) rd[i] = d;
        isstart[ry[i]][rx[i]] = 1;
    }

    /*Tour 0 : flèche sous un robot qui fonce dans le vide.*/
    ninitial = 0;
    for (int i = 0; i < rc; i++) {
        int nx = wrap(rx[i] + DX[rd[i]], W), ny = wrap(ry[i] + DY[rd[i]], H);
        if (!isvoid[ny][nx]) continue;
        for (int d2 = 0; d2 < 4; d2++) {
            int tx = wrap(rx[i] + DX[d2], W), ty = wrap(ry[i] + DY[d2], H);
            if (!isvoid[ty][tx]) {
                board[ry[i]][rx[i]] = d2;
                rd[i] = d2;
                initial[ninitial][0] = rx[i];
                initial[ninitial][1] = ry[i];
                initial[ninitial][2] = d2;
                ninitial++;
                break;
            }
        }
    }

    int order[MAXR], p1[MAXOUT][3], p2[MAXOUT][3], n1, n2;
    for (int i = 0; i < rc; i++) order[i] = i;
    int s1 = build(order, rc, p1, &n1);
    for (int i = 0; i < rc; i++) order[i] = rc - 1 - i;
    int s2 = build(order, rc, p2, &n2);

    int first = 1;
    for (int i = 0; i < ninitial; i++) {
        if (!first) putchar(' ');
        printf("%d %d %c", initial[i][0], initial[i][1], CH[initial[i][2]]);
        first = 0;
    }
    int (*extra)[3] = (s2 > s1) ? p2 : p1;
    int nextra = (s2 > s1) ? n2 : n1;
    for (int i = 0; i < nextra; i++) {
        if (!first) putchar(' ');
        printf("%d %d %c", extra[i][0], extra[i][1], CH[extra[i][2]]);
        first = 0;
    }
    putchar('\n');
    fflush(stdout);
    return 0;
}
