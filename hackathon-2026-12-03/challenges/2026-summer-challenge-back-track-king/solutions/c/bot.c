/* Back Track King (Wood) — construit en boucle la connexion entre le couple
 * de villes (demandeur, cible) dont la distance de Manhattan est minimale. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXTOWN 32
#define MAXTARGET 256
#define MAXWORD 300

static int town_x[MAXTOWN], town_y[MAXTOWN];

static int absi(int v) { return v < 0 ? -v : v; }

int main(void) {
    int my_id, width, height;
    if (scanf("%d %d %d", &my_id, &width, &height) != 3) return 0;
    for (int i = 0; i < width * height; ++i) {
        int region, type;
        if (scanf("%d %d", &region, &type) != 2) return 0;
    }

    int town_count;
    if (scanf("%d", &town_count) != 1) return 0;
    if (town_count > MAXTOWN) town_count = MAXTOWN;
    int wanted_u[MAXTARGET], wanted_v[MAXTARGET], target_count = 0;
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        char desired[MAXWORD];
        if (scanf("%d %d %d %299s", &id, &x, &y, desired) != 4) return 0;
        town_x[id] = x;
        town_y[id] = y;
        if (strcmp(desired, "x") == 0) continue;
        for (char *tok = strtok(desired, ","); tok; tok = strtok(NULL, ",")) {
            if (target_count >= MAXTARGET) break;
            wanted_u[target_count] = id;
            wanted_v[target_count] = atoi(tok);
            ++target_count;
        }
    }

    int best_u = wanted_u[0], best_v = wanted_v[0];
    int best_d = absi(town_x[best_u] - town_x[best_v])
               + absi(town_y[best_u] - town_y[best_v]);
    for (int i = 1; i < target_count; ++i) {
        int u = wanted_u[i], v = wanted_v[i];
        int d = absi(town_x[u] - town_x[v]) + absi(town_y[u] - town_y[v]);
        if (d < best_d || (d == best_d &&
            (u < best_u || (u == best_u && v < best_v)))) {
            best_d = d;
            best_u = u;
            best_v = v;
        }
    }
    int x1 = town_x[best_u], y1 = town_y[best_u];
    int x2 = town_x[best_v], y2 = town_y[best_v];

    int my_score, foe_score;
    while (scanf("%d %d", &my_score, &foe_score) == 2) {
        for (int i = 0; i < width * height; ++i) {
            int owner, instability;
            char inked[MAXWORD], active[MAXWORD];
            if (scanf("%d %d %299s %299s", &owner, &instability,
                      inked, active) != 4) return 0;
        }
        printf("AUTOPLACE %d %d %d %d\n", x1, y1, x2, y2);
        fflush(stdout);
    }
    return 0;
}
