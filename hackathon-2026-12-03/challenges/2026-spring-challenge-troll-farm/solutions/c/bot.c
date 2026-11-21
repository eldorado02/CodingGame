/* Troll Farm (Wood 1+) — entraîne un troll polyvalent, récolte, dépose. */
#include <stdio.h>
#include <string.h>

#define MAXC 1024
#define MAXT 512
#define MAXOUT 8192

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

static int w, h, shack_x, shack_y;
static char grid[24][32];
static unsigned char blocked[MAXC], occupied[MAXC];
static int dista[MAXC], first_step[MAXC], queue[MAXC];

static int idx(int x, int y) { return y * w + x; }
static int walkable(int x, int y) {
    return x >= 0 && x < w && y >= 0 && y < h && grid[y][x] == '.';
}

static void bfs(int sx, int sy) {
    for (int i = 0; i < w * h; ++i) dista[i] = -1;
    int head = 0, tail = 0;
    int start = idx(sx, sy);
    dista[start] = 0;
    first_step[start] = start;
    queue[tail++] = start;
    while (head < tail) {
        int cur = queue[head++];
        int cx = cur % w, cy = cur / w;
        for (int d = 0; d < 4; ++d) {
            int nx = cx + DX[d], ny = cy + DY[d];
            if (!walkable(nx, ny) || occupied[idx(nx, ny)]) continue;
            int ni = idx(nx, ny);
            if (dista[ni] >= 0) continue;
            dista[ni] = dista[cur] + 1;
            first_step[ni] = (cur == start) ? ni : first_step[cur];
            queue[tail++] = ni;
        }
    }
}

static int step_toward(int goal, int speed) {
    if (dista[goal] <= speed) return goal;
    int cur = goal;
    while (dista[cur] > speed) cur = first_step[cur];
    return cur;
}

int main(void) {
    if (scanf("%d %d", &w, &h) != 2) return 0;
    for (int y = 0; y < h; ++y) {
        int got = scanf("%31s", grid[y]);
        (void)got;
        for (int x = 0; x < w; ++x) {
            if (grid[y][x] == '0') {
                shack_x = x;
                shack_y = y;
            }
            blocked[idx(x, y)] = grid[y][x] != '.';
        }
    }
    int drop[4], drop_n = 0;
    for (int d = 0; d < 4; ++d) {
        int nx = shack_x + DX[d], ny = shack_y + DY[d];
        if (walkable(nx, ny)) drop[drop_n++] = idx(nx, ny);
    }

    const int MAX_TROLLS = 3;
    char out[MAXOUT];
    while (1) {
        int inv[6];
        if (scanf("%d %d %d %d %d %d", &inv[0], &inv[1], &inv[2],
                  &inv[3], &inv[4], &inv[5]) != 6) break;
        for (int k = 0; k < 6; ++k) { int tmp; int got = scanf("%d", &tmp); (void)got; }

        int tree_n;
        if (scanf("%d", &tree_n) != 1) break;
        static char kind[MAXT][16];
        static int tx[MAXT], ty[MAXT], fruit[MAXT];
        for (int i = 0; i < tree_n; ++i) {
            int size, health, cooldown;
            int got = scanf("%15s %d %d %d %d %d %d", kind[i], &tx[i], &ty[i],
                            &size, &health, &fruit[i], &cooldown);
            (void)got;
        }

        int troll_n;
        if (scanf("%d", &troll_n) != 1) break;
        static int id[MAXT], px[MAXT], py[MAXT], speed[MAXT], carry[MAXT], load[MAXT];
        memset(occupied, 0, sizeof(occupied));
        int mine[MAXT], mine_n = 0;
        for (int i = 0; i < troll_n; ++i) {
            int player, power, reserved, a, b, c, d, e, f;
            int got = scanf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                            &id[i], &player, &px[i], &py[i], &speed[i], &carry[i],
                            &power, &reserved, &a, &b, &c, &d, &e, &f);
            (void)got;
            int ci = idx(px[i], py[i]);
            if (player == 0) {
                mine[mine_n++] = i;
                load[i] = a + b + c + d;
            } else {
                occupied[ci] = 1;
            }
        }
        for (int k = 0; k < mine_n; ++k) occupied[idx(px[mine[k]], py[mine[k]])] = 1;

        out[0] = '\0';
        int len = 0;
        if (mine_n < MAX_TROLLS) {
            int n = mine_n;
            int best_score = -1, best_cost = 1 << 30, bs = 1, bc = 1, bh = 1;
            for (int s = 1; s <= 3; ++s)
                for (int c = 1; c <= 3; ++c)
                    for (int hv = 1; hv <= 3; ++hv) {
                        int cp = n + s * s, cl = n + c * c, ca = n + hv * hv;
                        if (cp > inv[0] || cl > inv[1] || ca > inv[2]) continue;
                        int score = 3 * hv + 2 * c + s;
                        int cost = cp + cl + ca;
                        if (score > best_score || (score == best_score && cost < best_cost)) {
                            best_score = score; best_cost = cost;
                            bs = s; bc = c; bh = hv;
                        }
                    }
            if (best_score >= 0)
                len += snprintf(out + len, sizeof(out) - (size_t)len,
                                "TRAIN %d %d %d 0", bs, bc, bh);
        }

        for (int k = 0; k < mine_n; ++k) {
            int t = mine[k];
            if (len) len += snprintf(out + len, sizeof(out) - (size_t)len, ";");
            int here = idx(px[t], py[t]);
            bfs(px[t], py[t]);

            int on_fruit = 0;
            for (int i = 0; i < tree_n; ++i)
                if (fruit[i] > 0 && tx[i] == px[t] && ty[i] == py[t]) on_fruit = 1;
            if (on_fruit && load[t] < carry[t]) {
                len += snprintf(out + len, sizeof(out) - (size_t)len,
                                "HARVEST %d", id[t]);
                continue;
            }
            if (load[t] > 0) {
                int adjacent = 0;
                for (int d = 0; d < drop_n; ++d)
                    if (drop[d] == here) adjacent = 1;
                if (adjacent) {
                    len += snprintf(out + len, sizeof(out) - (size_t)len,
                                    "DROP %d", id[t]);
                    continue;
                }
                int best = -1, bd = 0, bx = 0, by = 0;
                for (int d = 0; d < drop_n; ++d) {
                    int cell = drop[d], dd = dista[cell];
                    int x = cell % w, y = cell / w;
                    if (dd >= 0 && (best < 0 || dd < bd || (dd == bd && (y < by || (y == by && x < bx))))) {
                        best = cell; bd = dd; bx = x; by = y;
                    }
                }
                if (best < 0) {
                    len += snprintf(out + len, sizeof(out) - (size_t)len, "WAIT");
                } else {
                    int st = step_toward(best, speed[t]);
                    len += snprintf(out + len, sizeof(out) - (size_t)len,
                                    "MOVE %d %d %d", id[t], st % w, st / w);
                }
                continue;
            }
            int best = -1, bd = 0, bf = 0, bx = 0, by = 0;
            for (int i = 0; i < tree_n; ++i) {
                if (fruit[i] <= 0) continue;
                int cell = idx(tx[i], ty[i]), dd = dista[cell];
                if (dd < 0) continue;
                if (best < 0 || dd < bd ||
                    (dd == bd && (fruit[i] > bf ||
                     (fruit[i] == bf && (ty[i] < by || (ty[i] == by && tx[i] < bx)))))) {
                    best = cell; bd = dd; bf = fruit[i]; bx = tx[i]; by = ty[i];
                }
            }
            if (best < 0) {
                len += snprintf(out + len, sizeof(out) - (size_t)len, "WAIT");
            } else {
                int st = step_toward(best, speed[t]);
                len += snprintf(out + len, sizeof(out) - (size_t)len,
                                "MOVE %d %d %d", id[t], st % w, st / w);
            }
        }
        puts(len ? out : "WAIT");
        fflush(stdout);
    }
    return 0;
}
