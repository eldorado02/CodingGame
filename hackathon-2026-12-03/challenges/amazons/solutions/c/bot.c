/* Amazons — C port of solutions/python/bot.py (v1 two-stage territory search).
 * Eval = queen-distance territory + MOBILITY_WEIGHT * mobility difference.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 10
#define MAX_CELLS (MAX_N * MAX_N)
#define MAX_RANKED 512
enum { TOP_MOVES = 10, MOBILITY_WEIGHT = 1 };
static const int DX[8] = {-1, 0, 1, -1, 1, -1, 0, 1}, DY[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
#define EMPTY '.'
#define WALL '-'

static int n = 8;
static int ray[MAX_CELLS][8][MAX_N], ray_len[MAX_CELLS][8];

static void init_board(int size) {
    n = size;
    for (int i = 0; i < n * n; ++i) {
        int x = i % n, y = i / n;
        for (int d = 0; d < 8; ++d) {
            int k = 0;
            for (int cx = x + DX[d], cy = y + DY[d]; cx >= 0 && cx < n && cy >= 0 && cy < n; cx += DX[d], cy += DY[d])
                ray[i][d][k++] = cy * n + cx;
            ray_len[i][d] = k;
        }
    }
}

static int reach(const char *cells, int start, int *out) {
    int k = 0;
    for (int d = 0; d < 8; ++d)
        for (int j = 0; j < ray_len[start][d]; ++j) {
            int t = ray[start][d][j];
            if (cells[t] != EMPTY) break;
            out[k++] = t;
        }
    return k;
}

static int pieces(const char *cells, char color, int *out) {
    int k = 0;
    for (int i = 0; i < n * n; ++i) if (cells[i] == color) out[k++] = i;
    return k;
}

static void queen_distance(const char *cells, const int *src, int ns, int *dist) {
    int frontier[MAX_CELLS], next[MAX_CELLS], nf = 0, buf[MAX_CELLS], d = 0;
    for (int i = 0; i < n * n; ++i) dist[i] = -1;
    for (int i = 0; i < ns; ++i) { dist[src[i]] = 0; frontier[nf++] = src[i]; }
    while (nf) {
        ++d;
        int nn = 0;
        for (int f = 0; f < nf; ++f) {
            int k = reach(cells, frontier[f], buf);
            for (int j = 0; j < k; ++j)
                if (dist[buf[j]] < 0) { dist[buf[j]] = d; next[nn++] = buf[j]; }
        }
        memcpy(frontier, next, sizeof(int) * (size_t)nn);
        nf = nn;
    }
}

static int evaluate(const char *cells, char me, char op) {
    int mine[MAX_CELLS] = {0}, theirs[MAX_CELLS] = {0}, dm[MAX_CELLS], dt[MAX_CELLS], buf[MAX_CELLS];
    int nm = pieces(cells, me, mine), nt = pieces(cells, op, theirs);
    queen_distance(cells, mine, nm, dm);
    queen_distance(cells, theirs, nt, dt);
    int territory = 0, mobility = 0;
    for (int i = 0; i < n * n; ++i) {
        if (cells[i] != EMPTY) continue;
        int a = dm[i], b = dt[i];
        if (a >= 0 && (b < 0 || a < b)) ++territory;
        else if (b >= 0 && (a < 0 || b < a)) --territory;
    }
    for (int i = 0; i < nm; ++i) mobility += reach(cells, mine[i], buf);
    for (int i = 0; i < nt; ++i) mobility -= reach(cells, theirs[i], buf);
    return territory + MOBILITY_WEIGHT * mobility;
}

typedef struct { int value, src, dst; } Ranked;

/* two stages: rank every queen move without arrow, keep the top_moves best
 * (stable insertion sort), then score every arrow of those moves */
static int best_action(char *cells, char me, char op, int top_moves, int *out) {
    static Ranked ranked[MAX_RANKED];
    int nr = 0, own[MAX_CELLS], moves[MAX_CELLS], arrows[MAX_CELLS];
    int np = pieces(cells, me, own);
    for (int p = 0; p < np; ++p) {
        int src = own[p], nm = reach(cells, src, moves);
        for (int a = 0; a < nm && nr < MAX_RANKED; ++a) {
            int dst = moves[a];
            cells[src] = EMPTY; cells[dst] = me;
            ranked[nr].value = evaluate(cells, me, op); ranked[nr].src = src; ranked[nr].dst = dst; ++nr;
            cells[src] = me; cells[dst] = EMPTY;
        }
    }
    for (int i = 1; i < nr; ++i) {
        Ranked x = ranked[i];
        int j = i;
        while (j > 0 && ranked[j - 1].value < x.value) { ranked[j] = ranked[j - 1]; --j; }
        ranked[j] = x;
    }
    int found = 0, best_v = 0;
    for (int r = 0; r < nr && r < top_moves; ++r) {
        int src = ranked[r].src, dst = ranked[r].dst;
        cells[src] = EMPTY; cells[dst] = me;
        int na = reach(cells, dst, arrows);
        for (int b = 0; b < na; ++b) {
            cells[arrows[b]] = WALL;
            int v = evaluate(cells, me, op);
            cells[arrows[b]] = EMPTY;
            if (!found || v > best_v) { found = 1; best_v = v; out[0] = src; out[1] = dst; out[2] = arrows[b]; }
        }
        cells[src] = me; cells[dst] = EMPTY;
    }
    return found;
}

static void print_square(int i) { printf("%c%d", 'a' + i % n, n - i / n); }

int main(void) {
    int top_moves = TOP_MOVES;
    const char *env = getenv("CG_NODES");
    if (env) top_moves = atoi(env);
    char line[256], cells[MAX_CELLS + 1];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int size = atoi(line);
    if (size < 1 || size > MAX_N) size = 8;
    init_board(size);
    while (fgets(line, sizeof line, stdin)) {
        char me = line[0] == 'b' ? 'b' : 'w', op = me == 'w' ? 'b' : 'w';
        for (int y = 0; y < n; ++y) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            size_t len = strcspn(line, "\r\n");
            for (int x = 0; x < n; ++x) cells[y * n + x] = (size_t)x < len ? line[x] : WALL;
        }
        cells[n * n] = 0;
        if (!fgets(line, sizeof line, stdin) || !fgets(line, sizeof line, stdin)) return 0;
        int a[3];
        if (!best_action(cells, me, op, top_moves, a)) { printf("random\n"); fflush(stdout); continue; }
        print_square(a[0]); print_square(a[1]); print_square(a[2]);
        printf("\n");
        fflush(stdout);
    }
    return 0;
}
