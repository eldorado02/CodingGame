/* Code a la Mode — plat BLUEBERRIES + ICE_CREAM puis livraison. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMELEN 64
#define MAXT 60

typedef struct { int x, y; } Pt;

static Pt D = {-1, -1}, W = {-1, -1}, B = {-1, -1}, I = {-1, -1};
static Pt tables[MAXT];
static int ntables = 0;
static char linebuf[512];

static char *next_line(void) {
    if (!fgets(linebuf, sizeof(linebuf), stdin)) return NULL;
    size_t n = strlen(linebuf);
    while (n && (linebuf[n - 1] == '\n' || linebuf[n - 1] == '\r')) linebuf[--n] = 0;
    return linebuf;
}

static int has_food(const char *s, const char *food) { return strstr(s, food) != NULL; }
static int is_dish(const char *s) { return strncmp(s, "DISH", 4) == 0; }

static void contents(const char *s, bool *bb, bool *ic) {
    *bb = *ic = false;
    if (!is_dish(s)) return;
    *bb = has_food(s, "BLUEBERRIES");
    *ic = has_food(s, "ICE_CREAM");
}

static void emit(Pt p) {
    if (p.x < 0) puts("WAIT");
    else printf("USE %d %d\n", p.x, p.y);
}

int main(void) {
    char *p = next_line();
    if (!p) return 0;
    int nall = atoi(p);
    for (int i = 0; i < nall; i++) next_line();
    for (int y = 0; y < 7; y++) {
        p = next_line();
        if (!p) return 0;
        for (int x = 0; x < 11; x++) {
            char c = p[x];
            Pt q = {x, y};
            switch (c) {
                case 'D': if (D.x < 0) D = q; break;
                case 'W': if (W.x < 0) W = q; break;
                case 'B': if (B.x < 0) B = q; break;
                case 'I': if (I.x < 0) I = q; break;
                case '#': if (ntables < MAXT) tables[ntables++] = q; break;
                default: break;
            }
        }
    }

    while ((p = next_line()) != NULL) {
        if (!*p) continue;  /* tours restants */
        char playerline[512];
        char *pl = next_line();
        if (!pl) break;
        strcpy(playerline, pl);
        char hx[NAMELEN], hy[NAMELEN], held[NAMELEN];
        next_line();  /* partenaire */
        int nt = atoi(next_line());
        static Pt itempos[MAXT];
        static char itemname[MAXT][NAMELEN];
        for (int i = 0; i < nt && i < MAXT; i++) {
            int x, y;
            char name[NAMELEN];
            if (sscanf(next_line(), "%d %d %63s", &x, &y, name) != 3) return 0;
            itempos[i] = (Pt){x, y};
            strcpy(itemname[i], name);
        }
        next_line();  /* four */
        int nc = atoi(next_line());
        for (int i = 0; i < nc; i++) next_line();

        /* On relit la ligne joueur mémorisée plus haut. */
        if (sscanf(playerline, "%63s %63s %63s", hx, hy, held) != 3) continue;
        (void)hx; (void)hy;

        bool bb, ic;
        contents(held, &bb, &ic);
        Pt target = {-1, -1};
        if (is_dish(held) && bb && ic) {
            target = W;
        } else if (is_dish(held)) {
            int missing_bb = !bb, missing_ic = !ic;
            for (int i = 0; i < nt && i < MAXT && target.x < 0; i++)
                if (missing_bb && strcmp(itemname[i], "BLUEBERRIES") == 0) target = itempos[i];
            for (int i = 0; i < nt && i < MAXT && target.x < 0; i++)
                if (missing_ic && strcmp(itemname[i], "ICE_CREAM") == 0) target = itempos[i];
            if (target.x < 0) target = missing_bb ? B : I;
        } else if (strcmp(held, "BLUEBERRIES") == 0 || strcmp(held, "ICE_CREAM") == 0) {
            target = D;
        } else {
            /* Plat complet sur table, puis n'importe quel plat. */
            for (int i = 0; i < nt && i < MAXT && target.x < 0; i++) {
                bool a, b;
                contents(itemname[i], &a, &b);
                if (is_dish(itemname[i]) && a && b) target = itempos[i];
            }
            for (int i = 0; i < nt && i < MAXT && target.x < 0; i++)
                if (is_dish(itemname[i])) target = itempos[i];
            for (int i = 0; i < nt && i < MAXT && target.x < 0; i++)
                if (strcmp(itemname[i], "BLUEBERRIES") == 0 ||
                    strcmp(itemname[i], "ICE_CREAM") == 0) target = itempos[i];
            if (target.x < 0) target = D;
        }
        emit(target);
        fflush(stdout);
    }
    return 0;
}
