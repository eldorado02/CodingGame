/* Spring Challenge 2021 (Wood 4) — compléter les meilleurs arbres au plus vite. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXC 64
static int richness[MAXC];
static char line[4096];

static char *rd(void) {
    if (!fgets(line, sizeof(line), stdin)) return NULL;
    size_t n = 0;
    while (line[n]) n++;
    while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = 0;
    return line;
}

int main(void) {
    char *p = rd();
    if (!p) return 0;
    int n;
    if (sscanf(p, "%d", &n) != 1) return 0;
    for (int i = 0; i < n; i++) {
        char c[4096];
        char *r = rd();
        if (!r) return 0;
        strcpy(c, r);
        int idx, rch, nb[6];
        if (sscanf(c, "%d %d %d %d %d %d %d %d", &idx, &rch, &nb[0], &nb[1],
                   &nb[2], &nb[3], &nb[4], &nb[5]) >= 2 && idx >= 0 && idx < MAXC)
            richness[idx] = rch;
    }

    while ((p = rd()) != NULL) {
        if (!*p) break;
        rd(); /* nutrients */
        char sunline[4096];
        char *s = rd();
        if (!s) break;
        strcpy(sunline, s);
        int my_sun, my_score;
        if (sscanf(sunline, "%d %d", &my_sun, &my_score) != 2) break;
        rd(); /* adversaire */
        int nt = atoi(rd());
        int best_rich = 0, best_idx = -1;
        for (int i = 0; i < nt; i++) {
            char tl[4096];
            char *t = rd();
            if (!t) return 0;
            strcpy(tl, t);
            int idx, size, mine, dormant;
            if (sscanf(tl, "%d %d %d %d", &idx, &size, &mine, &dormant) != 4) continue;
            if (mine == 1 && size == 3) {
                int rch = idx >= 0 && idx < MAXC ? richness[idx] : 0;
                if (best_idx < 0 || rch > best_rich || (rch == best_rich && idx < best_idx)) {
                    best_rich = rch; best_idx = idx;
                }
            }
        }
        int na = atoi(rd());
        for (int i = 0; i < na; i++) rd();
        if (my_sun >= 4 && best_idx >= 0) printf("COMPLETE %d\n", best_idx);
        else printf("WAIT\n");
        fflush(stdout);
    }
    return 0;
}
