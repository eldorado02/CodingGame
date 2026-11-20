/* Botters of the Galaxy Wood 6 — HULK, ATTACK héros puis tour. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXE 64

typedef struct { int id, x, y, health; char kind[16]; } Entity;

static int next_line(char* buf, int cap) {
    if (!fgets(buf, cap, stdin)) return 0;
    size_t n = strlen(buf);
    while (n && (buf[n-1] == '\n' || buf[n-1] == '\r')) buf[--n] = '\0';
    return 1;
}

static long long d2(int ax, int ay, int bx, int by) {
    long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

static int best_of(const Entity* e, int n, const char* kind, int mx, int my) {
    int best = -1;
    for (int i = 0; i < n; i++) {
        if (strcmp(e[i].kind, kind) != 0) continue;
        if (best < 0 || d2(mx, my, e[i].x, e[i].y) < d2(mx, my, e[best].x, e[best].y)) best = i;
    }
    return best;
}

int main(void) {
    char buf[2048];
    if (!next_line(buf, sizeof buf)) return 0;
    int team = atoi(buf);
    if (!next_line(buf, sizeof buf)) return 0;
    int bushes = atoi(buf);
    for (int i = 0; i < bushes; i++) if (!next_line(buf, sizeof buf)) return 0;
    if (!next_line(buf, sizeof buf)) return 0;
    int items = atoi(buf);
    for (int i = 0; i < items; i++) if (!next_line(buf, sizeof buf)) return 0;

    int mx = 0, my = 0;
    while (next_line(buf, sizeof buf)) {
        int gold = atoi(buf);
        (void)gold;
        if (!next_line(buf, sizeof buf)) return 0;
        int opp_gold = atoi(buf);
        (void)opp_gold;
        if (!next_line(buf, sizeof buf)) return 0;
        int round_type = atoi(buf);
        if (!next_line(buf, sizeof buf)) return 0;
        int count = atoi(buf);

        Entity ents[MAXE];
        int n = 0;
        for (int i = 0; i < count && n < MAXE; i++) {
            if (!next_line(buf, sizeof buf)) return 0;
            Entity e = {0};
            int eteam, range, maxhp, shield, dmg, speed, stun, goldval;
            if (sscanf(buf, "%d %d %15s %d %d %d %d %d %d %d %d %d %d",
                       &e.id, &eteam, e.kind, &e.x, &e.y, &range, &e.health,
                       &maxhp, &shield, &dmg, &speed, &stun, &goldval) < 7) continue;
            if (strcmp(e.kind, "HERO") == 0 && eteam == team) { mx = e.x; my = e.y; }
            if (eteam == team || e.health <= 0) continue;
            ents[n++] = e;
        }

        /* La phase de pick contient aussi des entités : lues ci-dessus. */
        if (round_type < 0) {
            printf("HULK\n");
            fflush(stdout);
            continue;
        }

        int idx = best_of(ents, n, "HERO", mx, my);
        if (idx < 0) idx = best_of(ents, n, "TOWER", mx, my);
        if (idx < 0) {
            for (int i = 0; i < n; i++) {
                if (strcmp(ents[i].kind, "HERO") == 0 || strcmp(ents[i].kind, "TOWER") == 0) continue;
                if (idx < 0 || d2(mx, my, ents[i].x, ents[i].y) < d2(mx, my, ents[idx].x, ents[idx].y)) idx = i;
            }
        }
        for (int h = 0; h < round_type; h++) {
            if (idx >= 0) printf("ATTACK %d\n", ents[idx].id);
            else printf("WAIT\n");
        }
        fflush(stdout);
    }
    return 0;
}
