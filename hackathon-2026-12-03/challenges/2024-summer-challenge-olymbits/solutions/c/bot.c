/* Olymbits (Silver, 4 mini-jeux) — décision jointe, arithmétique entière. */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const NAMES[4] = {"LEFT", "DOWN", "RIGHT", "UP"};
static const int DX[4] = {-1, 0, 1, 0};
static const int DY[4] = {0, 1, 0, -1};
static const char LETTER[4] = {'L', 'D', 'R', 'U'};
static const int STEPS[4] = {1, 2, 3, 2};
static const int JUMP[4] = {0, 0, 0, 1};

static int clamp20(int v) {
    if (v < -20) return -20;
    if (v > 20) return 20;
    return v;
}

static void hurdle_step(const char *track, int pos, int stun, int steps,
                        int jump, int *op, int *os) {
    if (stun > 0) { *op = pos; *os = stun - 1; return; }
    if (pos < 0) pos = 0;
    int last = (int)strlen(track) - 1;
    if (jump) {
        int np = pos + 2 > last ? last : pos + 2;
        *os = (np < last && track[np] == '#') ? 3 : 0;
        *op = np;
        return;
    }
    int np = pos;
    for (int s = 1; s <= steps; ++s) {
        if (pos + s >= last) { *op = last; *os = 0; return; }
        if (track[pos + s] == '#') { *op = pos + s; *os = 3; return; }
        np = pos + s;
    }
    *op = np;
    *os = 0;
}

static void hurdle_scores(const char *track, int pos, int stun, int *out) {
    int last = (int)strlen(track) - 1;
    const int options[4] = {2, 1, 3, 0};
    for (int a = 0; a < 4; ++a) {
        int p = pos, s = stun;
        hurdle_step(track, p, s, STEPS[a], JUMP[a], &p, &s);
        int done = -1;
        if (p >= last) {
            done = 1;
        } else {
            for (int t = 0; t < 3; ++t) {
                int moved = 0;
                for (int k = 0; k < 4; ++k) {
                    int m = options[k], q = p, ss = s;
                    hurdle_step(track, q, ss, STEPS[m], JUMP[m], &q, &ss);
                    if (q > p) { p = q; s = ss; moved = 1; break; }
                }
                if (!moved) hurdle_step(track, p, s, 2, 1, &p, &s);
                if (p >= last) { done = t + 2; break; }
            }
        }
        if (done < 0) done = 5 + (last - p) / 2;
        out[a] = -100 * done;
    }
}

static void archery_scores(const char *gpu, int x, int y, int *out) {
    int winds[64], nw = 0;
    for (const char *c = gpu; *c && nw < 64; ++c)
        if (*c >= '0' && *c <= '9') winds[nw++] = *c - '0';
    int w = nw ? winds[0] : 1;
    for (int a = 0; a < 4; ++a) {
        int nx = clamp20(x + DX[a] * w), ny = clamp20(y + DY[a] * w);
        int d2 = nx * nx + ny * ny;
        int px = nx, py = ny;
        for (int i = 1; i < nw && i <= 5; ++i) {
            int fw = winds[i];
            if (px > 0) px = clamp20(px - fw);
            else if (px < 0) { px += fw; if (px > 20) px = 20; }
            if (py > 0) { py -= fw; if (py < 0) py = 0; }
            else if (py < 0) { py += fw; if (py > 0) py = 0; }
        }
        out[a] = -2 * d2 - (px * px + py * py);
    }
}

static void roller_scores(const char *gpu, int pos, int risk, int turns,
                          const int *others, int n_others, int *out) {
    const int steps[5] = {0, 1, 2, 2, 3};
    for (int a = 0; a < 4; ++a) {
        const char *f = strchr(gpu, LETTER[a]);
        int idx = f ? (int)(f - gpu) : -1;
        if (idx < 0 || idx >= 4 || gpu[idx] == 0) {
            out[a] = -10000;
        } else if (risk < 0) {
            out[a] = -100;
        } else {
            int nr = risk + (idx == 0 ? -1 : idx == 1 ? 0 : idx == 2 ? 1 : 2);
            int collide = 0;
            for (int i = 0; i < n_others; ++i) {
                int diff = (others[i] - pos) % 10;
                if (diff < 0) diff += 10;
                if (diff == 0) collide = 1;
            }
            if (collide) nr += 2;
            if (nr >= 5) out[a] = -100 - turns;
            else out[a] = 10 * steps[idx] - 7 * nr - 40 * collide;
        }
    }
}

static void diving_scores(const char *gpu, int combo, int *out) {
    char goal = gpu[0] ? gpu[0] : 'U';
    for (int a = 0; a < 4; ++a) {
        if (LETTER[a] == goal) out[a] = 10 * (8 + (combo + 1 < 10 ? combo + 1 : 10));
        else out[a] = 10 * (-(combo < 10 ? combo : 10) - 1);
    }
}

static int all_of(const char *s, const char *set) {
    if (!*s) return 0;
    for (const char *c = s; *c; ++c) if (!strchr(set, *c)) return 0;
    return 1;
}

static int parse_line(char *buf, char *gpu, size_t gpu_size, int *reg) {
    char *tok = strtok(buf, " \t\r\n");
    if (!tok) return 0;
    snprintf(gpu, gpu_size, "%s", tok);
    int n = 1;
    for (; n < 8; ++n) {
        tok = strtok(NULL, " \t\r\n");
        if (!tok) break;
        reg[n - 1] = (int)strtol(tok, NULL, 10);
    }
    return n;
}

int main(void) {
    char buf[256], gpu[128];
    int reg[7];
    long long total[4] = {0, 0, 0, 0};
    int player = 0, games = 0;
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    player = (int)strtol(buf, NULL, 10);
    if (!fgets(buf, sizeof buf, stdin)) return 0;
    games = (int)strtol(buf, NULL, 10);

    while (1) {
        int alive = 1;
        for (int i = 0; i < 3 && alive; ++i) alive = fgets(buf, sizeof buf, stdin) != NULL;
        if (!alive) break;
        total[0] = total[1] = total[2] = total[3] = 0;
        for (int g = 0; g < games; ++g) {
            if (!fgets(buf, sizeof buf, stdin)) break;
            if (parse_line(buf, gpu, sizeof gpu, reg) < 8) continue;
            if (strcmp(gpu, "GAME_OVER") == 0) continue;
            int vals[4] = {0, 0, 0, 0}, weight = 0;
            if (strlen(gpu) >= 10 && all_of(gpu, ".#")) {
                hurdle_scores(gpu, reg[player], reg[3 + player], vals);
                weight = 3;
            } else if (all_of(gpu, "0123456789")) {
                archery_scores(gpu, reg[2 * player], reg[2 * player + 1], vals);
                weight = 1;
            } else if (strlen(gpu) == 4 && all_of(gpu, "ULDR") && reg[6] >= 0) {
                int others[3], j = 0;
                for (int i = 0; i < 3; ++i) if (i != player) others[j++] = reg[i];
                roller_scores(gpu, reg[player], reg[3 + player], reg[6], others, j, vals);
                weight = 2;
            } else if (all_of(gpu, "ULDR")) {
                diving_scores(gpu, reg[3 + player], vals);
                weight = 2;
            }
            for (int a = 0; a < 4; ++a) total[a] += weight * vals[a];
        }
        int best = 0;
        for (int a = 1; a < 4; ++a) if (total[a] > total[best]) best = a;
        printf("%s\n", NAMES[best]);
        fflush(stdout);
    }
    return 0;
}
