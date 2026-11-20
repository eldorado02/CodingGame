/* Code of Kutulu Wood 3 — réassurance prioritaire, fuite des wanderers. */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXW 40
#define MAXH 32
#define MAXE 128

static const int DX[5] = {0, 0, 0, -1, 1};
static const int DY[5] = {0, -1, 1, 0, 0};

int main(void) {
    int w, h;
    if (scanf("%d %d", &w, &h) != 2) return 0;
    char grid[MAXH][MAXW + 1];
    for (int y = 0; y < h; y++) {
        if (scanf("%39s", grid[y]) != 1) return 0;
    }
    int a, b, c, d;
    if (scanf("%d %d %d %d", &a, &b, &c, &d) != 4) return 0;

    for (;;) {
        int ec;
        if (scanf("%d", &ec) != 1) return 0;
        int mx = 0, my = 0, no = 0, nw = 0;
        int ox[MAXE], oy[MAXE], wx[MAXE], wy[MAXE];
        for (int i = 0; i < ec; i++) {
            char type[32];
            int id, x, y, p0, p1, p2;
            if (scanf("%31s %d %d %d %d %d %d", type, &id, &x, &y,
                      &p0, &p1, &p2) != 7) return 0;
            if (i == 0) {
                mx = x; my = y;
            } else if (strcmp(type, "EXPLORER") == 0) {
                if (p0 > 0) { ox[no] = x; oy[no] = y; no++; }
            } else if (strcmp(type, "WANDERER") == 0 && p1 == 1) {
                wx[nw] = x; wy[nw] = y; nw++;
            }
        }

        long long best = LLONG_MIN;
        int bx = mx, by = my;
        for (int k = 0; k < 5; k++) {
            int px = mx + DX[k], py = my + DY[k];
            if (px < 0 || px >= w || py < 0 || py >= h || grid[py][px] == '#') continue;
            int reassured = 0, de = 100;
            for (int j = 0; j < no; j++) {
                int dd = abs(px - ox[j]) + abs(py - oy[j]);
                if (dd <= 2) reassured = 1;
                if (dd < de) de = dd;
            }
            int dw = 100;
            for (int j = 0; j < nw; j++) {
                int dd = abs(px - wx[j]) + abs(py - wy[j]);
                if (dd < dw) dw = dd;
            }
            long long score = (reassured ? 100000LL : 0) + 100LL * dw - de;
            if (score > best) { best = score; bx = px; by = py; }
        }
        if (bx == mx && by == my) printf("WAIT\n");
        else printf("MOVE %d %d\n", bx, by);
        fflush(stdout);
    }
    return 0;
}
