/* Fantastic Bits Wood 2 — attaquant + défenseur, THROW au but après grab. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define W 16000
#define H 7500
#define GOAL_Y 3750

typedef struct { int id, x, y, vx, vy, state; } Entity;

static long long d2(int ax, int ay, int bx, int by) {
    long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

static int clamp(int v, int lo, int hi) {
    return v < lo ? lo : v > hi ? hi : v;
}

int main(void) {
    int team;
    if (scanf("%d", &team) != 1) return 0;
    int goal_x = team == 0 ? W : 0;
    int own_x = team == 0 ? 0 : W;

    for (;;) {
        int ms, mm, os, om, n;
        if (scanf("%d %d", &ms, &mm) != 2) return 0;
        if (scanf("%d %d", &os, &om) != 2) return 0;
        if (scanf("%d", &n) != 1) return 0;
        (void)ms; (void)mm; (void)os; (void)om;

        Entity wizards[4], snaffles[13];
        int nw = 0, ns = 0;
        for (int i = 0; i < n; i++) {
            int id, x, y, vx, vy, state;
            char kind[32];
            if (scanf("%d %31s %d %d %d %d %d", &id, kind, &x, &y, &vx, &vy, &state) != 7) return 0;
            Entity e = {id, x, y, vx, vy, state};
            if (strcmp(kind, "WIZARD") == 0) wizards[nw++] = e;
            else if (strcmp(kind, "SNAFFLE") == 0) snaffles[ns++] = e;
        }

        bool chosen[13] = {false};
        for (int i = 0; i < nw; i++) {
            Entity w = wizards[i];
            if (w.state == 1) {
                printf("THROW %d %d 500\n", goal_x, GOAL_Y);
                fflush(stdout);
                continue;
            }

            int best = -1;
            if (i == 0) {
                for (int s = 0; s < ns; s++) {
                    if (chosen[s]) continue;
                    long long dd = d2(w.x, w.y, snaffles[s].x, snaffles[s].y);
                    if (best < 0 || dd < d2(w.x, w.y, snaffles[best].x, snaffles[best].y)) best = s;
                }
            } else {
                for (int s = 0; s < ns; s++) {
                    if (chosen[s]) continue;
                    bool in_own = (snaffles[s].x < W / 2) == (team == 0);
                    if (!in_own) continue;
                    if (best < 0 || d2(own_x, GOAL_Y, snaffles[s].x, snaffles[s].y) <
                                    d2(own_x, GOAL_Y, snaffles[best].x, snaffles[best].y)) best = s;
                }
                if (best < 0) {
                    for (int s = 0; s < ns; s++) {
                        if (chosen[s]) continue;
                        if (best < 0 || d2(w.x, w.y, snaffles[s].x, snaffles[s].y) <
                                        d2(w.x, w.y, snaffles[best].x, snaffles[best].y)) best = s;
                    }
                }
            }

            if (best < 0) {
                printf("MOVE %d %d 150\n", goal_x, GOAL_Y);
                fflush(stdout);
                continue;
            }
            chosen[best] = true;
            int tx = clamp(snaffles[best].x + 3 * snaffles[best].vx, 0, W);
            int ty = clamp(snaffles[best].y + 3 * snaffles[best].vy, 0, H);
            int thrust = d2(w.x, w.y, tx, ty) > 1200LL * 1200 ? 150 : 60;
            printf("MOVE %d %d %d\n", tx, ty, thrust);
            fflush(stdout);
        }
    }
}
