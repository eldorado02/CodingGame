/* Impasse — C port of solutions/python/bot.py (v1 greedy). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_coords(const char *t, int *col, int *row) {
    size_t len = strlen(t);
    if (len == 0 || len % 2 || len > 16) return 0;
    int n = 0;
    for (size_t i = 0; i < len; i += 2) {
        char c = t[i], r = t[i + 1];
        if (c < 'a' || c > 'h' || r < '1' || r > '8') return 0;
        col[n] = c - 'a'; row[n] = r - '0'; ++n;
    }
    return n;
}

static int score(char board[8][9], char me, const int *col, const int *row, int n) {
    if (n == 1) return 50;
    int up = me == 'w', near_r = up ? 1 : 8, far_r = up ? 8 : 1;
    char src = board[8 - row[0]][col[0]], dst = board[8 - row[1]][col[1]];
    char me_u = up ? 'W' : 'B';
    int sc = 0, rows = abs(row[1] - row[0]);
    if (src == me_u) {
        if (dst == me) sc += 12 + (row[1] == near_r ? 20 : 0);
        else sc += 3 * rows + (row[1] == near_r ? 25 : 0);
    } else {
        sc += 3 * rows + (row[1] == far_r ? 8 : 0);
    }
    if (n >= 3) sc += 2;
    return sc;
}

int main(void) {
    char line[256], board[8][9], best[64];
    if (!fgets(line, sizeof line, stdin)) return 0;
    char me = 'w';
    for (char *p = line; *p; ++p) if (*p != ' ') { me = (*p == 'b' || *p == 'B') ? 'b' : 'w'; break; }
    for (;;) {
        for (int y = 0; y < 8; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < 8; ++x) board[y][x] = (size_t)x < len ? line[x] : '.';
            board[y][8] = 0;
        }
        if (!fgets(line, sizeof line, stdin)) return 0;
        if (!fgets(line, sizeof line, stdin)) return 0;
        int n = atoi(line), best_v = -1, col[8], row[8];
        best[0] = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char tok[64] = {0};
            if (sscanf(line, "%63s", tok) != 1) continue;
            int k = parse_coords(tok, col, row);
            if (!k) continue;
            int v = score(board, me, col, row, k);
            if (v > best_v) { best_v = v; strcpy(best, tok); }
        }
        printf("%s\n", best[0] ? best : "random");
        fflush(stdout);
    }
}
