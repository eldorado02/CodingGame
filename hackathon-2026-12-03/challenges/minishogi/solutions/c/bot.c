/* Minishogi 5x5 — C port of solutions/python/bot.py (v1 greedy). */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DICE
#define DICE 0
#endif

/* cell: kind char ('.', 'K','R','B','G','S','P'), promoted flag, owner */
typedef struct { char k; int prom; int own; } Cell;
static const int ORTH[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
static const int DIAG[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
static const int GOLD[6][2] = {{0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0}, {0, 1}};
static const int SILVER[5][2] = {{0, -1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

static int val(char k, int prom) {
    if (prom) return k == 'R' ? 11 : k == 'B' ? 10 : (k == 'S' || k == 'P') ? 6 : 0;
    switch (k) { case 'R': return 9; case 'B': return 8; case 'G': return 6; case 'S': return 5; case 'P': return 1; }
    return 0;
}

static void trim(char *s) {
    size_t len = strlen(s);
    while (len && isspace((unsigned char)s[len - 1])) s[--len] = 0;
    size_t a = 0;
    while (s[a] && isspace((unsigned char)s[a])) ++a;
    if (a) memmove(s, s + a, len - a + 1);
}

static void parse_row(const char *s, Cell *row) {
    int n = 0;
    size_t i = 0, len = strlen(s);
    while (i < len && n < 5) {
        Cell c = {'.', 0, -1};
        char ch = s[i];
        if (ch == '+' && i + 1 < len) { c.prom = 1; ch = s[i + 1]; i += 2; }
        else ++i;
        if (ch != '.') { c.k = (char)toupper((unsigned char)ch); c.own = isupper((unsigned char)ch) ? 0 : 1; }
        row[n++] = c;
    }
    while (n < 5) { Cell c = {'.', 0, -1}; row[n++] = c; }
}

static void attacks(Cell b[5][5], int side, int att[5][5]) {
    memset(att, 0, sizeof(int) * 25);
    int fwd = side == 0 ? -1 : 1;
    for (int y = 0; y < 5; ++y) for (int x = 0; x < 5; ++x) {
        Cell c = b[y][x];
        if (c.own != side) continue;
        int steps[8][2], ns = 0, slides[4][2], nsl = 0;
        #define ADD(arr, n, src, cnt) for (int q = 0; q < (cnt); ++q) { arr[n][0] = src[q][0]; arr[n][1] = src[q][1]; ++n; }
        if (c.k == 'K') { ADD(steps, ns, ORTH, 4); ADD(steps, ns, DIAG, 4); }
        else if (c.k == 'G' || (c.prom && (c.k == 'S' || c.k == 'P'))) { ADD(steps, ns, GOLD, 6); }
        else if (c.k == 'S') { ADD(steps, ns, SILVER, 5); }
        else if (c.k == 'P') { steps[0][0] = 0; steps[0][1] = -1; ns = 1; }
        else if (c.k == 'R') { ADD(slides, nsl, ORTH, 4); if (c.prom) { ADD(steps, ns, DIAG, 4); } }
        else if (c.k == 'B') { ADD(slides, nsl, DIAG, 4); if (c.prom) { ADD(steps, ns, ORTH, 4); } }
        for (int i = 0; i < ns; ++i) {
            int nx = x + steps[i][0], ny = y + steps[i][1] * fwd;
            if (nx >= 0 && nx < 5 && ny >= 0 && ny < 5) att[ny][nx] = 1;
        }
        for (int i = 0; i < nsl; ++i) {
            int nx = x + slides[i][0], ny = y + slides[i][1];
            while (nx >= 0 && nx < 5 && ny >= 0 && ny < 5) {
                att[ny][nx] = 1;
                if (b[ny][nx].k != '.') break;
                nx += slides[i][0]; ny += slides[i][1];
            }
        }
    }
}

static int sq(char c, char r, int *x, int *y) {
    if (c < '1' || c > '5' || r < '1' || r > '5') return 0;
    *x = 5 - (c - '0'); *y = (r - '0') - 1;
    return 1;
}

static int score(Cell b[5][5], int me, int att[5][5], const char *a, int *out) {
    int x, y, sx, sy;
    size_t len = strlen(a);
    if (len >= 4 && a[1] == '*') {
        if (!sq(a[2], a[3], &x, &y)) return 0;
        *out = att[y][x] ? -10 * val((char)toupper((unsigned char)a[0]), 0) : 0;
        return 1;
    }
    if (len < 4) return 0;
    if (!sq(a[0], a[1], &sx, &sy) || !sq(a[2], a[3], &x, &y)) return 0;
    Cell mover = b[sy][sx], target = b[y][x];
    int sc = 0;
    if (target.k != '.' && target.own != me) sc += 10 * val(target.k, target.prom);
    if (a[len - 1] == '+') sc += 4;
    if (mover.k == 'K' && !mover.prom) sc -= 1;
    else if (att[y][x]) sc -= 10 * val(mover.k, mover.prom);
    *out = sc;
    return 1;
}

int main(void) {
    char line[512], best[64];
    if (!fgets(line, sizeof line, stdin)) return 0;
    trim(line);
    int me = strcmp(line, "1") == 0 ? 1 : 0;
    if (!fgets(line, sizeof line, stdin)) return 0;
    trim(line);
    int n = isdigit((unsigned char)line[0]) ? atoi(line) : 5;
    if (n > 5) n = 5;
    for (;;) {
        Cell b[5][5];
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            trim(line);
            parse_row(line, b[i]);
        }
        for (int i = 0; i < 3 + DICE; ++i) if (!fgets(line, sizeof line, stdin)) return 0;
        if (!fgets(line, sizeof line, stdin)) return 0;
        int c = atoi(line), have = 0, best_v = 0, att[5][5];
        attacks(b, 1 - me, att);
        for (int i = 0; i < c; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char tok[64] = {0};
            if (sscanf(line, "%63s", tok) != 1) continue;
            int v;
            if (score(b, me, att, tok, &v) && (!have || v > best_v)) { have = 1; best_v = v; strcpy(best, tok); }
        }
        printf("%s\n", have ? best : "random");
        fflush(stdout);
    }
}
