/* Wondev Woman — arbitre version 2 (2 unités, PUSH&BUILD possible).
 *
 * Protocole vérifié sur le stub officiel et des test games :
 *   init  : size, unitsPerPlayer (= 2 ici)
 *   tour  : size lignes de grille ('.' = trou, '0'..'3' = hauteur),
 *           unitsPerPlayer lignes "x y" pour nous, autant pour l'adversaire,
 *           puis legalActions, puis "atype index dir1 dir2".
 *   sortie : UNE ligne seulement (le referee ne lit que outputs[0]).
 *
 * Marcher sur une case de hauteur 3 marque 1 point. On crée des tremplins
 * 2 -> 3 à côté de notre unité et on détruit ceux de l'adversaire.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 32
#define MAX_ACTIONS 512
#define MAX_TOK 64
#define MAX_LINE 256

static const char *const DIR_NAME[8] = {
    "N", "NE", "E", "SE", "S", "SW", "W", "NW"};
static const int DX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
static const int DY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

static int dir_index(const char *s) {
    for (int i = 0; i < 8; ++i)
        if (strcmp(s, DIR_NAME[i]) == 0) return i;
    return 0;
}

static int grid[MAX_SIZE][MAX_SIZE];

static int height_at(int x, int y, int size) {
    if (x < 0 || y < 0 || x >= size || y >= size) return -1;
    return grid[y][x];
}

int main(void) {
    int size = 0, upp = 0;
    if (scanf("%d %d", &size, &upp) != 2) return 0;

    char line[MAX_LINE];
    int pos[MAX_TOK][2];
    int foe[MAX_TOK][2];

    while (1) {
        for (int y = 0; y < size; ++y) {
            char row[MAX_LINE];
            if (scanf("%s", row) != 1) return 0;
            for (int x = 0; x < size; ++x)
                grid[y][x] = (row[x] == '.') ? -1 : (row[x] - '0');
        }
        for (int i = 0; i < upp; ++i)
            if (scanf("%d %d", &pos[i][0], &pos[i][1]) != 2) return 0;
        for (int i = 0; i < upp; ++i)
            if (scanf("%d %d", &foe[i][0], &foe[i][1]) != 2) return 0;

        int n = 0;
        if (scanf("%d", &n) != 1) return 0;
        if (n > MAX_ACTIONS) n = MAX_ACTIONS;

        long long best_score = 0;
        char best_token[MAX_LINE] = "";
        int have = 0;

        for (int i = 0; i < n; ++i) {
            char atype[MAX_TOK], d1[MAX_TOK], d2[MAX_TOK];
            int index = 0;
            if (scanf("%31s %d %31s %31s", atype, &index, d1, d2) != 4) return 0;
            if (index < 0 || index >= upp) continue;

            int a = dir_index(d1), b = dir_index(d2);
            int ux = pos[index][0], uy = pos[index][1];
            int mx = ux + DX[a], my = uy + DY[a];
            int h_dest = height_at(mx, my, size);
            int cx = mx + DX[b], cy = my + DY[b];
            int h_build = height_at(cx, cy, size);

            long long score = 0;
            if (h_dest == 3) {
                score += 1000000;
            } else if (h_dest >= 0) {
                score += 400 * h_dest;
            }
            if (h_dest >= 0) {
                for (int k = 0; k < 8; ++k)
                    if (height_at(mx + DX[k], my + DY[k], size) == 3) {
                        score += 1500;
                        break;
                    }
            }
            if (h_build == 3) {
                int near = 0;
                for (int k = 0; k < upp; ++k)
                    if (abs(cx - foe[k][0]) <= 1 && abs(cy - foe[k][1]) <= 1) {
                        near = 1;
                        break;
                    }
                score += near ? 50000 : -40;
            } else if (h_build == 2) {
                score += (abs(cx - mx) + abs(cy - my) <= 1) ? 3000 : 500;
            } else if (h_build == 1) {
                score += 100;
            } else if (h_build == 0) {
                score += 10;
            }

            /* Première action légale gagne une égalité (identique au Python). */
            if (!have || score > best_score) {
                have = 1;
                best_score = score;
                snprintf(best_token, sizeof(best_token), "%s %d %s %s",
                         atype, index, DIR_NAME[a], DIR_NAME[b]);
            }
        }
        (void)line;
        printf("%s\n", have ? best_token : "ACCEPT-DEFEAT");
        fflush(stdout);
    }
    return 0;
}
