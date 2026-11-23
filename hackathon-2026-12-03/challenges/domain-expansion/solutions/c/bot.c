/* Domain Expansion 7x7 (CodinGame) — C port of solutions/python/bot.py (v1).
 * Same rules, action order, evaluation and tie-breaks.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { SIZE = 7, CELLS = SIZE * SIZE };
enum { MAX_MOVE = 3 };             /* Manhattan reach of a move */
enum { PATH_EXPANSION_DEPTH = 3 }; /* referee BFS expands nodes up to this depth */
static const char LETTERS[4] = {'U', 'R', 'D', 'L'};
static const int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
static const int OPPOSITE[4] = {2, 3, 0, 1};
enum { WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64 };
enum { DEFAULT_NODES = 10000 };    /* CodinGame budget (100 ms per turn): ~15 ms local */
enum { FIRST_TURN_NODES = 60000 }; /* first turn has 1000 ms */
enum { TIME_GUARD_MS = 40, FIRST_TURN_GUARD_MS = 400 }; /* 40 % of the limits */
enum { MAX_ACTIONS = CELLS * 4 };
enum { NOT_FINAL = INF };          /* final_score sentinel: tokens still connected */

static int NEIGHBOUR[CELLS][4];

static void build_neighbours(void) {
    for (int cell = 0; cell < CELLS; ++cell)
        for (int d = 0; d < 4; ++d) {
            int nx = cell % SIZE + DX[d], ny = cell / SIZE + DY[d];
            NEIGHBOUR[cell][d] = (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) ? ny * SIZE + nx : -1;
        }
}

static int manhattan(int a, int b) { return abs(a % SIZE - b % SIZE) + abs(a / SIZE - b / SIZE); }

typedef struct { int cell, dir; } Action;

typedef struct {
    int walls[CELLS]; /* bitmask over U R D L of sides already walled */
    int tokens[2];    /* 0 = us, 1 = them */
} State;

static void init_borders(State *s) {
    for (int c = 0; c < CELLS; ++c) {
        s->walls[c] = 0;
        for (int d = 0; d < 4; ++d) if (NEIGHBOUR[c][d] < 0) s->walls[c] |= 1 << d;
    }
}

static void apply_action(State *s, int player, int cell, int d) {
    s->tokens[player] = cell;
    s->walls[cell] |= 1 << d;
    s->walls[NEIGHBOUR[cell][d]] |= 1 << OPPOSITE[d];
}

/* Cells the token of `player` may move to, ascending (referee findPath). */
static int destinations(const State *s, int player, int *out) {
    int start = s->tokens[player], blocker = s->tokens[1 - player];
    int depth[CELLS], queue[CELLS], head = 0, tail = 0;
    for (int c = 0; c < CELLS; ++c) depth[c] = -1;
    depth[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int cell = queue[head++];
        if (depth[cell] > PATH_EXPANSION_DEPTH) continue;
        for (int d = 0; d < 4; ++d) {
            int next = NEIGHBOUR[cell][d];
            if (((s->walls[cell] >> d) & 1) || next == blocker || depth[next] >= 0) continue;
            depth[next] = depth[cell] + 1;
            queue[tail++] = next;
        }
    }
    int n = 0;
    for (int c = 0; c < CELLS; ++c)
        if (depth[c] >= 0 && manhattan(start, c) <= MAX_MOVE) out[n++] = c;
    return n;
}

static int generate_actions(const State *s, int player, Action *out) {
    int cells[CELLS], nc = destinations(s, player, cells), n = 0;
    for (int i = 0; i < nc; ++i)
        for (int d = 0; d < 4; ++d)
            if (!((s->walls[cells[i]] >> d) & 1)) { out[n].cell = cells[i]; out[n].dir = d; ++n; }
    return n;
}

/* BFS steps through walls only (tokens do not block territory); -1 = unreachable. */
static void distances(const State *s, int start, int *dist) {
    int queue[CELLS], head = 0, tail = 0;
    for (int c = 0; c < CELLS; ++c) dist[c] = -1;
    dist[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int cell = queue[head++];
        for (int d = 0; d < 4; ++d) {
            if ((s->walls[cell] >> d) & 1) continue;
            int next = NEIGHBOUR[cell][d];
            if (dist[next] < 0) { dist[next] = dist[cell] + 1; queue[tail++] = next; }
        }
    }
}

static int reachable_count(const int *dist) {
    int n = 0;
    for (int c = 0; c < CELLS; ++c) n += dist[c] >= 0;
    return n;
}

/* NOT_FINAL while the tokens are connected, else the outcome for `player`. */
static int final_score(const State *s, int player) {
    int mine[CELLS], theirs[CELLS];
    distances(s, s->tokens[player], mine);
    if (mine[s->tokens[1 - player]] >= 0) return NOT_FINAL;
    distances(s, s->tokens[1 - player], theirs);
    int diff = reachable_count(mine) - reachable_count(theirs);
    if (diff > 0) return WIN + diff;
    if (diff < 0) return -WIN + diff;
    return 0;
}

/* Voronoi territory of `player` minus the opponent's. */
static int evaluate(const State *s, int player) {
    int mine[CELLS], theirs[CELLS], score = 0;
    distances(s, s->tokens[player], mine);
    distances(s, s->tokens[1 - player], theirs);
    for (int c = 0; c < CELLS; ++c) {
        int a = mine[c], b = theirs[c];
        if (a < 0 && b < 0) continue;
        if (b < 0 || (a >= 0 && a < b)) ++score;
        else if (a < 0 || b < a) --score;
    }
    return score;
}

/* ------------------------------------------------------------- search */
static long long node_limit = DEFAULT_NODES, nodes = 0;
static int time_guard_ms = TIME_GUARD_MS, last_depth = 0, aborted = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

static int negamax(const State *s, int player, int depth, int alpha, int beta);

/* Value of `a` for `player`, searching depth - 1 below it. */
static int action_value(const State *s, int player, const Action *a, int depth, int alpha, int beta) {
    State child = *s;
    apply_action(&child, player, a->cell, a->dir);
    int final = final_score(&child, player);
    if (final != NOT_FINAL) return final;
    return -negamax(&child, 1 - player, depth - 1, -beta, -alpha);
}

static int negamax(const State *s, int player, int depth, int alpha, int beta) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 63) == 0 && elapsed_ms() > time_guard_ms) { fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0; }
    if (depth == 0) return evaluate(s, player);
    Action acts[MAX_ACTIONS];
    int n = generate_actions(s, player, acts), best = -INF;
    for (int i = 0; i < n; ++i) {
        int value = action_value(s, player, &acts[i], depth, alpha, beta);
        if (aborted) return 0;
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

/* Root actions sorted by 1-ply value (stable insertion sort). */
static int ordered_root(const State *s, int player, Action *acts) {
    int n = generate_actions(s, player, acts), keys[MAX_ACTIONS];
    for (int i = 0; i < n; ++i) keys[i] = action_value(s, player, &acts[i], 1, -INF, INF);
    for (int i = 1; i < n; ++i) {
        Action item = acts[i];
        int key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { acts[j + 1] = acts[j]; keys[j + 1] = keys[j]; --j; }
        acts[j + 1] = item;
        keys[j + 1] = key;
    }
    return n;
}

static Action search(const State *s, int player) {
    nodes = 0; aborted = 0; last_depth = 0;
    Action acts[MAX_ACTIONS];
    int n = ordered_root(s, player, acts);
    Action best = acts[0];
    for (int depth = 2; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_value = -INF, best_index = 0;
        for (int i = 0; i < n; ++i) {
            int value = action_value(s, player, &acts[i], depth, alpha, INF);
            if (aborted) break;
            if (value > best_value) { best_value = value; best_index = i; }
            if (value > alpha) alpha = value;
        }
        if (aborted) break;
        best = acts[best_index];
        last_depth = depth;
        for (int i = best_index; i > 0; --i) acts[i] = acts[i - 1];
        acts[0] = best;
        if (best_value >= WIN || best_value <= -WIN) break;
    }
    return best;
}

/* ------------------------------------------------------------- I/O */
static int letter_index(const char *t) {
    for (int d = 0; d < 4; ++d) if (t[0] == LETTERS[d] && t[1] == 0) return d;
    return -1;
}

int main(void) {
    build_neighbours();
    const char *env = getenv("CG_NODES");
    long long cli_nodes = env ? atoll(env) : -1;
    char line[256];
    int mx, my, ox, oy;
    if (!fgets(line, sizeof line, stdin)) return 0;
    if (!fgets(line, sizeof line, stdin) || sscanf(line, "%d %d", &mx, &my) != 2) return 0;
    if (!fgets(line, sizeof line, stdin) || sscanf(line, "%d %d", &ox, &oy) != 2) return 0;
    State s;
    init_borders(&s);
    s.tokens[0] = my * SIZE + mx;
    s.tokens[1] = oy * SIZE + ox;
    int first_turn = 1;
    while (fgets(line, sizeof line, stdin)) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        int ax = -1, ay = -1;
        char dir[16] = "";
        sscanf(line, "%d %d %15s", &ax, &ay, dir);
        int d = letter_index(dir);
        if (d >= 0) apply_action(&s, 1, ay * SIZE + ax, d);
        node_limit = cli_nodes > 0 ? cli_nodes : (first_turn ? FIRST_TURN_NODES : DEFAULT_NODES);
        time_guard_ms = first_turn ? FIRST_TURN_GUARD_MS : TIME_GUARD_MS;
        Action a = search(&s, 0);
        apply_action(&s, 0, a.cell, a.dir);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%d %d %c\n", a.cell % SIZE, a.cell / SIZE, LETTERS[a.dir]);
        fflush(stdout);
        first_turn = 0;
    }
    return 0;
}
