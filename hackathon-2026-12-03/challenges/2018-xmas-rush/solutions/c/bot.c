/* Xmas Rush (Silver) — port C du prototype Python v17 validé 3/5 contre 2215206. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 7
#define CELLS (N * N)
#define MAXSTEP 20
#define MAXIT 32
#define STSZ (CELLS * 8)
#define NAMELEN 64

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};
static const char *DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

typedef struct { int x, y; } Pos;
typedef int Grid[N][N];

static int wrap(int v) { v %= N; return v < 0 ? v + N : v; }

static int mask_of(const char *s) {
    int m = 0;
    for (int d = 0; d < 4; ++d) if (s[d] == '1') m |= 1 << d;
    return m;
}

static bool connected(const Grid g, int x, int y, int d) {
    if (!((g[y][x] >> d) & 1)) return false;
    int nx = x + DX[d], ny = y + DY[d];
    return nx >= 0 && nx < N && ny >= 0 && ny < N &&
           ((g[ny][nx] >> ((d + 2) % 4)) & 1);
}

static int bfs_dist(const Grid g, int sx, int sy, int tx, int ty) {
    if (sx == tx && sy == ty) return 0;
    bool seen[CELLS] = {false};
    int qx[CELLS], qy[CELLS], qs[CELLS], head = 0, tail = 0;
    qx[tail] = sx; qy[tail] = sy; qs[tail] = 0; ++tail;
    seen[sy * N + sx] = true;
    while (head < tail) {
        int x = qx[head], y = qy[head], s = qs[head]; ++head;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx == tx && ny == ty) return s + 1;
            if (!seen[ny * N + nx]) {
                seen[ny * N + nx] = true;
                qx[tail] = nx; qy[tail] = ny; qs[tail] = s + 1; ++tail;
            }
        }
    }
    return 999;
}

static void dist_field(const Grid g, int tx, int ty, int dist[N][N]) {
    for (int y = 0; y < N; ++y) for (int x = 0; x < N; ++x) dist[y][x] = 999;
    dist[ty][tx] = 0;
    int qx[CELLS], qy[CELLS], head = 0, tail = 0;
    qx[tail] = tx; qy[tail] = ty; ++tail;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        int nd = dist[y][x] + 1;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (dist[ny][nx] == 999) {
                dist[ny][nx] = nd;
                qx[tail] = nx; qy[tail] = ny; ++tail;
            }
        }
    }
}

static int idx_[STSZ], pre_[STSZ], pdir_[STSZ];
static int xx_[STSZ], yy_[STSZ], mm_[STSZ], stp_[STSZ];
static int que_[STSZ];

/* Meilleur chemin de <=20 pas collectant le plus de quêtes. */
static int multi_bfs(const Grid g, int px, int py, const Pos *targets, int nt,
                     int out[MAXSTEP]) {
    for (int i = 0; i < STSZ; ++i) idx_[i] = -1;
    if (nt == 0) return 0;
    int fields[3][N][N];
    for (int i = 0; i < nt && i < 3; ++i)
        dist_field(g, targets[i].x, targets[i].y, fields[i]);
    int start = (py * N + px) * 8;
    idx_[start] = 0; pre_[0] = -1;
    xx_[0] = px; yy_[0] = py; mm_[0] = 0; stp_[0] = 0;
    int head = 0, tail = 0;
    que_[tail++] = 0;
    int bestGot = 0, bestSteps = 0, bestState = 0, bestMd = 999;
    int initialMd = 999;
    for (int i = 0; i < nt && i < 3; ++i) {
        int v = fields[i][py][px];
        if (v < initialMd) initialMd = v;
    }
    bestMd = initialMd;
    while (head < tail) {
        int id = que_[head++];
        int x = xx_[id], y = yy_[id], mask = mm_[id], st = stp_[id];
        int got = 0;
        for (int i = 0; i < nt; ++i) if ((mask >> i) & 1) ++got;
        int md = 0;
        if (got < nt) {
            md = 999;
            for (int i = 0; i < nt && i < 3; ++i) {
                int v = fields[i][y][x];
                if (v < md) md = v;
            }
        }
        if (got > bestGot || (got == bestGot &&
            (md < bestMd || (md == bestMd && st < bestSteps)))) {
            bestGot = got; bestSteps = st; bestState = id; bestMd = md;
        }
        if (st >= MAXSTEP) continue;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int nm = mask;
            for (int i = 0; i < nt; ++i)
                if (targets[i].x == nx && targets[i].y == ny) nm |= 1 << i;
            int code = (ny * N + nx) * 8 + nm;
            if (idx_[code] != -1) continue;
            int nid = tail;
            idx_[code] = nid; pre_[nid] = id; pdir_[nid] = d;
            xx_[nid] = nx; yy_[nid] = ny; mm_[nid] = nm; stp_[nid] = st + 1;
            que_[tail++] = nid;
        }
    }
    int rev[MAXSTEP], n = 0;
    for (int cur = bestState; pre_[cur] >= 0; cur = pre_[cur])
        rev[n++] = pdir_[cur];
    for (int i = 0; i < n; ++i) out[i] = rev[n - 1 - i];
    return n;
}

static void path_to_goal(const Grid g, int px, int py, int tx, int ty,
                         int out[MAXSTEP], int *outn) {
    *outn = 0;
    int prev[CELLS], pdir[CELLS], q[CELLS], head = 0, tail = 0;
    for (int i = 0; i < CELLS; ++i) prev[i] = -2;
    int s = py * N + px;
    prev[s] = -1; q[tail++] = s;
    int target = -1;
    while (head < tail) {
        int id = q[head++];
        if (id == ty * N + tx) { target = id; break; }
        int x = id % N, y = id / N;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int nid = ny * N + nx;
            if (prev[nid] != -2) continue;
            prev[nid] = id; pdir[nid] = d; q[tail++] = nid;
        }
    }
    if (target < 0) return;
    int rev[MAXSTEP], n = 0;
    for (int cur = target; prev[cur] >= 0; cur = prev[cur]) rev[n++] = pdir[cur];
    for (int i = 0; i < n && i < MAXSTEP; ++i) out[i] = rev[n - 1 - i];
    *outn = n < MAXSTEP ? n : MAXSTEP;
}

static void nearest_path(const Grid g, int px, int py, const Pos *goals, int ng,
                         int out[MAXSTEP], int *outn) {
    *outn = 0;
    int best = 1000000, tx = -1, ty = -1;
    for (int i = 0; i < ng; ++i) {
        int d = bfs_dist(g, px, py, goals[i].x, goals[i].y);
        if (d < 999 && d < best) { best = d; tx = goals[i].x; ty = goals[i].y; }
    }
    if (tx < 0 || best == 0) return;
    path_to_goal(g, px, py, tx, ty, out, outn);
}

typedef struct {
    Grid g;
    int nx, ny, popped, instant, nt;
    Pos targets[MAXIT];
} SimResult;

static SimResult simulate(const Grid grid, int tile, int px, int py,
                          const Pos *board_targets, int nb, int hand,
                          int line, int d) {
    SimResult r;
    memcpy(r.g, grid, sizeof(Grid));
    r.popped = 0; r.instant = 0; r.nt = 0;
    bool horizontal = (d == 1 || d == 3);
    int insx, insy, popx, popy;
    if (d == 1) {
        insx = 0; insy = line; popx = N - 1; popy = line;
        for (int x = N - 1; x > 0; --x) r.g[line][x] = r.g[line][x - 1];
    } else if (d == 3) {
        insx = N - 1; insy = line; popx = 0; popy = line;
        for (int x = 0; x < N - 1; ++x) r.g[line][x] = r.g[line][x + 1];
    } else if (d == 0) {
        insx = line; insy = N - 1; popx = line; popy = 0;
        for (int y = 0; y < N - 1; ++y) r.g[y][line] = r.g[y + 1][line];
    } else {
        insx = line; insy = 0; popx = line; popy = N - 1;
        for (int y = N - 1; y > 0; --y) r.g[y][line] = r.g[y - 1][line];
    }
    r.g[insy][insx] = tile;
    bool on_line = horizontal ? (py == line) : (px == line);
    r.nx = on_line ? wrap(px + DX[d]) : px;
    r.ny = on_line ? wrap(py + DY[d]) : py;
    for (int i = 0; i < nb; ++i) {
        int x = board_targets[i].x, y = board_targets[i].y;
        if (x == popx && y == popy) {
            ++r.popped;
        } else if ((horizontal && y == line) || (!horizontal && x == line)) {
            r.targets[r.nt++] = (Pos){wrap(x + DX[d]), wrap(y + DY[d])};
        } else {
            r.targets[r.nt++] = (Pos){x, y};
        }
    }
    if (on_line && r.nx == insx && r.ny == insy) r.instant = hand;
    if (hand && !on_line) r.targets[r.nt++] = (Pos){insx, insy};
    return r;
}

static int reachable_count(const Grid g, int sx, int sy) {
    bool seen[CELLS] = {false};
    int qx[CELLS], qy[CELLS], head = 0, tail = 0;
    qx[tail] = sx; qy[tail] = sy; ++tail;
    seen[sy * N + sx] = true;
    int total = 1;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (!seen[ny * N + nx]) {
                seen[ny * N + nx] = true; ++total;
                qx[tail] = nx; qy[tail] = ny; ++tail;
            }
        }
    }
    return total;
}

typedef struct {
    int turn_type, px, py, tile, ox, oy, hand;
    Grid grid;
    Pos board_targets[MAXIT], opp_targets[MAXIT], our_all[MAXIT], opp_all[MAXIT];
    int nb, no, na, nall;
} Input;

static bool read_input(Input *in) {
    if (scanf("%d", &in->turn_type) != 1) return false;
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            char tok[16];
            if (scanf("%15s", tok) != 1) return false;
            in->grid[y][x] = mask_of(tok);
        }
    }
    for (int p = 0; p < 2; ++p) {
        int count;
        char tok[16];
        if (scanf("%d %d %d %15s", &count,
                  p == 0 ? &in->px : &in->ox,
                  p == 0 ? &in->py : &in->oy, tok) != 4) return false;
        if (p == 0) in->tile = mask_of(tok);
    }
    in->nb = in->no = in->na = in->nall = in->hand = 0;
    char names[2][MAXIT][NAMELEN];
    Pos poss[2][MAXIT];
    int cnts[2] = {0, 0};
    int ni;
    if (scanf("%d", &ni) != 1) return false;
    for (int i = 0; i < ni && i < MAXIT; ++i) {
        char name[NAMELEN]; int x, y, pid;
        if (scanf("%63s %d %d %d", name, &x, &y, &pid) != 4) return false;
        if (pid < 0 || pid > 1) continue;
        strncpy(names[pid][cnts[pid]], name, NAMELEN - 1);
        names[pid][cnts[pid]][NAMELEN - 1] = '\0';
        poss[pid][cnts[pid]++] = (Pos){x, y};
    }
    char quests[2][MAXIT][NAMELEN];
    int qcnts[2] = {0, 0};
    int nq;
    if (scanf("%d", &nq) != 1) return false;
    for (int i = 0; i < nq && i < MAXIT; ++i) {
        char name[NAMELEN]; int pid;
        if (scanf("%63s %d", name, &pid) != 2) return false;
        if (pid < 0 || pid > 1) continue;
        strncpy(quests[pid][qcnts[pid]++], name, NAMELEN - 1);
        quests[pid][qcnts[pid] - 1][NAMELEN - 1] = '\0';
    }
    for (int pid = 0; pid < 2; ++pid) {
        for (int i = 0; i < cnts[pid]; ++i) {
            int x = poss[pid][i].x, y = poss[pid][i].y;
            if (x == -1) { if (pid == 0) ++in->hand; continue; }
            if (x == -2) continue;
            bool revealed = false;
            for (int k = 0; k < qcnts[pid]; ++k)
                if (strcmp(quests[pid][k], names[pid][i]) == 0) { revealed = true; break; }
            if (pid == 0) in->our_all[in->nall++] = (Pos){x, y};
            else in->opp_all[in->na++] = (Pos){x, y};
            if (revealed) {
                if (pid == 0) in->board_targets[in->nb++] = (Pos){x, y};
                else in->opp_targets[in->no++] = (Pos){x, y};
            }
        }
    }
    return true;
}

typedef struct {
    double score;
    int instant, popped, conn, line, d;
    char cmd[16];
} Candidate;

static int cand_better(const void *a_, const void *b_) {
    const Candidate *a = a_, *b = b_;
    if (a->score != b->score) return a->score > b->score ? -1 : 1;
    if (a->instant != b->instant) return a->instant > b->instant ? -1 : 1;
    if (a->popped != b->popped) return a->popped < b->popped ? -1 : 1;
    if (a->conn != b->conn) return a->conn > b->conn ? -1 : 1;
    if (a->line != b->line) return a->line < b->line ? -1 : 1;
    if (a->d != b->d) return a->d < b->d ? -1 : 1;
    int c = strcmp(a->cmd, b->cmd);
    return c > 0 ? -1 : (c < 0 ? 1 : 0);
}

static void best_push(const Input *in, const char *last_push, int stale,
                      char *out) {
    Candidate cands[28];
    int nc = 0;
    int last_line = -1; bool last_horiz = false;
    int lp; char ldir[16];
    if (sscanf(last_push, "PUSH %d %15s", &lp, ldir) == 2) {
        last_line = lp;
        last_horiz = (strcmp(ldir, "RIGHT") == 0 || strcmp(ldir, "LEFT") == 0);
    }
    for (int pass = 0; pass < 2; ++pass) {
        bool horizontal = (pass == 0);
        for (int line = 0; line < N; ++line) {
            for (int step = 0; step < 2; ++step) {
                int d = horizontal ? (step == 0 ? 1 : 3) : (step == 0 ? 0 : 2);
                if (last_line >= 0 && last_line == line && last_horiz == horizontal)
                    continue;
                SimResult s = simulate(in->grid, in->tile, in->px, in->py,
                                       in->board_targets, in->nb, in->hand, line, d);
                SimResult o = simulate(in->grid, in->tile, in->ox, in->oy,
                                       in->opp_targets, in->no, 0, line, d);
                double score = 1000.0 * s.instant - 100.0 * s.popped;
                for (int i = 0; i < s.nt; ++i) {
                    int dist = bfs_dist(s.g, s.nx, s.ny, s.targets[i].x, s.targets[i].y);
                    if (dist <= 20) score += 1000 - 10 * dist;
                    else if (dist < 999) score += 200.0 / (dist + 1);
                    else score += 500.0 - 30.0 *
                        (abs(s.nx - s.targets[i].x) + abs(s.ny - s.targets[i].y));
                }
                score += 2.0 * reachable_count(s.g, s.nx, s.ny);
                for (int i = 0; i < o.nt; ++i) {
                    int dist = bfs_dist(s.g, o.nx, o.ny, o.targets[i].x, o.targets[i].y);
                    if (dist <= 20) score -= 10;
                }
                for (int i = 0; i < in->nall; ++i) {
                    bool is_target = false;
                    for (int k = 0; k < in->nb; ++k)
                        if (in->board_targets[k].x == in->our_all[i].x &&
                            in->board_targets[k].y == in->our_all[i].y) { is_target = true; break; }
                    if (is_target) continue;
                    int dist = bfs_dist(s.g, s.nx, s.ny, in->our_all[i].x, in->our_all[i].y);
                    if (dist <= 20) score += 20;
                    else if (dist < 999) score += 20.0 / (dist + 1);
                }
                int conn = 0;
                for (int k = 0; k < 4; ++k) if (connected(s.g, s.nx, s.ny, k)) ++conn;
                score += conn;
                Candidate *c = &cands[nc++];
                c->score = score; c->instant = s.instant; c->popped = s.popped;
                c->conn = conn; c->line = line; c->d = d;
                snprintf(c->cmd, sizeof(c->cmd), "PUSH %d %s", line, DIRS[d]);
            }
        }
    }
    qsort(cands, nc, sizeof(cands[0]), cand_better);
    if (stale >= 3 && nc > 1) {
        for (int i = 0; i < nc; ++i)
            if (strcmp(cands[i].cmd, last_push) != 0) { strcpy(out, cands[i].cmd); return; }
    }
    strcpy(out, cands[0].cmd);
}

int main(void) {
    Input in;
    char last_push[32] = "";
    int stale = 0;
    while (read_input(&in)) {
        if (in.turn_type == 1) {
            int moves[MAXSTEP], nm = 0;
            if (in.nb) nm = multi_bfs(in.grid, in.px, in.py, in.board_targets, in.nb, moves);
            if (!nm && in.nb)
                nearest_path(in.grid, in.px, in.py, in.board_targets, in.nb, moves, &nm);
            if (!nm && in.hand) {
                Pos edge[2 * N + 2 * N];
                int ne = 0;
                for (int x = 0; x < N; ++x) { edge[ne++] = (Pos){x, 0}; edge[ne++] = (Pos){x, N - 1}; }
                for (int y = 0; y < N; ++y) { edge[ne++] = (Pos){0, y}; edge[ne++] = (Pos){N - 1, y}; }
                nearest_path(in.grid, in.px, in.py, edge, ne, moves, &nm);
            }
            if (!nm) {
                puts("PASS");
            } else {
                printf("MOVE");
                for (int i = 0; i < nm; ++i) printf(" %s", DIRS[moves[i]]);
                putchar('\n');
            }
        } else {
            char cmd[32];
            best_push(&in, last_push, stale, cmd);
            if (strcmp(cmd, last_push) == 0) ++stale; else stale = 0;
            strcpy(last_push, cmd);
            puts(cmd);
        }
        fflush(stdout);
    }
    return 0;
}
