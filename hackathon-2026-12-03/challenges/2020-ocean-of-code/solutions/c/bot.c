/* Ocean of Code (Wood 3) — croyance adversaire + torpedo. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 15
#define INF 1000000

static const char *DIR_NAME[4] = {"N", "E", "S", "W"};
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

typedef struct {
    bool water[N][N];
    bool visited[N][N];
    bool bel[N][N];
    int prev_opp_life;
    int last_x, last_y;
    int cnt, sumx, sumy;
} Bot;

static char linebuf[2048];

static char *next_line(void) {
    if (!fgets(linebuf, sizeof(linebuf), stdin)) return NULL;
    size_t n = strlen(linebuf);
    while (n && (linebuf[n - 1] == '\n' || linebuf[n - 1] == '\r'))
        linebuf[--n] = 0;
    return linebuf;
}

static void clear_bel(Bot *b) {
    for (int y = 0; y < N; y++)
        for (int x = 0; x < N; x++) b->bel[y][x] = false;
}

static void reset_bel(Bot *b) {
    for (int y = 0; y < N; y++)
        for (int x = 0; x < N; x++) b->bel[y][x] = b->water[y][x];
}

static bool bel_empty(const Bot *b) {
    for (int y = 0; y < N; y++)
        for (int x = 0; x < N; x++)
            if (b->bel[y][x]) return false;
    return true;
}

static void reset_if_empty(Bot *b) {
    if (bel_empty(b)) reset_bel(b);
}

static int sector_of(int x, int y) {
    return x / 5 + 1 + (y / 5) * 3;
}

static void bfs(const bool water[N][N], int sx, int sy, int dist[N][N]) {
    for (int y = 0; y < N; y++)
        for (int x = 0; x < N; x++) dist[y][x] = -1;
    int qx[N * N], qy[N * N], head = 0, tail = 0;
    dist[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; tail++;
    while (head < tail) {
        int x = qx[head], y = qy[head]; head++;
        for (int k = 0; k < 4; k++) {
            int nx = x + DX[k], ny = y + DY[k];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!water[ny][nx] || dist[ny][nx] != -1) continue;
            dist[ny][nx] = dist[y][x] + 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
}

static void choose_start(const Bot *b, int *out_x, int *out_y) {
    int bx = -1, by = -1, bdeg = -1, bcenter = INF;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (!b->water[y][x]) continue;
            int deg = 0;
            for (int k = 0; k < 4; k++) {
                int nx = x + DX[k], ny = y + DY[k];
                if (nx >= 0 && nx < N && ny >= 0 && ny < N && b->water[ny][nx]) deg++;
            }
            int center = abs(x - 7) + abs(y - 7);
            if (deg > bdeg ||
                (deg == bdeg && center < bcenter) ||
                (deg == bdeg && center == bcenter &&
                 (bx < 0 || x < bx || (x == bx && y < by)))) {
                bx = x; by = y; bdeg = deg; bcenter = center;
            }
        }
    }
    *out_x = bx; *out_y = by;
}

static void compute_centroid(Bot *b) {
    b->cnt = b->sumx = b->sumy = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (b->bel[y][x]) {
                b->cnt++; b->sumx += x; b->sumy += y;
            }
        }
    }
    if (b->cnt == 0) {
        reset_bel(b);
        for (int y = 0; y < N; y++)
            for (int x = 0; x < N; x++)
                if (b->bel[y][x]) {
                    b->cnt++; b->sumx += x; b->sumy += y;
                }
    }
}

static int best_dir(const Bot *b, int px, int py, bool ignore_visited,
                    int *nx_out, int *ny_out, int *idx_out) {
    bool found = false;
    int best_free = -1, best_dist = 0, best_idx = 0, bnx = 0, bny = 0;
    for (int k = 0; k < 4; k++) {
        int nx = px + DX[k], ny = py + DY[k];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N || !b->water[ny][nx]) continue;
        if (!ignore_visited && b->visited[ny][nx]) continue;
        int free = 0;
        for (int j = 0; j < 4; j++) {
            int ax = nx + DX[j], ay = ny + DY[j];
            if (ax < 0 || ax >= N || ay < 0 || ay >= N || !b->water[ay][ax]) continue;
            bool blocked = ignore_visited ? (ax == px && ay == py)
                                          : b->visited[ay][ax];
            if (!blocked) free++;
        }
        int cdist = abs(2 * b->cnt * nx - b->sumx) + abs(2 * b->cnt * ny - b->sumy);
        if (!found || free > best_free ||
            (free == best_free && cdist < best_dist) ||
            (free == best_free && cdist == best_dist && k < best_idx)) {
            found = true; best_free = free; best_dist = cdist; best_idx = k;
            bnx = nx; bny = ny;
        }
    }
    if (!found) return 0;
    *nx_out = bnx; *ny_out = bny; *idx_out = best_idx;
    return 1;
}

static void append(char *out, const char *piece) {
    size_t used = strlen(out);
    size_t room = 2048 - used;
    if (strlen(piece) >= room) return;
    strcat(out, piece);
}

int main(void) {
    Bot b;
    memset(&b, 0, sizeof(b));
    b.prev_opp_life = -1;
    b.last_x = b.last_y = -1;

    char *p = next_line();
    if (!p) return 0;
    char head[2048];
    strcpy(head, p);
    int hw, hh, hid;
    int nhead = sscanf(head, "%d %d %d", &hw, &hh, &hid);

    if (nhead == 3) {
        char row[2048];
        for (int y = 0; y < hh; y++) {
            char *r = next_line();
            if (!r) return 0;
            strcpy(row, r);
            for (int x = 0; x < N; x++) b.water[y][x] = row[x] == '.';
        }
        reset_bel(&b);
        int sx, sy;
        choose_start(&b, &sx, &sy);
        b.visited[sy][sx] = true;
        printf("%d %d\n", sx, sy);
        fflush(stdout);
    }

    while ((p = next_line()) != NULL) {
        if (!*p) break;
        char t[2048];
        strcpy(t, p);
        int px, py, my_life, opp_life, cooldown, sonar, silence, mine;
        if (sscanf(t, "%d %d %d %d %d %d %d %d", &px, &py, &my_life, &opp_life,
                   &cooldown, &sonar, &silence, &mine) != 8) break;
        (void)my_life; (void)sonar; (void)silence; (void)mine;
        next_line(); /* sonar result */
        char *orderline = next_line();
        if (!orderline) break;
        char orders[2048];
        strcpy(orders, orderline);

        if (b.last_x >= 0 && b.prev_opp_life >= 0) {
            int delta = b.prev_opp_life - opp_life;
            int tx = b.last_x, ty = b.last_y;
            if (delta == 2) {
                clear_bel(&b);
                if (b.water[ty][tx]) b.bel[ty][tx] = true;
            } else if (delta == 1) {
                clear_bel(&b);
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = tx + dx, ny = ty + dy;
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N && b.water[ny][nx])
                            b.bel[ny][nx] = true;
                    }
            } else if (delta == 0) {
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        int nx = tx + dx, ny = ty + dy;
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                            b.bel[ny][nx] = false;
                    }
            }
        }
        b.last_x = b.last_y = -1;
        reset_if_empty(&b);

        if (strcmp(orders, "NA") != 0) {
            char *cursor = orders;
            while (*cursor) {
                char *bar = strchr(cursor, '|');
                size_t seglen = bar ? (size_t)(bar - cursor) : strlen(cursor);
                char s2[2048];
                if (seglen >= sizeof(s2)) seglen = sizeof(s2) - 1;
                memcpy(s2, cursor, seglen);
                s2[seglen] = 0;
                char *tok = strtok(s2, " \t\r\n");
                if (tok) {
                    if (strcmp(tok, "MOVE") == 0) {
                        char *dirtok = strtok(NULL, " \t\r\n");
                        int k = -1;
                        if (dirtok)
                            for (int j = 0; j < 4; j++)
                                if (strcmp(dirtok, DIR_NAME[j]) == 0) { k = j; break; }
                        if (k >= 0) {
                            bool nb[N][N];
                            memset(nb, 0, sizeof(nb));
                            for (int y = 0; y < N; y++)
                                for (int x = 0; x < N; x++) {
                                    if (!b.bel[y][x]) continue;
                                    int nx = x + DX[k], ny = y + DY[k];
                                    if (nx >= 0 && nx < N && ny >= 0 && ny < N && b.water[ny][nx])
                                        nb[ny][nx] = true;
                                }
                            memcpy(b.bel, nb, sizeof(nb));
                        }
                    } else if (strcmp(tok, "SURFACE") == 0) {
                        char *stok = strtok(NULL, " \t\r\n");
                        if (stok) {
                            int s = atoi(stok);
                            for (int y = 0; y < N; y++)
                                for (int x = 0; x < N; x++)
                                    if (b.bel[y][x] && sector_of(x, y) != s)
                                        b.bel[y][x] = false;
                        }
                    } else if (strcmp(tok, "TORPEDO") == 0) {
                        char *xt = strtok(NULL, " \t\r\n");
                        char *yt = strtok(NULL, " \t\r\n");
                        if (xt && yt) {
                            int ax = atoi(xt), ay = atoi(yt);
                            int d[N][N];
                            bfs(b.water, ax, ay, d);
                            for (int y = 0; y < N; y++)
                                for (int x = 0; x < N; x++)
                                    if (b.bel[y][x] && (d[y][x] < 0 || d[y][x] > 4))
                                        b.bel[y][x] = false;
                        }
                    }
                }
                reset_if_empty(&b);
                if (!bar) break;
                cursor = bar + 1;
            }
        }

        compute_centroid(&b);

        b.visited[py][px] = true;
        char out[2048] = "";
        int final_x = px, final_y = py;
        int nx, ny, kidx;
        if (best_dir(&b, px, py, false, &nx, &ny, &kidx)) {
            char piece[64];
            snprintf(piece, sizeof(piece), "MOVE %s TORPEDO", DIR_NAME[kidx]);
            append(out, piece);
            final_x = nx; final_y = ny;
            b.visited[ny][nx] = true;
        } else {
            for (int y = 0; y < N; y++)
                for (int x = 0; x < N; x++) b.visited[y][x] = false;
            b.visited[py][px] = true;
            append(out, "SURFACE");
            if (best_dir(&b, px, py, true, &nx, &ny, &kidx)) {
                char piece[64];
                snprintf(piece, sizeof(piece), "|MOVE %s TORPEDO", DIR_NAME[kidx]);
                append(out, piece);
                final_x = nx; final_y = ny;
                b.visited[py][px] = true;
                b.visited[ny][nx] = true;
            }
        }

        if (cooldown == 0) {
            int dstart[N][N];
            bfs(b.water, px, py, dstart);
            int best_score = -1, bx = -1, by = -1;
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    if (!b.water[y][x] || dstart[y][x] < 0 || dstart[y][x] > 4) continue;
                    if (abs(x - final_x) <= 1 && abs(y - final_y) <= 1) continue;
                    int score = b.bel[y][x] ? 2 : 0;
                    for (int dy = -1; dy <= 1; dy++)
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            int ax = x + dx, ay = y + dy;
                            if (ax >= 0 && ax < N && ay >= 0 && ay < N && b.bel[ay][ax])
                                score++;
                        }
                    if (score > best_score ||
                        (score == best_score && (bx < 0 || x < bx || (x == bx && y < by)))) {
                        best_score = score; bx = x; by = y;
                    }
                }
            }
            if (best_score > 0) {
                char piece[64];
                snprintf(piece, sizeof(piece), "|TORPEDO %d %d", bx, by);
                append(out, piece);
                b.last_x = bx; b.last_y = by;
            }
        }

        printf("%s\n", out);
        fflush(stdout);
        b.prev_opp_life = opp_life;
    }
    return 0;
}
