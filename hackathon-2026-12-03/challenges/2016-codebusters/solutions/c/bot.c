/* CodeBusters — stratégie Wood : un fantôme par buster, bande de capture. */
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define W 16000
#define H 9000
#define STEP 800
#define BUST_MIN2 (900LL * 900LL)
#define BUST_MAX2 (1760LL * 1760LL)

typedef struct { int id, x, y, state; } Buster;
typedef struct { int id, x, y; } Ghost;

static void clamp(int *v, int lo, int hi) {
    if (*v < lo) *v = lo;
    if (*v > hi) *v = hi;
}

/* Écrit dans nx/ny le point à 800 unités de (x,y) vers (tx,ty). */
static void step_toward(int x, int y, int tx, int ty, int *nx, int *ny) {
    long long dx = tx - x, dy = ty - y;
    long long d2 = dx * dx + dy * dy;
    if (d2 == 0) { *nx = x; *ny = y; return; }
    double d = sqrt((double)d2);
    *nx = (int)floor(x + STEP * dx / d + 0.5);
    *ny = (int)floor(y + STEP * dy / d + 0.5);
    clamp(nx, 0, W);
    clamp(ny, 0, H);
}

static long long dist2(int ax, int ay, int bx, int by) {
    long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

int main(void) {
    int per_player, ghost_count, team;
    if (scanf("%d %d %d", &per_player, &ghost_count, &team) != 3) return 0;

    for (;;) {
        int n;
        if (scanf("%d", &n) != 1) return 0;
        Buster busters[5];
        Ghost ghosts[28];
        int nb = 0, ng = 0;
        for (int i = 0; i < n; i++) {
            int id, x, y, type, state, value;
            if (scanf("%d %d %d %d %d %d", &id, &x, &y, &type, &state, &value) != 6) return 0;
            if (type == team) busters[nb++] = (Buster){id, x, y, state};
            else if (type == -1) ghosts[ng++] = (Ghost){id, x, y};
        }

        int base_x = team == 0 ? 0 : W;
        int base_y = team == 0 ? 0 : H;
        bool reserved[28] = {false};

        for (int i = 0; i < nb; i++) {
            Buster b = busters[i];
            if (b.state == 1) {
                if (dist2(b.x, b.y, base_x, base_y) <= (long long)STEP * STEP) {
                    puts("RELEASE");
                } else {
                    int nx, ny;
                    step_toward(b.x, b.y, base_x, base_y, &nx, &ny);
                    printf("MOVE %d %d\n", nx, ny);
                }
                fflush(stdout);
                continue;
            }

            int best = -1;
            long long best_d2 = 0;
            for (int g = 0; g < ng; g++) {
                if (reserved[g]) continue;
                long long d2 = dist2(b.x, b.y, ghosts[g].x, ghosts[g].y);
                if (best < 0 || d2 < best_d2) { best = g; best_d2 = d2; }
            }
            if (best < 0) {
                int nx, ny;
                step_toward(b.x, b.y, W / 2, H / 2, &nx, &ny);
                printf("MOVE %d %d\n", nx, ny);
                fflush(stdout);
                continue;
            }

            long long d2 = best_d2;
            Ghost g = ghosts[best];
            if (d2 >= BUST_MIN2 && d2 <= BUST_MAX2) {
                reserved[best] = true;
                printf("BUST %d\n", g.id);
                fflush(stdout);
                continue;
            }
            int tx, ty;
            if (d2 < BUST_MIN2) {
                tx = b.x - (g.x - b.x);
                ty = b.y - (g.y - b.y);
            } else {
                tx = g.x;
                ty = g.y;
            }
            int nx, ny;
            step_toward(b.x, b.y, tx, ty, &nx, &ny);
            printf("MOVE %d %d\n", nx, ny);
            fflush(stdout);
        }
    }
}
