/* Wondev Woman Wood 3 — meilleure action légale : victoire, déni, montée. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXS 7
#define MAXA 64

typedef struct { int dx, dy; } Delta;

static Delta delta_of(const char* d) {
    if (!strcmp(d,"N"))  return (Delta){0,-1};
    if (!strcmp(d,"NE")) return (Delta){1,-1};
    if (!strcmp(d,"E"))  return (Delta){1,0};
    if (!strcmp(d,"SE")) return (Delta){1,1};
    if (!strcmp(d,"S"))  return (Delta){0,1};
    if (!strcmp(d,"SW")) return (Delta){-1,1};
    if (!strcmp(d,"W"))  return (Delta){-1,0};
    return (Delta){-1,-1}; /* NW */
}

static int height(int grid[MAXS][MAXS], int x, int y, int size) {
    if (x < 0 || y < 0 || x >= size || y >= size) return -1;
    return grid[y][x];
}

int main(void) {
    int size, up;
    if (scanf("%d %d", &size, &up) != 2) return 0;
    char line[16];
    for (;;) {
        int grid[MAXS][MAXS];
        for (int y = 0; y < size; y++) {
            if (scanf("%15s", line) != 1) return 0;
            for (int x = 0; x < size; x++)
                grid[y][x] = line[x] == '.' ? -1 : line[x] - '0';
        }
        int ux, uy, ox, oy, n;
        if (scanf("%d %d", &ux, &uy) != 2) return 0;
        if (scanf("%d %d", &ox, &oy) != 2) return 0;
        if (scanf("%d", &n) != 1) return 0;

        char atype[16], d1s[4], d2s[4];
        int index;
        long long best_score = -1000000000LL;
        char best1[4] = "", best2[4] = "";
        int have = 0;

        for (int i = 0; i < n; i++) {
            if (scanf("%15s %d %3s %3s", atype, &index, d1s, d2s) != 4) return 0;
            Delta d1 = delta_of(d1s), d2 = delta_of(d2s);
            int mx = ux + d1.dx, my = uy + d1.dy;
            int h_dest = height(grid, mx, my, size);
            int cx = mx + d2.dx, cy = my + d2.dy;
            int h_build = height(grid, cx, cy, size);

            long long score = 0;
            if (h_dest == 3) score += 1000000;
            if (h_dest < 0) score -= 100000;
            else score += 1000LL * h_dest;
            if (h_build == 3) {
                if (ox >= 0 && abs(cx-ox) <= 1 && abs(cy-oy) <= 1) score += 50000;
                else score -= 50;
            } else if (h_build == 2) score += 300;
            else if (h_build == 1) score += 100;

            if (score > best_score) {
                best_score = score;
                strcpy(best1, d1s); strcpy(best2, d2s);
                have = 1;
            }
        }
        if (have) printf("MOVE&BUILD 0 %s %s\n", best1, best2);
        else printf("WAIT\n");
        fflush(stdout);
    }
}
