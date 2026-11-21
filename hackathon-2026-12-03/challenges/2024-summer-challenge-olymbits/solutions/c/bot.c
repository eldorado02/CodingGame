/* Olymbits (Wood 1, 4 courses) — énumération des 4^5 séquences d'actions.
   Toutes les machines reçoivent la même action ; on minimise la somme des
   tours restants estimés sur toutes les courses simultaneously actives. */
#include <stdio.h>
#include <string.h>

#define MAX_GAMES 4
#define MAX_TRACK 64
#define HORIZON 5
#define NSEQ 1024 /* 4^5 */

static const char *const NAMES[4] = {"LEFT", "DOWN", "RIGHT", "UP"};
static const int STEPS[4] = {1, 2, 3, 2};
static const int JUMP[4] = {0, 0, 0, 1};
static const int POW4[5] = {1, 4, 16, 64, 256};

static void step(const char *track, int last, int *pos, int *stun, int move) {
    /* Les registres des mini-jeux ne sont pas toujours des positions :
       l'archerie stocke x/-y, qui peut être négatif. */
    if (*pos < 0) *pos = 0;
    if (*stun > 0) {
        (*stun)--;
        return;
    }
    int steps = STEPS[move], jump = JUMP[move];
    if (jump) {
        int np = *pos + 2;
        if (np > last) np = last;
        *stun = (*pos + 2 < last && track[*pos + 2] == '#') ? 3 : 0;
        *pos = np;
        return;
    }
    for (int s = 1; s <= steps; ++s) {
        if (*pos + s > last) {
            *pos = last;
            *stun = 0;
            return;
        }
        if (track[*pos + s] == '#') {
            *pos += s;
            *stun = 3;
            return;
        }
    }
    *pos += steps;
    *stun = 0;
}

int main(void) {
    char line[512];
    int player = 0, games = 0;
    if (!fgets(line, sizeof(line), stdin)) return 0;
    sscanf(line, "%d", &player);
    if (!fgets(line, sizeof(line), stdin)) return 0;
    sscanf(line, "%d", &games);
    if (games > MAX_GAMES) games = MAX_GAMES;

    for (;;) {
        int alive = 1;
        for (int i = 0; i < 3 && alive; ++i)
            alive = fgets(line, sizeof(line), stdin) != NULL;
        if (!alive) break;

        char tracks[MAX_GAMES][MAX_TRACK];
        int p0[MAX_GAMES], s0[MAX_GAMES], last[MAX_GAMES];
        int valid[MAX_GAMES] = {0};
        for (int g = 0; g < games; ++g) {
            if (!fgets(line, sizeof(line), stdin)) continue;
            char gpu[MAX_TRACK];
            int reg[7] = {0};
            int n = sscanf(line, "%63s %d %d %d %d %d %d %d", gpu,
                           &reg[0], &reg[1], &reg[2], &reg[3], &reg[4],
                           &reg[5], &reg[6]);
            if (n >= 8 && strcmp(gpu, "GAME_OVER") != 0) {
                size_t glen = strlen(gpu);
                if (glen >= sizeof(tracks[g])) glen = sizeof(tracks[g]) - 1;
                memcpy(tracks[g], gpu, glen);
                tracks[g][glen] = '\0';
                p0[g] = reg[player];
                s0[g] = reg[3 + player];
                last[g] = (int)strlen(tracks[g]) - 1;
                valid[g] = 1;
            }
        }

        int best = -1, best_first = 0;
        for (int code = 0; code < NSEQ; ++code) {
            int total = 0;
            for (int g = 0; g < games; ++g) {
                if (!valid[g]) continue;
                int pos = p0[g], stun = s0[g], done = -1;
                for (int t = 0; t < HORIZON; ++t) {
                    int m = (code / POW4[HORIZON - 1 - t]) % 4;
                    step(tracks[g], last[g], &pos, &stun, m);
                    if (pos >= last[g]) {
                        done = t + 1;
                        break;
                    }
                }
                if (done < 0) done = HORIZON + (last[g] - pos);
                total += done;
            }
            if (best < 0 || total < best) {
                best = total;
                best_first = (code / POW4[HORIZON - 1]) % 4;
                if (best == 0) break;
            }
        }
        printf("%s\n", NAMES[best_first]);
        fflush(stdout);
    }
    return 0;
}
