/* Xmas Rush Bronze — poussées locales, distance de Manhattan atteignable, anti-stale. */
/* Port de xmas_v5.py. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 7
#define CELLS (N * N)
#define MAXSTEP 20
#define MAXT 12
#define MASKS (1 << MAXT)
#define STATES (CELLS * MASKS)
#define NAMELEN 64

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};
static const char *DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

typedef struct { int x, y; } Pos;

static int mask_of(const char *s) {
    int m = 0;
    for (int d = 0; d < 4; d++) if (s[d] == '1') m |= 1 << d;
    return m;
}
static int wrap(int v) { v %= N; return v < 0 ? v + N : v; }

static bool connected(const int g[N][N], int x, int y, int d) {
    if (!((g[y][x] >> d) & 1)) return false;
    int nx = x + DX[d], ny = y + DY[d];
    return nx >= 0 && nx < N && ny >= 0 && ny < N && ((g[ny][nx] >> ((d + 2) % 4)) & 1);
}

/* BFS : distance jusqu'à (tx,ty), 999 si inaccessible. */
static int bfs_dist(const int g[N][N], int sx, int sy, int tx, int ty) {
    if (sx == tx && sy == ty) return 0;
    int seen[CELLS];
    for (int i = 0; i < CELLS; i++) seen[i] = -1;
    int q[CELLS], head = 0, tail = 0;
    int si = sy * N + sx;
    seen[si] = 0;
    q[tail++] = si;
    while (head < tail) {
        int cur = q[head++];
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; d++) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx == tx && ny == ty) return seen[cur] + 1;
            int ni = ny * N + nx;
            if (seen[ni] == -1) { seen[ni] = seen[cur] + 1; q[tail++] = ni; }
        }
    }
    return 999;
}

/* BFS multi-quêtes : meilleur chemin (≤20 pas) collectant le plus de quêtes. */
typedef struct { short px, py; short pm; signed char d; } Par;
typedef struct { short x, y; unsigned short mask; short steps; } QState;
static Par g_parent[STATES];
static QState g_queue[STATES];

static int manhattan(int x, int y, const Pos *targets, int nt) {
    int best = 1000000;
    for (int i = 0; i < nt; i++) {
        int v = abs(x - targets[i].x) + abs(y - targets[i].y);
        if (v < best) best = v;
    }
    return best;
}

static int multi_bfs(const int g[N][N], int px, int py,
                     const Pos *targets, int nt, int out[MAXSTEP]) {
    if (nt == 0) return 0;
    memset(g_parent, 0xff, sizeof(g_parent));
    int head = 0, tail = 0;
    g_queue[tail++] = (QState){(short)px, (short)py, 0, 0};
    int si = (py * N + px) * MASKS;
    g_parent[si].px = px; g_parent[si].py = py; g_parent[si].pm = 0;
    g_parent[si].d = -2; /* racine : -1 = non visité */
    int bestGot = 0, bestSteps = 0, bestMD = manhattan(px, py, targets, nt);
    int bx = px, by = py, bm = 0;
    while (head < tail) {
        QState st = g_queue[head++];
        int md = manhattan(st.x, st.y, targets, nt);
        int got = 0;
        for (int i = 0; i < 16; i++) if ((st.mask >> i) & 1) got++;
        if (got > bestGot || (got == bestGot && -md > -bestMD) ||
            (got == bestGot && md == bestMD && -st.steps > -bestSteps)) {
            bestGot = got; bestSteps = st.steps; bestMD = md;
            bx = st.x; by = st.y; bm = st.mask;
        }
        if (st.steps >= MAXSTEP) continue;
        for (int d = 0; d < 4; d++) {
            if (!connected(g, st.x, st.y, d)) continue;
            int nx = st.x + DX[d], ny = st.y + DY[d];
            unsigned short nm = st.mask;
            for (int i = nt - 1; i >= 0; i--)
                if (targets[i].x == nx && targets[i].y == ny) {
                    nm |= (unsigned short)(1u << i);
                    break;
                }
            int idx = (ny * N + nx) * MASKS + nm;
            if (g_parent[idx].d != -1) continue;
            g_parent[idx].px = st.x; g_parent[idx].py = st.y; g_parent[idx].pm = st.mask;
            g_parent[idx].d = (signed char)d;
            g_queue[tail++] = (QState){(short)nx, (short)ny, nm, (short)(st.steps + 1)};
        }
    }
    int rev[MAXSTEP], n = 0;
    int x = bx, y = by, m = bm;
    while (n < MAXSTEP) {
        Par p = g_parent[(y * N + x) * MASKS + m];
        if (p.d < 0) break;
        rev[n++] = p.d;
        x = p.px; y = p.py; m = p.pm;
    }
    for (int i = 0; i < n; i++) out[i] = rev[n - 1 - i];
    return n;
}

/* Distance de Manhattan minimale atteignable en ≤20 pas. */
static int reachable_manhattan(const int g[N][N], int px, int py,
                               const Pos *targets, int nt) {
    if (nt == 0) return 0;
    int best = manhattan(px, py, targets, nt);
    if (best == 0) return 0;
    int seen[CELLS] = {0};
    int q[CELLS], head = 0, tail = 0;
    seen[py * N + px] = 1;
    q[tail++] = py * N + px;
    while (head < tail) {
        int cur = q[head++];
        int x = cur % N, y = cur / N;
        int steps = seen[cur] - 1;
        if (steps >= MAXSTEP) continue;
        for (int d = 0; d < 4; d++) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (seen[ni]) continue;
            seen[ni] = seen[cur] + 1;
            int md = manhattan(nx, ny, targets, nt);
            if (md < best) best = md;
            if (best == 0) return 0;
            q[tail++] = ni;
        }
    }
    return best;
}

/* Chemin BFS vers la quête la plus proche. */
static int target_path(const int g[N][N], int px, int py,
                       const Pos *targets, int nt, int out[MAXSTEP]) {
    int bd = 1000000, tx = -1, ty = -1;
    for (int i = 0; i < nt; i++) {
        int d = bfs_dist(g, px, py, targets[i].x, targets[i].y);
        if (d < 999 && d < bd) { bd = d; tx = targets[i].x; ty = targets[i].y; }
    }
    if (tx < 0 || bd == 0) return 0;
    int prev[CELLS];
    for (int i = 0; i < CELLS; i++) prev[i] = -2;
    int q[CELLS], head = 0, tail = 0;
    int si = py * N + px, ti = ty * N + tx;
    prev[si] = si;
    q[tail++] = si;
    while (head < tail) {
        int cur = q[head++];
        if (cur == ti) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; d++) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (prev[ni] != -2) continue;
            prev[ni] = cur;
            q[tail++] = ni;
        }
    }
    if (prev[ti] == -2) return 0;
    int rev[MAXSTEP], n = 0, cur = ti;
    while (cur != si && n < MAXSTEP) {
        int p = prev[cur];
        int x = cur % N, y = cur / N;
        int px2 = p % N, py2 = p / N;
        for (int d = 0; d < 4; d++)
            if (px2 + DX[d] == x && py2 + DY[d] == y) { rev[n++] = d; break; }
        cur = p;
    }
    for (int i = 0; i < n; i++) out[i] = rev[n - 1 - i];
    return n;
}

/* Chemin vers la tuile de bord la plus proche. */
static int edge_path(const int g[N][N], int px, int py, int out[MAXSTEP]) {
    Pos goals[4 * N];
    int ng = 0;
    for (int x = 0; x < N; x++) { goals[ng++] = (Pos){x, 0}; goals[ng++] = (Pos){x, N - 1}; }
    for (int y = 0; y < N; y++) { goals[ng++] = (Pos){0, y}; goals[ng++] = (Pos){N - 1, y}; }
    int bd = 1000000, tx = -1, ty = -1;
    for (int i = 0; i < ng; i++) {
        int d = bfs_dist(g, px, py, goals[i].x, goals[i].y);
        if (d < 999 && d < bd) { bd = d; tx = goals[i].x; ty = goals[i].y; }
    }
    if (tx < 0 || bd == 0) return 0;
    int prev[CELLS];
    for (int i = 0; i < CELLS; i++) prev[i] = -2;
    int q[CELLS], head = 0, tail = 0;
    int si = py * N + px, ti = ty * N + tx;
    prev[si] = si;
    q[tail++] = si;
    while (head < tail) {
        int cur = q[head++];
        if (cur == ti) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; d++) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (prev[ni] != -2) continue;
            prev[ni] = cur;
            q[tail++] = ni;
        }
    }
    if (prev[ti] == -2) return 0;
    int rev[MAXSTEP], n = 0, cur = ti;
    while (cur != si && n < MAXSTEP) {
        int p = prev[cur];
        int x = cur % N, y = cur / N;
        int px2 = p % N, py2 = p / N;
        for (int d = 0; d < 4; d++)
            if (px2 + DX[d] == x && py2 + DY[d] == y) { rev[n++] = d; break; }
        cur = p;
    }
    for (int i = 0; i < n; i++) out[i] = rev[n - 1 - i];
    return n;
}

typedef struct {
    int g[N][N];
    Pos p;
    Pos targets[MAXT];
    int nt, popped, instant;
} SimResult;

static SimResult simulate(const int in[N][N], int tile, Pos p,
                          const Pos *targets, int nt, int hand, int line, int d) {
    SimResult r;
    memcpy(r.g, in, sizeof(r.g));
    int insx, insy, popx, popy;
    bool horizontal = (d == 1 || d == 3);
    if (d == 1) {
        insx = 0; insy = line; popx = N - 1; popy = line;
        for (int x = N - 1; x > 0; x--) r.g[x][line] = r.g[x - 1][line];
    } else if (d == 3) {
        insx = N - 1; insy = line; popx = 0; popy = line;
        for (int x = 0; x < N - 1; x++) r.g[x][line] = r.g[x + 1][line];
    } else if (d == 0) {
        insx = line; insy = N - 1; popx = line; popy = 0;
        for (int y = 0; y < N - 1; y++) r.g[line][y] = r.g[line][y + 1];
    } else {
        insx = line; insy = 0; popx = line; popy = N - 1;
        for (int y = N - 1; y > 0; y--) r.g[line][y] = r.g[line][y - 1];
    }
    r.g[insy][insx] = tile;
    bool onLine = horizontal ? (p.y == line) : (p.x == line);
    r.p = onLine ? (Pos){wrap(p.x + DX[d]), wrap(p.y + DY[d])} : p;
    r.nt = 0; r.popped = 0;
    for (int i = 0; i < nt; i++) {
        Pos t = targets[i];
        if (t.x == popx && t.y == popy) r.popped++;
        else if ((horizontal && t.y == line) || (!horizontal && t.x == line))
            r.targets[r.nt++] = (Pos){wrap(t.x + DX[d]), wrap(t.y + DY[d])};
        else r.targets[r.nt++] = t;
    }
    r.instant = onLine ? hand : 0;
    if (hand && !onLine) r.targets[r.nt++] = (Pos){insx, insy};
    return r;
}

typedef struct {
    int score[8];
    char cmd[32];
} Candidate;

static bool cand_better(const Candidate *a, const Candidate *b) {
    for (int i = 0; i < 8; i++) {
        if (a->score[i] != b->score[i]) return a->score[i] > b->score[i];
    }
    return strcmp(a->cmd, b->cmd) > 0;
}

static void make_candidates(const int g[N][N], Pos p, int tile,
                            const Pos *targets, int nt, int hand,
                            Candidate cand[16], int *nc) {
    int lines_y[3] = {p.y - 1, p.y, p.y + 1};
    int lines_x[3] = {p.x - 1, p.x, p.x + 1};
    *nc = 0;
    for (int hor = 1; hor >= 0; hor--) {
        int *lines = hor ? lines_y : lines_x;
        for (int k = 0; k < 3; k++) {
            int line = lines[k];
            if (line < 0 || line >= N) continue;
            for (int di = 0; di < 2; di++) {
                int d = hor ? (di == 0 ? 1 : 3) : (di == 0 ? 0 : 2);
                SimResult r = simulate(g, tile, p, targets, nt, hand, line, d);
                int md = r.nt ? reachable_manhattan(r.g, r.p.x, r.p.y, r.targets, r.nt) : 0;
                int collectable = 0;
                for (int i = 0; i < r.nt; i++)
                    if (bfs_dist(r.g, r.p.x, r.p.y, r.targets[i].x, r.targets[i].y) <= 20)
                        collectable++;
                int conn = 0;
                for (int dd = 0; dd < 4; dd++)
                    if (connected(r.g, r.p.x, r.p.y, dd)) conn++;
                Candidate *c = &cand[(*nc)++];
                c->score[0] = r.instant;
                c->score[1] = -r.popped;
                c->score[2] = -md;
                c->score[3] = collectable;
                c->score[4] = conn;
                c->score[5] = -hand;
                c->score[6] = -line;
                c->score[7] = -d;
                snprintf(c->cmd, sizeof(c->cmd), "PUSH %d %s", line, DIRS[d]);
            }
        }
    }
    /* tri décroissant par (score, cmd) */
    for (int i = 1; i < *nc; i++) {
        Candidate key = cand[i];
        int j = i - 1;
        while (j >= 0 && cand_better(&key, &cand[j])) {
            cand[j + 1] = cand[j];
            j--;
        }
        cand[j + 1] = key;
    }
}

static char linebuf[1024];
static char *next_line(void) {
    if (!fgets(linebuf, sizeof(linebuf), stdin)) return NULL;
    size_t n = strlen(linebuf);
    while (n && (linebuf[n - 1] == '\n' || linebuf[n - 1] == '\r')) linebuf[--n] = 0;
    return linebuf;
}

static char last_push[32] = "";
static int stale = 0;

static void solve(int turn) {
    int g[N][N];
    for (int y = 0; y < N; y++) {
        char *p = next_line();
        if (!p) return;
        char save[1024];
        strcpy(save, p);
        char *tok = strtok(save, " ");
        for (int x = 0; x < N; x++) { g[y][x] = mask_of(tok); tok = strtok(NULL, " "); }
    }
    int px = 0, py = 0, tile = 0;
    for (int i = 0; i < 2; i++) {
        char *p = next_line();
        if (!p) return;
        int cards, x, y;
        char t[8];
        if (sscanf(p, "%d %d %d %7s", &cards, &x, &y, t) != 4) return;
        if (i == 0) { px = x; py = y; tile = mask_of(t); }
    }
    char *nl = next_line();
    if (!nl) return;
    int nitems = atoi(nl);
    char names[64][NAMELEN];
    int ix[64], iy[64], ni = 0;
    for (int i = 0; i < nitems; i++) {
        char *p = next_line();
        if (!p) return;
        char name[NAMELEN]; int x, y, pp;
        if (sscanf(p, "%63s %d %d %d", name, &x, &y, &pp) != 4) return;
        if (pp == 0 && ni < 64) {
            snprintf(names[ni], NAMELEN, "%s", name);
            ix[ni] = x; iy[ni] = y; ni++;
        }
    }
    nl = next_line();
    if (!nl) return;
    int nquests = atoi(nl);
    char quests[64][NAMELEN];
    int nq = 0;
    for (int i = 0; i < nquests; i++) {
        char *p = next_line();
        if (!p) return;
        char name[NAMELEN]; int pp;
        if (sscanf(p, "%63s %d", name, &pp) != 2) return;
        if (pp == 0 && nq < 64) {
            snprintf(quests[nq], NAMELEN, "%s", name);
            nq++;
        }
    }
    Pos targets[MAXT];
    int nt = 0, hand = 0;
    for (int i = 0; i < nq; i++) {
        for (int j = 0; j < ni; j++) {
            if (strcmp(quests[i], names[j]) != 0) continue;
            if (ix[j] == -1) hand++;
            else if (ix[j] != -2 && nt < MAXT) targets[nt++] = (Pos){ix[j], iy[j]};
            break;
        }
    }
    if (turn == 1) {
        int moves[MAXSTEP], n = 0;
        if (nt > 0) n = multi_bfs(g, px, py, targets, nt, moves);
        if (n == 0 && nt > 0) n = target_path(g, px, py, targets, nt, moves);
        if (n == 0 && hand > 0) n = edge_path(g, px, py, moves);
        if (n == 0) puts("PASS");
        else {
            printf("MOVE");
            for (int i = 0; i < n; i++) printf(" %s", DIRS[moves[i]]);
            putchar('\n');
        }
        return;
    }
    Candidate cand[16]; int nc = 0;
    make_candidates(g, (Pos){px, py}, tile, targets, nt, hand, cand, &nc);
    const char *cmd = cand[0].cmd;
    if (stale >= 3 && nc > 1) {
        for (int i = 0; i < nc; i++)
            if (strcmp(cand[i].cmd, last_push) != 0) { cmd = cand[i].cmd; break; }
    }
    if (strcmp(cmd, last_push) == 0) stale++;
    else { stale = 0; strncpy(last_push, cmd, sizeof(last_push) - 1); last_push[sizeof(last_push) - 1] = 0; }
    puts(cmd);
}

int main(void) {
    char *first;
    while ((first = next_line()) != NULL) {
        if (!*first) continue;
        solve(atoi(first));
        fflush(stdout);
    }
    return 0;
}
