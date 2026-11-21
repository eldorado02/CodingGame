/* Back Track King (Wood 1+) — ink la région ennemie la plus instable.
   Quatre DISRUPT successifs sur une région l'encrent et détruisent ses tracks. */
#include <stdio.h>
#include <string.h>

#define MAXCELL 1024
#define MAXREG 8192

static int region_at[MAXCELL];
static unsigned char town_region[MAXREG];

static int is_inked(const char *s) {
    return s[0] != '0' && s[0] != 'f' && s[0] != 'F';
}

int main(void) {
    int my_id, width, height;
    if (scanf("%d %d %d", &my_id, &width, &height) != 3) return 0;
    int cells = width * height;
    for (int i = 0; i < cells && i < MAXCELL; ++i) {
        int type;
        if (scanf("%d %d", &region_at[i], &type) != 2) return 0;
    }

    int town_count;
    if (scanf("%d", &town_count) != 1) return 0;
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        char desired[512];
        if (scanf("%d %d %d %511s", &id, &x, &y, desired) != 4) return 0;
        int idx = y * width + x;
        if (idx >= 0 && idx < cells) {
            int rid = region_at[idx];
            if (rid >= 0 && rid < MAXREG) town_region[rid] = 1;
        }
    }

    int opponent = 1 - my_id;
    int my_score, foe_score;
    while (scanf("%d %d", &my_score, &foe_score) == 2) {
        int best_region = -1, best_instability = -1;
        for (int i = 0; i < cells; ++i) {
            int owner, instability;
            char inked[32], active[512];
            if (scanf("%d %d %31s %511s", &owner, &instability,
                      inked, active) != 4) return 0;
            int rid = region_at[i];
            if (owner != opponent || rid < 0 || rid >= MAXREG ||
                town_region[rid] || is_inked(inked)) {
                continue;
            }
            if (instability > best_instability ||
                (instability == best_instability &&
                 (best_region < 0 || rid < best_region))) {
                best_instability = instability;
                best_region = rid;
            }
        }
        if (best_region >= 0)
            printf("DISRUPT %d\n", best_region);
        else
            puts("WAIT");
        fflush(stdout);
    }
    return 0;
}
