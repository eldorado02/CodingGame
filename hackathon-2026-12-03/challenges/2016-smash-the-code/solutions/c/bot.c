/* Smash The Code — solution C (Wood league).
 * Grille 6x12, paires verticales, groupes de 4+, cranes, gravite et chaines. */
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define W 6
#define H 12
#define N (H * W)

typedef struct {
    char cell[H][W];
} Grid;

static const int DR[4] = {1, -1, 0, 0};
static const int DC[4] = {0, 0, 1, -1};

/* Renvoie false si la colonne deborde (top out). */
static bool drop(Grid *g, int col, char a, char b) {
    int row = 0;
    while (row < H && g->cell[row][col] == '.') row++;
    if (row < 2) return false;
    g->cell[row - 2][col] = a;
    g->cell[row - 1][col] = b;
    return true;
}

static void gravity(Grid *g) {
    for (int c = 0; c < W; c++) {
        char stack[H];
        int n = 0;
        for (int r = H - 1; r >= 0; r--) {
            if (g->cell[r][c] != '.') stack[n++] = g->cell[r][c];
        }
        int k = 0;
        for (int r = H - 1; r >= 0 && k < n; r--) g->cell[r][c] = stack[k++];
        for (int r = H - 1 - n; r >= 0; r--) g->cell[r][c] = '.';
    }
}

/* Efface les groupes, applique la gravite, recommence. Renvoie (blocs, chaines). */
static void resolve(Grid *g, int *total, int *chains) {
    *total = 0;
    *chains = 0;
    for (;;) {
        bool visited[H][W] = {false};
        bool clear[H][W] = {false};
        bool found = false;
        int qr[N], qc[N];

        for (int r = 0; r < H; r++) {
            for (int c = 0; c < W; c++) {
                char colour = g->cell[r][c];
                if (colour == '.' || colour == '0' || visited[r][c]) continue;
                int head = 0, tail = 0, size = 0;
                qr[tail] = r; qc[tail] = c; tail++;
                visited[r][c] = true;
                while (head < tail) {
                    int cr = qr[head], cc = qc[head]; head++;
                    size++;
                    for (int d = 0; d < 4; d++) {
                        int nr = cr + DR[d], nc = cc + DC[d];
                        if (nr >= 0 && nr < H && nc >= 0 && nc < W &&
                            !visited[nr][nc] && g->cell[nr][nc] == colour) {
                            visited[nr][nc] = true;
                            qr[tail] = nr; qc[tail] = nc; tail++;
                        }
                    }
                }
                if (size >= 4) {
                    found = true;
                    for (int i = 0; i < tail; i++) clear[qr[i]][qc[i]] = true;
                }
            }
        }
        if (!found) break;

        for (int r = 0; r < H; r++) {
            for (int c = 0; c < W; c++) {
                if (g->cell[r][c] != '0') continue;
                for (int d = 0; d < 4; d++) {
                    int nr = r + DR[d], nc = c + DC[d];
                    if (nr >= 0 && nr < H && nc >= 0 && nc < W && clear[nr][nc]) {
                        clear[r][c] = true;
                        break;
                    }
                }
            }
        }
        for (int r = 0; r < H; r++) {
            for (int c = 0; c < W; c++) {
                if (clear[r][c]) {
                    g->cell[r][c] = '.';
                    (*total)++;
                }
            }
        }
        (*chains)++;
        gravity(g);
    }
}

static int height(const Grid *g) {
    int best = 0;
    for (int c = 0; c < W; c++) {
        int h = 0;
        for (int r = 0; r < H; r++) if (g->cell[r][c] != '.') h++;
        if (h > best) best = h;
    }
    return best;
}

static int evaluate(const Grid *g, int col, char a, char b) {
    Grid trial = *g;
    if (!drop(&trial, col, a, b)) return INT_MIN;
    int cleared, chains;
    resolve(&trial, &cleared, &chains);
    return cleared * 100 + chains * 50 - height(&trial) * 2;
}

int main(void) {
    char line[64];
    for (;;) { /* une entree complete = un tour, puis on rejoue */
        /* Le nombre de paires varie (8 au debut, puis les restantes) : on lit
           jusqu'a la ligne de score, qui ne contient qu'un seul entier. */
        char first_a = 0, first_b = 0;
        for (;;) {
            if (fgets(line, sizeof line, stdin) == NULL) return 0;
            int ca, cb;
            if (sscanf(line, "%d %d", &ca, &cb) == 2) {
                if (first_a == 0) {
                    first_a = (char)('0' + ca);
                    first_b = (char)('0' + cb);
                }
                continue;
            }
            break; /* ligne de score consommee */
        }
        if (first_a == 0) return 0;

        Grid g;
        for (int r = 0; r < H; r++) {
            if (fgets(line, sizeof line, stdin) == NULL) return 0;
            for (int c = 0; c < W; c++) g.cell[r][c] = line[c];
        }
        if (fgets(line, sizeof line, stdin) == NULL) return 0; /* score adverse */
        for (int r = 0; r < H; r++) {
            if (fgets(line, sizeof line, stdin) == NULL) return 0; /* grille adverse */
        }

        int best_col = 0, best_score = INT_MIN;
        for (int c = 0; c < W; c++) {
            int s = evaluate(&g, c, first_a, first_b);
            if (s > best_score) { best_score = s; best_col = c; }
        }
        printf("%d\n", best_col);
        fflush(stdout);
    }
}
