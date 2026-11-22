/* Ataxx bot — deterministic 1-ply greedy (port of solutions/python/bot.py).
 * Build: gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o bot_c
 * The referee provides the legal actions; we only simulate and score them.
 */
#include <stdio.h>
#include <string.h>

#define N 49
#define MAX_ACTIONS 512
#define WIN_SCORE 1000000
#define PIECE_WEIGHT 20

/* Center-control weights (doubly symmetric matrix, row 0 = bottom). */
static const int WEIGHTS[N] = {
    1, 2, 2, 3, 2, 2, 1,
    2, 3, 4, 4, 4, 3, 2,
    2, 4, 5, 5, 5, 4, 2,
    3, 4, 5, 6, 5, 4, 3,
    2, 4, 5, 5, 5, 4, 2,
    2, 3, 4, 4, 4, 3, 2,
    1, 2, 2, 3, 2, 2, 1
};

static int neigh[N][8];
static int neigh_count[N];

static void init_neighbors(void) {
    for (int y = 0; y < 7; y++) {
        for (int x = 0; x < 7; x++) {
            int i = y * 7 + x;
            neigh_count[i] = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < 7 && ny >= 0 && ny < 7) {
                        neigh[i][neigh_count[i]++] = ny * 7 + nx;
                    }
                }
            }
        }
    }
}

static int char_to_cell(char ch) {
    if (ch == '1') return 2;
    if (ch == '0') return 1;
    if (ch == '#') return 3;
    return 0;
}

static int clamp7(int v) {
    if (v < 0) return 0;
    if (v > 6) return 6;
    return v;
}

/* Parse an action: returns 1 for jump, 0 for spawn. */
static int parse_action(const char *s, int *x0, int *y0, int *x1, int *y1) {
    if (s == NULL || strlen(s) < 2) {
        *x0 = *y0 = *x1 = *y1 = 0;
        return 0;
    }
    *x0 = clamp7(s[0] - 'a');
    /* Le plateau est reçu du haut vers le bas : '1' -> 6, '7' -> 0. */
    *y0 = clamp7('7' - s[1]);
    if (strlen(s) >= 4) {
        *x1 = clamp7(s[2] - 'a');
        *y1 = clamp7('7' - s[3]);
        return 1;
    }
    *x1 = *x0;
    *y1 = *y0;
    return 0;
}

/* Apply one action to a copy of the board. */
static void simulate(const int *cells, const char *action, int me, int opp,
                     int *out) {
    memcpy(out, cells, N * sizeof(int));
    int x0, y0, x1, y1;
    int is_jump = parse_action(action, &x0, &y0, &x1, &y1);
    int dst = y1 * 7 + x1;
    if (is_jump) {
        out[y0 * 7 + x0] = 0; /* source vacated by a jump */
    }
    out[dst] = me;
    for (int k = 0; k < neigh_count[dst]; k++) {
        int nb = neigh[dst][k];
        if (out[nb] == opp) out[nb] = me;
    }
}

static int evaluate(const int *board, int me, int opp) {
    int my_count = 0, opp_count = 0, score = 0;
    for (int i = 0; i < N; i++) {
        int cell = board[i];
        if (cell == me) {
            my_count++;
            int s = WEIGHTS[i];
            for (int k = 0; k < neigh_count[i]; k++) {
                if (board[neigh[i][k]] == opp) s++;
            }
            score += s;
        } else if (cell == opp) {
            opp_count++;
            int s = WEIGHTS[i];
            for (int k = 0; k < neigh_count[i]; k++) {
                if (board[neigh[i][k]] == me) s++;
            }
            score -= s;
        }
    }
    if (my_count == 0) return -WIN_SCORE;
    if (opp_count == 0) return WIN_SCORE;
    return score + PIECE_WEIGHT * (my_count - opp_count);
}

/* Best action: max eval, then smallest ASCII string. */
static void choose(const int *cells, int me, const char actions[MAX_ACTIONS][16],
                   int count, char *best) {
    int opp = 3 - me;
    best[0] = '\0';
    int best_eval = 0;
    int have = 0;
    int tmp[N];
    for (int a = 0; a < count; a++) {
        const char *action = actions[a];
        if (action[0] == '\0') continue;
        simulate(cells, action, me, opp, tmp);
        int val = evaluate(tmp, me, opp);
        if (!have || val > best_eval ||
            (val == best_eval && strcmp(action, best) < 0)) {
            best_eval = val;
            strcpy(best, action);
            have = 1;
        }
    }
    if (!have) strcpy(best, "random");
}

int main(void) {
    init_neighbors();
    int id;
    if (scanf("%d", &id) != 1) return 0;
    int me = id + 1;
    if (me != 1 && me != 2) me = 1;

    char row[16], last[64], act[16];
    while (1) {
        int cells[N];
        int complete = 1;
        for (int y = 0; y < 7; y++) {
            if (scanf("%15s", row) != 1) { complete = 0; break; }
            for (int x = 0; x < 7; x++) {
                char ch = (x < (int)strlen(row)) ? row[x] : '.';
                cells[y * 7 + x] = char_to_cell(ch);
            }
        }
        if (!complete) break;
        if (scanf("%63s", last) != 1) break;       /* lastAction */
        int count;
        if (scanf("%d", &count) != 1) break;
        if (count < 0) count = 0;
        if (count > MAX_ACTIONS) count = MAX_ACTIONS;

        static char actions[MAX_ACTIONS][16];
        for (int i = 0; i < count; i++) {
            if (scanf("%15s", act) != 1) { count = i; break; }
            strcpy(actions[i], act); /* act is at most 15 chars via %15s */
        }
        char best[16];
        choose(cells, me, (const char (*)[16])actions, count, best);
        printf("%s\n", best);
        fflush(stdout);
    }
    return 0;
}
