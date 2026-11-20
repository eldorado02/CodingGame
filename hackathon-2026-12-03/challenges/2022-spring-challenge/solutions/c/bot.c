/* Spring Challenge 2022 (Wood 4) — défense de base par 3 héros. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_W 17630
#define MAP_H 9000
#define MAXE 512
#define MAXH 8

typedef struct { int id, x, y, nx, ny, hp, threat; } Monster;
typedef struct { int id, x, y; } Hero;

static char line[4096];

static char *rd(void) {
    if (!fgets(line, sizeof(line), stdin)) return NULL;
    size_t n = strlen(line);
    while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = 0;
    return line;
}

static double dist2(int ax, int ay, int bx, int by) {
    double dx = ax - bx, dy = ay - by;
    return sqrt(dx * dx + dy * dy);
}

int main(void) {
    char *p = rd();
    if (!p) return 0;
    int bx, by;
    if (sscanf(p, "%d %d", &bx, &by) != 2) return 0;
    int nh = atoi(rd());

    while ((p = rd()) != NULL) {
        if (!*p) break;
        int myhp, mana;
        if (sscanf(p, "%d %d", &myhp, &mana) != 2) break;
        rd();
        int n = atoi(rd());
        Monster monsters[MAXE];
        Hero heroes[MAXH];
        int nm = 0, nhe = 0;
        for (int i = 0; i < n; i++) {
            char e[4096];
            char *r = rd();
            if (!r) return 0;
            strcpy(e, r);
            int id, type, x, y, shield, ctrl, hp, vx, vy, near, threat;
            if (sscanf(e, "%d %d %d %d %d %d %d %d %d %d %d",
                       &id, &type, &x, &y, &shield, &ctrl, &hp, &vx, &vy,
                       &near, &threat) != 11) continue;
            (void)shield; (void)ctrl;
            if (type == 0) {
                Monster m;
                m.id = id; m.x = x; m.y = y; m.hp = hp; m.threat = threat;
                if (near) {
                    int px = threat == 1 ? bx : MAP_W - bx;
                    int py = threat == 1 ? by : MAP_H - by;
                    double dx = px - x, dy = py - y;
                    double d = dist2(x, y, px, py);
                    if (d < 1) d = 1;
                    m.nx = x + (int)(400.0 * dx / d);
                    m.ny = y + (int)(400.0 * dy / d);
                } else {
                    m.nx = x + vx; m.ny = y + vy;
                }
                if (threat == 1) monsters[nm++] = m;
            } else if (type == 1 && nhe < MAXH) {
                heroes[nhe].id = id; heroes[nhe].x = x; heroes[nhe].y = y;
                nhe++;
            }
        }
        /* Tri des menaces par distance au base puis hp puis coordonnées. */
        for (int i = 1; i < nm; i++) {
            Monster k = monsters[i];
            int j = i - 1;
            while (j >= 0) {
                int cmp = 0;
                double dk = dist2(k.nx, k.ny, bx, by);
                double dj = dist2(monsters[j].nx, monsters[j].ny, bx, by);
                if (dk < dj) cmp = -1;
                else if (dk > dj) cmp = 1;
                else if (k.hp != monsters[j].hp) cmp = k.hp < monsters[j].hp ? -1 : 1;
                else if (k.x != monsters[j].x) cmp = k.x < monsters[j].x ? -1 : 1;
                else cmp = k.y < monsters[j].y ? -1 : 1;
                if (cmp >= 0) break;
                monsters[j + 1] = monsters[j];
                j--;
            }
            monsters[j + 1] = k;
        }

        int used[MAXE] = {0};
        for (int h = 0; h < nhe; h++) {
            int best = -1;
            double bd = 0;
            for (int i = 0; i < nm; i++) {
                if (used[i]) continue;
                double d = dist2(heroes[h].x, heroes[h].y, monsters[i].nx, monsters[i].ny);
                if (best < 0 || d < bd) { best = i; bd = d; }
            }
            if (best < 0) { printf("WAIT\n"); continue; }
            used[best] = 1;
            int tx = monsters[best].nx, ty = monsters[best].ny;
            if (bd <= 800) { tx = heroes[h].x; ty = heroes[h].y; }
            printf("MOVE %d %d\n", tx, ty);
        }
        for (int h = nhe; h < nh; h++) printf("WAIT\n");
        fflush(stdout);
    }
    return 0;
}
