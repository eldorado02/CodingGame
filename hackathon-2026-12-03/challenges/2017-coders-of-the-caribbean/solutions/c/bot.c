/* Coders of the Caribbean Wood 3 — viser le baril le plus proche. */
#include <stdio.h>
#include <string.h>

#define MAXB 64

int main(void) {
    int ships;
    while (scanf("%d", &ships) == 1) {
        int n;
        if (scanf("%d", &n) != 1) return 0;
        int sx = -1, sy = -1;
        int bx[MAXB], by[MAXB], nb = 0;
        for (int i = 0; i < n; i++) {
            int id, x, y, a1, a2, a3, a4;
            char kind[16];
            if (scanf("%d %15s %d %d %d %d %d %d", &id, kind, &x, &y, &a1, &a2, &a3, &a4) != 8) return 0;
            if (strcmp(kind, "SHIP") == 0 && a4 == 1) {
                sx = x; sy = y;
            } else if (strcmp(kind, "BARREL") == 0 && nb < MAXB) {
                bx[nb] = x; by[nb] = y; nb++;
            }
        }
        if (sx < 0 || nb == 0) {
            printf("WAIT\n");
        } else {
            int best_x = bx[0], best_y = by[0];
            long long best = 0;
            for (int i = 0; i < nb; i++) {
                long long dx = bx[i] - sx, dy = by[i] - sy;
                long long d2 = dx * dx + dy * dy;
                if (i == 0 || d2 < best) { best = d2; best_x = bx[i]; best_y = by[i]; }
            }
            printf("MOVE %d %d\n", best_x, best_y);
        }
        fflush(stdout);
    }
    return 0;
}
