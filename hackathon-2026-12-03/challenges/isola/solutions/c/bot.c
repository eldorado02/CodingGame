/* Isola 9x9 — C port of solutions/python/bot.py (v1).
 * Negamax alpha-beta over (move, removal) pairs; removals limited to the
 * opponent's liberties. Eval = Voronoi + MOBILITY_WEIGHT * mobility diff.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define W 9
#define N 81
#define MAX_ACTIONS (8 * 9)
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)

enum { FIRST_TURN_NODES = 60000, TURN_NODES = 10000, FIRST_TURN_MS = 700, TURN_MS = 70 };
enum { MOBILITY_WEIGHT = 3 };

typedef struct { int gone[N]; int pos[2]; int side; } State;
typedef struct { int mv, rm; } Action;

static int neighbors[N][8], neighbor_count[N];

static void init_tables(void) {
    for (int i = 0; i < N; ++i) {
        int x = i % W, y = i / W, n = 0;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = x + dx, ny = y + dy;
                if ((dx || dy) && nx >= 0 && nx < W && ny >= 0 && ny < W) neighbors[i][n++] = ny * W + nx;
            }
        neighbor_count[i] = n;
    }
}

static int is_free(const State *s, int t) { return !s->gone[t] && t != s->pos[0] && t != s->pos[1]; }

static int mobility(const State *s, int p) {
    int n = 0, from = s->pos[p];
    for (int k = 0; k < neighbor_count[from]; ++k) n += is_free(s, neighbors[from][k]);
    return n;
}

static int gen_actions(State *s, Action *out) {
    int me = s->side, op = 1 - s->side, n = 0, from = s->pos[me];
    for (int k = 0; k < neighbor_count[from]; ++k) {
        int t = neighbors[from][k];
        if (!is_free(s, t)) continue;
        s->pos[me] = t;
        int added = 0, opp = s->pos[op];
        for (int j = 0; j < neighbor_count[opp]; ++j) {
            int r = neighbors[opp][j];
            if (is_free(s, r)) { out[n].mv = t; out[n].rm = r; ++n; ++added; }
        }
        if (!added)
            for (int r = 0; r < N; ++r)
                if (is_free(s, r)) { out[n].mv = t; out[n].rm = r; ++n; break; }
        s->pos[me] = from;
    }
    return n;
}

static State apply_action(const State *s, Action a) {
    State t = *s;
    t.pos[s->side] = a.mv;
    t.gone[a.rm] = 1;
    t.side = 1 - s->side;
    return t;
}

static void bfs(const State *s, int start, int *dist) {
    int queue[N], head = 0, tail = 0;
    for (int i = 0; i < N; ++i) dist[i] = -1;
    dist[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int i = queue[head++];
        for (int k = 0; k < neighbor_count[i]; ++k) {
            int t = neighbors[i][k];
            if (dist[t] < 0 && is_free(s, t)) { dist[t] = dist[i] + 1; queue[tail++] = t; }
        }
    }
}

/* tiles reached strictly first by the side to move minus by the other */
static int voronoi(const State *s) {
    int mine[N], theirs[N], score = 0;
    bfs(s, s->pos[s->side], mine);
    bfs(s, s->pos[1 - s->side], theirs);
    for (int i = 0; i < N; ++i) {
        int a = mine[i], b = theirs[i];
        if (a > 0 && (b < 0 || a < b)) ++score;
        else if (b > 0 && (a < 0 || b < a)) --score;
    }
    return score;
}

static int evaluate(const State *s) {
    return voronoi(s) + MOBILITY_WEIGHT * (mobility(s, s->side) - mobility(s, 1 - s->side));
}

static long long node_limit = TURN_NODES, nodes = 0;
static int time_ms = TURN_MS, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int negamax(const State *s0, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 255) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    State s = *s0;
    if (!mobility(&s, s.side)) return -(WIN - ply);
    if (depth == 0) return evaluate(&s);
    Action mv[MAX_ACTIONS];
    int n = gen_actions(&s, mv), best = -INF;
    for (int i = 0; i < n; ++i) {
        State t = apply_action(&s, mv[i]);
        int v = -negamax(&t, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

static Action search(State root) {
    Action mv[MAX_ACTIONS];
    int n = gen_actions(&root, mv);
    Action best = mv[0];
    last_depth = 0; nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        for (int i = 0; i < n; ++i) {
            State t = apply_action(&root, mv[i]);
            int v = -negamax(&t, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > best_v) { best_v = v; cur_idx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[cur_idx];
        last_depth = depth;
        for (int i = cur_idx; i > 0; --i) mv[i] = mv[i - 1];  /* PV first */
        mv[0] = best;
        if (best_v >= WIN - MAX_DEPTH || best_v <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

int main(void) {
    init_tables();
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    int mx, my;
    if (scanf("%d %d", &mx, &my) != 2) return 0;
    State s;
    memset(&s, 0, sizeof s);
    int me = mx == 0 ? 0 : 1, my_pos = my * W + mx, first = 1;
    int ox, oy, rx, ry;
    while (scanf("%d %d %d %d", &ox, &oy, &rx, &ry) == 4) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if (rx >= 0 && rx < W && ry >= 0 && ry < W) s.gone[ry * W + rx] = 1;
        s.pos[me] = my_pos;
        s.pos[1 - me] = oy * W + ox;
        s.side = me;
        node_limit = cli_nodes > 0 ? cli_nodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = first ? FIRST_TURN_MS : TURN_MS;
        first = 0;
        if (!mobility(&s, me)) { printf("RANDOM\n"); fflush(stdout); continue; }
        Action a = search(s);
        my_pos = a.mv;
        s.gone[a.rm] = 1;
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%d %d %d %d\n", a.mv % W, a.mv / W, a.rm % W, a.rm / W);
        fflush(stdout);
    }
    return 0;
}
