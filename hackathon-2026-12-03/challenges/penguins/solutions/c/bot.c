/* Penguins — C port of solutions/python/bot.py (v2 greedy + mobility).
 * Rows 1, 3, ... (index 0, 2, ...) are the narrow shifted rows; see step().
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 16
#define MAX_COLS 16
#define MAX_PENGUINS 16
enum { SOURCE_WEIGHT = 10, MOBILITY_WEIGHT = 1 };

static char board[MAX_ROWS][MAX_COLS + 1];
static int rows = 0;

/* neighbour in direction d (0 E, 1 W, 2 NE, 3 NW, 4 SE, 5 SW) */
static void step(int r, int x, int d, int *nr, int *nx) {
    int narrow = r % 2 == 0;
    if (d == 0) { *nr = r; *nx = x + 1; return; }
    if (d == 1) { *nr = r; *nx = x - 1; return; }
    int dr = (d == 2 || d == 3) ? 1 : -1, right = d == 2 || d == 4;
    *nr = r + dr;
    *nx = narrow ? (right ? x + 1 : x) : (right ? x : x - 1);
}

static int blocked_at(int (*blocked)[2], int nb, int r, int x) {
    for (int i = 0; i < nb; ++i) if (blocked[i][0] == r && blocked[i][1] == x) return 1;
    return 0;
}

static int is_ice(int (*blocked)[2], int nb, int r, int x) {
    if (r < 0 || r >= rows || x < 0 || x >= (int)strlen(board[r])) return 0;
    char c = board[r][x];
    return isdigit((unsigned char)c) && c != '0' && !blocked_at(blocked, nb, r, x);
}

static int mobility(int (*blocked)[2], int nb, int r, int x) {
    int n = 0;
    for (int d = 0; d < 6; ++d) {
        int cr, cx;
        step(r, x, d, &cr, &cx);
        while (is_ice(blocked, nb, cr, cx)) { ++n; step(cr, cx, d, &cr, &cx); }
    }
    return n;
}

static int parse_pos(const char *pos, int *r, int *x) {
    size_t len = strlen(pos);
    if (len < 2 || !isalpha((unsigned char)pos[0])) return 0;
    for (size_t i = 1; i < len; ++i) if (!isdigit((unsigned char)pos[i])) return 0;
    *r = atoi(pos + 1) - 1;
    *x = toupper((unsigned char)pos[0]) - 'A';
    return 1;
}

static int fish_at(int r, int x) {
    if (r < 0 || r >= rows || x < 0 || x >= (int)strlen(board[r]) || !isdigit((unsigned char)board[r][x])) return -1;
    return board[r][x] - '0';
}

static char mine_id[MAX_PENGUINS][16];
static int mine_r[MAX_PENGUINS], mine_x[MAX_PENGUINS], mine_count = 0;
static int occupied[MAX_PENGUINS * 4][2], occupied_count = 0;

static int score(const char *action) {
    char parts[8][32];
    int np = 0;
    const char *p = action;
    while (np < 8 && sscanf(p, "%31s", parts[np]) == 1) {
        p = strstr(p, parts[np]) + strlen(parts[np]);
        ++np;
    }
    int dr, dx;
    if (np == 0 || !parse_pos(parts[np - 1], &dr, &dx)) return -1;
    int value = fish_at(dr, dx), blocked[MAX_PENGUINS * 4 + 1][2], nb = 0;
    int src = -1;
    if (np >= 3)
        for (int i = 0; i < mine_count; ++i) if (strcmp(mine_id[i], parts[1]) == 0) { src = i; break; }
    for (int i = 0; i < occupied_count; ++i) {
        if (src >= 0 && occupied[i][0] == mine_r[src] && occupied[i][1] == mine_x[src]) continue;
        if (occupied[i][0] == dr && occupied[i][1] == dx) continue;
        blocked[nb][0] = occupied[i][0]; blocked[nb][1] = occupied[i][1]; ++nb;
    }
    char saved = 0;
    if (src >= 0) {
        int f = fish_at(mine_r[src], mine_x[src]);
        value += SOURCE_WEIGHT * (f > 0 ? f : 0);
        if (f >= 0) { saved = board[mine_r[src]][mine_x[src]]; board[mine_r[src]][mine_x[src]] = '0'; }
    }
    value += MOBILITY_WEIGHT * mobility(blocked, nb, dr, dx);
    if (saved) board[mine_r[src]][mine_x[src]] = saved;
    return value;
}

static void trim(char *s) {
    size_t len = strlen(s);
    while (len && isspace((unsigned char)s[len - 1])) s[--len] = 0;
    size_t a = 0;
    while (s[a] && isspace((unsigned char)s[a])) ++a;
    if (a) memmove(s, s + a, len - a + 1);
}

int main(void) {
    char line[256], me[16] = {0}, best[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int players = atoi(line);
    if (!fgets(line, sizeof line, stdin)) return 0;
    trim(line);
    snprintf(me, sizeof me, "%s", line);
    if (!fgets(line, sizeof line, stdin)) return 0;
    while (fgets(line, sizeof line, stdin)) {
        rows = atoi(line);
        if (rows > MAX_ROWS) rows = MAX_ROWS;
        for (int i = 0; i < rows; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            trim(line);
            snprintf(board[i], sizeof board[i], "%s", line);
        }
        for (int i = 0; i < players; ++i) if (!fgets(line, sizeof line, stdin)) return 0;
        if (!fgets(line, sizeof line, stdin)) return 0;
        int count = atoi(line);
        mine_count = occupied_count = 0;
        for (int i = 0; i < count; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            char id[16], owner[16], pos[16];
            int r, x;
            if (sscanf(line, "%15s %15s %15s", id, owner, pos) == 3 && parse_pos(pos, &r, &x) && occupied_count < MAX_PENGUINS * 4) {
                occupied[occupied_count][0] = r; occupied[occupied_count][1] = x; ++occupied_count;
                if (strcmp(owner, me) == 0 && mine_count < MAX_PENGUINS) {
                    snprintf(mine_id[mine_count], sizeof mine_id[mine_count], "%s", id);
                    mine_r[mine_count] = r; mine_x[mine_count] = x; ++mine_count;
                }
            }
        }
        if (!fgets(line, sizeof line, stdin)) return 0;
        int actions = atoi(line), best_v = -2;
        best[0] = 0;
        for (int i = 0; i < actions; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            trim(line);
            int v = line[0] ? score(line) : -2;
            if (v > best_v) { best_v = v; snprintf(best, sizeof best, "%s", line); }
        }
        printf("%s\n", best[0] ? best : "MOVE 0 A1");
        fflush(stdout);
    }
    return 0;
}
