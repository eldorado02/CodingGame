/* Code Royale Wood 3 — barracks KNIGHT, TRAIN en boucle. */
#include <math.h>
#include <stdio.h>

#define MAXS 16
#define MAXTRAIN 16

typedef struct {
    int id, x, y, r;
    int stype, owner, p1, p2;
} Site;

static double dist(int ax, int ay, int bx, int by) {
    double dx = ax - bx, dy = ay - by;
    return sqrt(dx * dx + dy * dy);
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    Site sites[MAXS];
    if (n > MAXS) n = MAXS;
    for (int i = 0; i < n; i++) {
        if (scanf("%d %d %d %d", &sites[i].id, &sites[i].x, &sites[i].y, &sites[i].r) != 4) return 0;
        sites[i].stype = sites[i].owner = sites[i].p1 = sites[i].p2 = -1;
    }

    for (;;) {
        int gold, touched;
        if (scanf("%d %d", &gold, &touched) != 2) return 0;
        (void)touched;

        for (int i = 0; i < n; i++) {
            int sid, i1, i2, st, ow, p1, p2;
            if (scanf("%d %d %d %d %d %d %d", &sid, &i1, &i2, &st, &ow, &p1, &p2) != 7) return 0;
            for (int j = 0; j < n; j++) {
                if (sites[j].id == sid) {
                    sites[j].stype = st;
                    sites[j].owner = ow;
                    sites[j].p1 = p1;
                    sites[j].p2 = p2;
                    break;
                }
            }
        }

        int nu;
        if (scanf("%d", &nu) != 1) return 0;
        int mx = 200, my = 200, ex = -1, ey = -1;
        for (int i = 0; i < nu; i++) {
            int x, y, owner, utype, hp;
            if (scanf("%d %d %d %d %d", &x, &y, &owner, &utype, &hp) != 5) return 0;
            if (utype == -1) {
                if (owner == 0) { mx = x; my = y; }
                else { ex = x; ey = y; }
            }
        }
        if (ex < 0) { ex = 1920 - mx; ey = 1000 - my; }

        /* 1. Détruire un barracks ennemi proche par contact. */
        const Site* destroy = NULL;
        double best_d = 250.0;
        for (int i = 0; i < n; i++) {
            if (sites[i].stype == 2 && sites[i].owner == 1) {
                double d = dist(mx, my, sites[i].x, sites[i].y);
                if (d < best_d) { best_d = d; destroy = &sites[i]; }
            }
        }
        if (destroy) {
            printf("MOVE %d %d\n", destroy->x, destroy->y);
        } else {
            /* 2. Meilleur site libre pour un barracks KNIGHT. */
            int best_i = -1;
            double best_score = 1e18;
            for (int i = 0; i < n; i++) {
                const Site* s = &sites[i];
                if (s->owner == 1) continue;
                if (s->stype == 2 && s->p1 > 0) continue;
                if (s->stype == 2 && s->p2 == 0) continue;
                double de = dist(s->x, s->y, ex, ey);
                double dm = dist(s->x, s->y, mx, my);
                double score = de + 0.6 * dm;
                if (de < 140) score += 10000;
                if (score < best_score) { best_score = score; best_i = i; }
            }
            if (best_i < 0) printf("WAIT\n");
            else printf("BUILD %d BARRACKS-KNIGHT\n", sites[best_i].id);
        }

        /* 3. Entraîner les barracks KNIGHT prêts et abordables. */
        int train[MAXTRAIN], nt = 0, left = gold;
        for (int i = 0; i < n && nt < MAXTRAIN; i++) {
            const Site* s = &sites[i];
            if (s->stype == 2 && s->owner == 0 && s->p1 == 0 && s->p2 == 0 && left >= 80) {
                train[nt++] = s->id;
                left -= 80;
            }
        }
        printf("TRAIN");
        for (int i = 0; i < nt; i++) printf(" %d", train[i]);
        printf("\n");
        fflush(stdout);
    }
    return 0;
}
