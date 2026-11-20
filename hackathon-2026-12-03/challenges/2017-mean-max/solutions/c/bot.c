/* Mean Max Wood 3 — simulation 1 pas des actions, reste dans les épaves. */
#include <math.h>
#include <stdio.h>

#define MAXU 64
#define MAXC 96

typedef struct { int x, y, r, water; } Wreck;

static int round_away(double z) {
    return z >= 0 ? (int)floor(z + 0.5) : -(int)floor(-z + 0.5);
}

typedef struct {
    int has_target;
    double tx, ty;
    int power;
} Candidate;

static void simulate(int x, int y, int vx, int vy, const Candidate* c,
                     int* nx, int* ny, int* nvx, int* nvy) {
    double ax = 0, ay = 0;
    if (c->has_target && c->power > 0) {
        double dx = c->tx - x, dy = c->ty - y;
        double dist = sqrt(dx * dx + dy * dy);
        if (dist > 1e-9) {
            double coef = 2.0 * c->power / dist;
            ax = dx * coef;
            ay = dy * coef;
        }
    }
    double sfx = vx + ax, sfy = vy + ay;
    *nx = round_away(x + sfx);
    *ny = round_away(y + sfy);
    *nvx = round_away(sfx * 0.8);
    *nvy = round_away(sfy * 0.8);
}

static double eval(int nx, int ny, int nvx, int nvy,
                   const Wreck* w, int nw) {
    int inside = 0, water = 0;
    double closest = 1e18;
    for (int i = 0; i < nw; i++) {
        double dx = nx - w[i].x, dy = ny - w[i].y;
        double d = sqrt(dx * dx + dy * dy);
        if (d <= w[i].r) { inside++; water += w[i].water; }
        if (d < closest) closest = d;
    }
    double speed = sqrt((double)nvx * nvx + (double)nvy * nvy);
    return inside * 1000000.0 + water * 10.0 - closest - speed;
}

int main(void) {
    for (;;) {
        /* 3 scores + 3 lignes de rage (placeholders) : 6 entiers ignorés. */
        for (int i = 0; i < 6; i++) {
            int ignored;
            if (scanf("%d", &ignored) != 1) return 0;
        }
        int n;
        if (scanf("%d", &n) != 1) return 0;

        int rx = 0, ry = 0, rvx = 0, rvy = 0, have = 0;
        Wreck wrecks[MAXU];
        int nw = 0;
        for (int i = 0; i < n; i++) {
            int id, type, player, radius, x, y, vx, vy, extra, extra2;
            double mass;
            if (scanf("%d %d %d %lf %d %d %d %d %d %d %d", &id, &type, &player,
                      &mass, &radius, &x, &y, &vx, &vy, &extra, &extra2) != 11) return 0;
            if (type == 0 && player == 0) {
                rx = x; ry = y; rvx = vx; rvy = vy; have = 1;
            } else if (type == 4) {
                wrecks[nw++] = (Wreck){x, y, radius, extra};
            }
        }
        if (!have) { printf("WAIT\nWAIT\nWAIT\n"); fflush(stdout); continue; }

        Candidate cands[MAXC];
        int nc = 0;
        cands[nc++] = (Candidate){0, 0, 0, 0};                 /* WAIT */
        static const int powers[] = {0, 100, 200, 300};
        for (int i = 0; i < nw; i++)
            for (int p = 0; p < 4; p++)
                cands[nc++] = (Candidate){1, wrecks[i].x, wrecks[i].y, powers[p]};
        if (rvx || rvy) {
            cands[nc++] = (Candidate){1, rx - rvx, ry - rvy, 150};
            cands[nc++] = (Candidate){1, rx - rvx, ry - rvy, 300};
        }

        double best = -1e18;
        int best_target = 0, best_tx = 0, best_ty = 0, best_power = 0;
        for (int i = 0; i < nc; i++) {
            int nx, ny, nvx, nvy;
            simulate(rx, ry, rvx, rvy, &cands[i], &nx, &ny, &nvx, &nvy);
            double s = eval(nx, ny, nvx, nvy, wrecks, nw);
            if (s > best) {
                best = s;
                best_target = cands[i].has_target;
                best_tx = (int)cands[i].tx;
                best_ty = (int)cands[i].ty;
                best_power = cands[i].power;
            }
        }
        if (!best_target || best_power == 0) printf("WAIT\n");
        else printf("%d %d %d\n", best_tx, best_ty, best_power);
        printf("WAIT\nWAIT\n");
        fflush(stdout);
    }
}
