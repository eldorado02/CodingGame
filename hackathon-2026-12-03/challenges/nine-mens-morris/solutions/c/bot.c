/* Nine Men's Morris — C port of solutions/python/bot.py (v1 alpha-beta).
 * Mills = aligned triples derived from the adjacency given at init.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FIELDS 24
#define MAX_MOVES (24 * 24)
#define MAX_MILLS 64
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)
#define EMPTY (-1)
enum { STONES = 9, TURN_NODES = 15000, TURN_MS = 35, STONE_VALUE = 100, MILL_VALUE = 10 };

typedef struct { int src, dst, take; } Move;
typedef struct { int owner[MAX_FIELDS]; int hand[2]; int side; } State;

static int F = 0, adj[MAX_FIELDS][8], adj_count[MAX_FIELDS];
static char names[MAX_FIELDS][8];
static int mills[MAX_MILLS][3], mill_count = 0, mills_of[MAX_FIELDS][4], mills_of_count[MAX_FIELDS];

static int field_index(const char *name) {
    for (int i = 0; i < F; ++i) if (strcmp(names[i], name) == 0) return i;
    return -1;
}

static int aligned(int a, int b, int c) {
    return (names[a][0] == names[b][0] && names[b][0] == names[c][0]) ||
           (strcmp(names[a] + 1, names[b] + 1) == 0 && strcmp(names[b] + 1, names[c] + 1) == 0);
}

static int cmp_mill(const void *x, const void *y) {
    const int *a = x, *b = y;
    for (int k = 0; k < 3; ++k) if (a[k] != b[k]) return a[k] - b[k];
    return 0;
}

static void build_mills(void) {
    for (int b = 0; b < F; ++b)
        for (int i = 0; i < adj_count[b]; ++i)
            for (int j = 0; j < adj_count[b]; ++j) {
                int a = adj[b][i], c = adj[b][j], dup = 0;
                if (a >= c || !aligned(a, b, c)) continue;
                for (int k = 0; k < mill_count; ++k) if (mills[k][0] == a && mills[k][1] == b && mills[k][2] == c) dup = 1;
                if (!dup && mill_count < MAX_MILLS) { mills[mill_count][0] = a; mills[mill_count][1] = b; mills[mill_count][2] = c; ++mill_count; }
            }
    qsort(mills, (size_t)mill_count, sizeof mills[0], cmp_mill);
    for (int k = 0; k < mill_count; ++k)
        for (int t = 0; t < 3; ++t) { int i = mills[k][t]; if (mills_of_count[i] < 4) mills_of[i][mills_of_count[i]++] = k; }
}

static int in_mill(const int *owner, int i) {
    int p = owner[i];
    for (int t = 0; t < mills_of_count[i]; ++t) {
        int *m = mills[mills_of[i][t]];
        if (owner[m[0]] == p && owner[m[1]] == p && owner[m[2]] == p) return 1;
    }
    return 0;
}

static int take_targets(const int *owner, int enemy, int *out) {
    int n = 0, free_n = 0, free_[MAX_FIELDS];
    for (int i = 0; i < F; ++i) if (owner[i] == enemy) { out[n++] = i; if (!in_mill(owner, i)) free_[free_n++] = i; }
    if (free_n) { memcpy(out, free_, sizeof(int) * (size_t)free_n); return free_n; }
    return n;
}

static int gen_moves(const State *s, Move *out) {
    int p = s->side, n = 0, steps[MAX_MOVES][2], ns = 0;
    if (s->hand[p] > 0) {
        for (int d = 0; d < F; ++d) if (s->owner[d] == EMPTY) { steps[ns][0] = -1; steps[ns][1] = d; ++ns; }
    } else {
        int mine[MAX_FIELDS], nm = 0;
        for (int i = 0; i < F; ++i) if (s->owner[i] == p) mine[nm++] = i;
        for (int k = 0; k < nm; ++k) {
            int src = mine[k];
            if (nm == 3) { for (int d = 0; d < F; ++d) if (s->owner[d] == EMPTY) { steps[ns][0] = src; steps[ns][1] = d; ++ns; } }
            else for (int j = 0; j < adj_count[src]; ++j) if (s->owner[adj[src][j]] == EMPTY) { steps[ns][0] = src; steps[ns][1] = adj[src][j]; ++ns; }
        }
    }
    int after[MAX_FIELDS], targets[MAX_FIELDS];
    for (int k = 0; k < ns; ++k) {
        memcpy(after, s->owner, sizeof after);
        if (steps[k][0] >= 0) after[steps[k][0]] = EMPTY;
        after[steps[k][1]] = p;
        if (in_mill(after, steps[k][1])) {
            int nt = take_targets(after, 1 - p, targets);
            for (int t = 0; t < nt && n < MAX_MOVES; ++t) { out[n].src = steps[k][0]; out[n].dst = steps[k][1]; out[n].take = targets[t]; ++n; }
        } else if (n < MAX_MOVES) { out[n].src = steps[k][0]; out[n].dst = steps[k][1]; out[n].take = -1; ++n; }
    }
    return n;
}

static State apply_move(const State *s, Move m) {
    State t = *s;
    if (m.src >= 0) t.owner[m.src] = EMPTY; else --t.hand[s->side];
    t.owner[m.dst] = s->side;
    if (m.take >= 0) t.owner[m.take] = EMPTY;
    t.side = 1 - s->side;
    return t;
}

static int material(const State *s, int p) {
    int n = s->hand[p];
    for (int i = 0; i < F; ++i) n += s->owner[i] == p;
    return n;
}

static int side_score(const State *s, int p) {
    int mc = 0, mob = 0;
    for (int k = 0; k < mill_count; ++k) mc += s->owner[mills[k][0]] == p && s->owner[mills[k][1]] == p && s->owner[mills[k][2]] == p;
    for (int i = 0; i < F; ++i) if (s->owner[i] == p) for (int j = 0; j < adj_count[i]; ++j) mob += s->owner[adj[i][j]] == EMPTY;
    return STONE_VALUE * material(s, p) + MILL_VALUE * mc + mob;
}

static int evaluate(const State *s) { return side_score(s, s->side) - side_score(s, 1 - s->side); }

static void order_moves(Move *mv, int n) {  /* stable, captures first */
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int j = i - 1;
        while (j >= 0 && mv[j].take < 0 && x.take >= 0) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

static long long node_limit = TURN_NODES, nodes = 0;
static int aborted = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > TURN_MS) { fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0; }
    if (material(s, s->side) < 3) return -(WIN - ply);
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    if (!n) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
    order_moves(mv, n);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        State t = apply_move(s, mv[i]);
        int v = -negamax(&t, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

static Move search(const State *root, Move *mv, int n) {
    order_moves(mv, n);
    Move best = mv[0];
    nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur = 0;
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[cur];
        for (int i = cur; i > 0; --i) mv[i] = mv[i - 1];  /* PV first */
        mv[0] = best;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

static void trim(char *s) {
    size_t len = strlen(s);
    while (len && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' ')) s[--len] = 0;
    size_t a = 0;
    while (s[a] == ' ') ++a;
    if (a) memmove(s, s + a, len - a + 1);
}

static int parse_command(const char *text, Move *m) {
    char buf[128], *parts[6];
    int np = 0, f[4];
    snprintf(buf, sizeof buf, "%s", text);
    for (char *tok = strtok(buf, ";"); tok && np < 6; tok = strtok(NULL, ";")) { trim(tok); parts[np++] = tok; }
    if (np < 2 || np > 4) return 0;
    for (int i = 1; i < np; ++i) if ((f[i - 1] = field_index(parts[i])) < 0) return 0;
    if (!strcmp(parts[0], "PLACE") && np == 2) { m->src = -1; m->dst = f[0]; m->take = -1; return 1; }
    if (!strcmp(parts[0], "PLACE&TAKE") && np == 3) { m->src = -1; m->dst = f[0]; m->take = f[1]; return 1; }
    if (!strcmp(parts[0], "MOVE") && np == 3) { m->src = f[0]; m->dst = f[1]; m->take = -1; return 1; }
    if (!strcmp(parts[0], "MOVE&TAKE") && np == 4) { m->src = f[0]; m->dst = f[1]; m->take = f[2]; return 1; }
    return 0;
}

int main(void) {
    long long cli = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli = atoll(env);
    static char line[4096], adj_text[MAX_FIELDS][128], texts[MAX_MOVES][64], first_cmd[64];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int me = atoi(line);
    if (!fgets(line, sizeof line, stdin)) return 0;
    int count = atoi(line);
    for (int i = 0; i < count; ++i) {
        if (!fgets(line, sizeof line, stdin)) return 0;
        trim(line);
        char *colon = strchr(line, ':');
        if (!colon || F >= MAX_FIELDS) continue;
        *colon = 0;
        trim(line);
        snprintf(names[F], sizeof names[F], "%s", line);
        snprintf(adj_text[F], sizeof adj_text[F], "%s", colon + 1);
        ++F;
    }
    for (int i = 0; i < F; ++i) {
        for (char *tok = strtok(adj_text[i], ";"); tok; tok = strtok(NULL, ";")) {
            trim(tok);
            int j = field_index(tok);
            if (j >= 0 && adj_count[i] < 8) adj[i][adj_count[i]++] = j;
        }
        for (int a = 1; a < adj_count[i]; ++a)  /* sort ascending */
            for (int b = a; b > 0 && adj[i][b - 1] > adj[i][b]; --b) { int t = adj[i][b]; adj[i][b] = adj[i][b - 1]; adj[i][b - 1] = t; }
    }
    build_mills();
    int placed[2] = {0, 0};
    static Move legal[MAX_MOVES];
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        trim(line);
        if (!strncmp(line, "PLACE", 5)) ++placed[1 - me];
        State s;
        for (int i = 0; i < F; ++i) s.owner[i] = EMPTY;
        if (!fgets(line, sizeof line, stdin)) return 0;
        trim(line);
        for (char *tok = strtok(line, ";"); tok; tok = strtok(NULL, ";")) {
            char *colon = strchr(tok, ':');
            if (!colon) continue;
            *colon = 0;
            trim(tok);
            char *st = colon + 1;
            while (*st == ' ') ++st;
            int i = field_index(tok);
            if (i >= 0 && (*st == '0' || *st == '1')) s.owner[i] = *st - '0';
        }
        if (!fgets(line, sizeof line, stdin)) return 0;
        int n = atoi(line), nl = 0;
        first_cmd[0] = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            trim(line);
            if (i == 0) snprintf(first_cmd, sizeof first_cmd, "%s", line);
            Move m;
            if (!parse_command(line, &m) || nl >= MAX_MOVES) continue;
            int dup = 0;
            for (int k = 0; k < nl; ++k) if (legal[k].src == m.src && legal[k].dst == m.dst && legal[k].take == m.take) dup = 1;
            if (!dup) { legal[nl] = m; snprintf(texts[nl], sizeof texts[nl], "%s", line); ++nl; }
        }
        if (!nl) { printf("%s\n", first_cmd[0] ? first_cmd : "PLACE;A1"); fflush(stdout); continue; }
        s.hand[0] = STONES - placed[0];
        s.hand[1] = STONES - placed[1];
        s.side = me;
        node_limit = cli > 0 ? cli : TURN_NODES;
        /* keep the text of each legal move aligned with the reordered array */
        Move copy[MAX_MOVES];
        memcpy(copy, legal, sizeof(Move) * (size_t)nl);
        Move m = search(&s, copy, nl);
        int idx = 0;
        for (int k = 0; k < nl; ++k) if (legal[k].src == m.src && legal[k].dst == m.dst && legal[k].take == m.take) { idx = k; break; }
        if (m.src < 0) ++placed[me];
        printf("%s\n", texts[idx]);
        fflush(stdout);
    }
    return 0;
}
