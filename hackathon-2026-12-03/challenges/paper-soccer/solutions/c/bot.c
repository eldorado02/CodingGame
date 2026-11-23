/* Paper Soccer 8x10 (CodinGame) — C port of solutions/python/bot.py (v1).
 * Same geometry, turn generation, evaluation, move order and tie-breaks.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { WIDTH = 8, HEIGHT = 10, COLS = WIDTH + 1, ROWS = HEIGHT + 1 };
enum { FIELD_POINTS = COLS * ROWS };
enum { TOP_GOAL = FIELD_POINTS, BOTTOM_GOAL = FIELD_POINTS + 3, POINTS = FIELD_POINTS + 6 };
enum { GOAL_X0 = 3, GOAL_WIDTH = 3 };
enum { CENTER_X = WIDTH / 2 };
enum { START_BALL = (HEIGHT / 2) * COLS + CENTER_X };
static const int DX[8] = {0, 1, 1, 1, 0, -1, -1, -1}, DY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
enum { PLAYER_TOP = 0, PLAYER_BOTTOM = 1 };
enum { NORMAL = 0, WON = 1, LOST = 2, OUTCOMES = 3 };

enum { WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64 };
enum { ROW_WEIGHT = 80 };             /* one row of vertical progress */
enum { GOAL_DISTANCE_WEIGHT = 120 };  /* unused-edge path to goal (Lecorbio evaluator) */
enum { NO_PATH = 40 };                /* goal distance when every route is drawn */
enum { CENTRE_WEIGHT = 15 };          /* per column away from the goal mouth */
enum { CENTRE_ZONE_ROWS = 3 };        /* centring matters this close to a goal */
enum { MAX_DFS_STEPS = 20000 };       /* bounce-sequence enumeration cap per node */
enum { DEFAULT_NODES = 3000 };        /* CodinGame budget (200 ms per turn) */
enum { FIRST_TURN_NODES = 15000 };    /* first turn has 1000 ms */
enum { TIME_GUARD_MS = 80, FIRST_TURN_GUARD_MS = 400 }; /* 40 % of 200 / 1000 ms: late nodes are slow */
enum { MAX_TURN_LEN = 448 };          /* more than the number of edges */
enum { MAX_TURNS = POINTS * OUTCOMES };
enum { MAX_PLY = MAX_DEPTH + 2 };

/* ------------------------------------------------------------- geometry */
static int NEIGHBOUR[POINTS][8], DEGREE[POINTS];

static int point_index(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) return y * COLS + x;
    int inMouth = x >= GOAL_X0 && x < GOAL_X0 + GOAL_WIDTH;
    if (inMouth && y == -1) return TOP_GOAL + x - GOAL_X0;
    if (inMouth && y == ROWS) return BOTTOM_GOAL + x - GOAL_X0;
    return -1;
}

static void point_xy(int p, int *x, int *y) {
    if (p < FIELD_POINTS) { *x = p % COLS; *y = p / COLS; }
    else if (p < BOTTOM_GOAL) { *x = GOAL_X0 + p - TOP_GOAL; *y = -1; }
    else { *x = GOAL_X0 + p - BOTTOM_GOAL; *y = ROWS; }
}

static int is_removed_pair(int p, int q) {
    /* goal posts: the ball cannot enter the goal diagonally from outside the mouth */
    static const int PAIRS[4][4] = {{2, 0, 3, -1}, {6, 0, 5, -1}, {2, 10, 3, 11}, {6, 10, 5, 11}};
    for (int i = 0; i < 4; ++i) {
        const int *r = PAIRS[i];
        int a = point_index(r[0], r[1]), b = point_index(r[2], r[3]);
        if ((p == a && q == b) || (p == b && q == a)) return 1;
    }
    return 0;
}

static void build_neighbours(void) {
    for (int p = 0; p < POINTS; ++p) {
        int x, y;
        point_xy(p, &x, &y);
        DEGREE[p] = 0;
        for (int d = 0; d < 8; ++d) {
            int q = point_index(x + DX[d], y + DY[d]);
            NEIGHBOUR[p][d] = (q < 0 || is_removed_pair(p, q)) ? -1 : q;
            if (NEIGHBOUR[p][d] >= 0) ++DEGREE[p];
        }
    }
}

static int direction_between(int p, int q) {
    for (int d = 0; d < 8; ++d) if (NEIGHBOUR[p][d] == q) return d;
    return -1;
}

static int goal_owner(int p) {
    if (p >= TOP_GOAL && p < BOTTOM_GOAL) return PLAYER_TOP;
    if (p >= BOTTOM_GOAL) return PLAYER_BOTTOM;
    return -1;
}

/* ------------------------------------------------------------- pitch */
typedef struct {
    int drawn[POINTS]; /* bitmask of directions already drawn from each point */
    int free_edges[POINTS];
    int ball;
} Pitch;

static int pitch_draw(Pitch *s, int p, int d) {
    int q = NEIGHBOUR[p][d];
    s->drawn[p] |= 1 << d;
    s->drawn[q] |= 1 << ((d + 4) % 8);
    --s->free_edges[p];
    --s->free_edges[q];
    return q;
}

static void pitch_undraw(Pitch *s, int p, int d) {
    int q = NEIGHBOUR[p][d];
    s->drawn[p] &= ~(1 << d);
    s->drawn[q] &= ~(1 << ((d + 4) % 8));
    ++s->free_edges[p];
    ++s->free_edges[q];
}

static int pitch_can_move(const Pitch *s, int d) {
    return NEIGHBOUR[s->ball][d] >= 0 && !((s->drawn[s->ball] >> d) & 1);
}

static void draw_pair(Pitch *s, int x1, int y1, int x2, int y2) {
    int a = point_index(x1, y1), b = point_index(x2, y2);
    pitch_draw(s, a, direction_between(a, b));
}

/* Border lines and goal frames, as Pitch() draws them. */
static void pitch_reset(Pitch *s) {
    memset(s->drawn, 0, sizeof s->drawn);
    for (int p = 0; p < POINTS; ++p) s->free_edges[p] = DEGREE[p];
    s->ball = START_BALL;
    for (int y = 0; y < HEIGHT; ++y) { draw_pair(s, 0, y, 0, y + 1); draw_pair(s, WIDTH, y, WIDTH, y + 1); }
    for (int x = 0; x < WIDTH; ++x)
        if (x < GOAL_X0 || x >= GOAL_X0 + GOAL_WIDTH - 1) { draw_pair(s, x, 0, x + 1, 0); draw_pair(s, x, HEIGHT, x + 1, HEIGHT); }
    const int frames[2][2] = {{-1, 0}, {ROWS, HEIGHT}};
    for (int i = 0; i < 2; ++i) {
        const int *f = frames[i];
        draw_pair(s, 3, f[0], 4, f[0]);
        draw_pair(s, 4, f[0], 5, f[0]);
        draw_pair(s, 3, f[0], 3, f[1]);
        draw_pair(s, 5, f[0], 5, f[1]);
    }
}

static void pitch_apply(Pitch *s, const char *digits, int len) {
    for (int i = 0; i < len; ++i) {
        int d = digits[i] - '0';
        if (d >= 0 && d < 8 && pitch_can_move(s, d)) s->ball = pitch_draw(s, s->ball, d);
    }
}

/* ------------------------------------------------------------- turn generation */
typedef struct { char digits[MAX_TURN_LEN]; int len, end, outcome; } Turn;

static struct {
    Pitch *pitch;
    int mover, steps, count, path_len;
    char path[MAX_TURN_LEN];
    char seen[POINTS][OUTCOMES];
    Turn *out;
} gen;

static void emit(int q, int outcome) {
    if (gen.seen[q][outcome]) return;
    gen.seen[q][outcome] = 1;
    Turn *t = &gen.out[gen.count++];
    memcpy(t->digits, gen.path, gen.path_len);
    t->len = gen.path_len; t->end = q; t->outcome = outcome;
}

static void dfs(void) {
    Pitch *s = gen.pitch;
    int p = s->ball;
    for (int d = 0; d < 8; ++d) {
        if (gen.steps >= MAX_DFS_STEPS) return;
        if (!pitch_can_move(s, d)) continue;
        ++gen.steps;
        int q = pitch_draw(s, p, d);
        s->ball = q;
        gen.path[gen.path_len++] = (char)('0' + d);
        int owner = goal_owner(q);
        int bounce = DEGREE[q] - s->free_edges[q] > 1;
        if (owner >= 0) emit(q, owner == gen.mover ? WON : LOST);
        else if (s->free_edges[q] == 0) emit(q, LOST);
        else if (bounce) dfs();
        else emit(q, NORMAL);
        --gen.path_len;
        s->ball = p;
        pitch_undraw(s, p, d);
    }
}

/* Complete turns from pitch.ball, first sequence per (end point, outcome). */
static int generate_turns(Pitch *pitch, int mover, Turn *out) {
    gen.pitch = pitch; gen.mover = mover; gen.steps = 0; gen.count = 0; gen.path_len = 0; gen.out = out;
    memset(gen.seen, 0, sizeof gen.seen);
    dfs();
    return gen.count;
}

/* ------------------------------------------------------------- evaluation */
static int rows_to_goal(int player, int p) {
    int x, y;
    point_xy(p, &x, &y);
    return player == PLAYER_TOP ? y + 1 : ROWS - y;
}

/* Edges the ball needs, over unused edges only, to reach the goal `player` attacks. */
static int goal_distance(const Pitch *pitch, int player) {
    int targetLo = player == PLAYER_TOP ? TOP_GOAL : BOTTOM_GOAL;
    int dist[POINTS], queue[POINTS], head = 0, tail = 0;
    for (int p = 0; p < POINTS; ++p) dist[p] = NO_PATH;
    dist[pitch->ball] = 0;
    queue[tail++] = pitch->ball;
    while (head < tail) {
        int p = queue[head++];
        if (p >= targetLo && p < targetLo + GOAL_WIDTH) return dist[p];
        if (goal_owner(p) >= 0) continue; /* the ball stops in any goal */
        for (int d = 0; d < 8; ++d) {
            int q = NEIGHBOUR[p][d];
            if (q >= 0 && !((pitch->drawn[p] >> d) & 1) && dist[q] == NO_PATH) {
                dist[q] = dist[p] + 1;
                queue[tail++] = q;
            }
        }
    }
    return NO_PATH;
}

/* Score for `player`, who is about to move with the ball at pitch.ball. */
static int evaluate(const Pitch *pitch, int player) {
    int x, y;
    point_xy(pitch->ball, &x, &y);
    int mine = rows_to_goal(player, pitch->ball), theirs = rows_to_goal(1 - player, pitch->ball);
    int score = ROW_WEIGHT * (theirs - mine);
    score += GOAL_DISTANCE_WEIGHT * (goal_distance(pitch, 1 - player) - goal_distance(pitch, player));
    int offCentre = abs(x - CENTER_X);
    if (mine <= CENTRE_ZONE_ROWS) score -= CENTRE_WEIGHT * offCentre;
    if (theirs <= CENTRE_ZONE_ROWS) score += CENTRE_WEIGHT * offCentre;
    return score;
}

static int order_key(const Turn *t, int mover) {
    if (t->outcome == WON) return WIN;
    if (t->outcome == LOST) return -WIN;
    return -rows_to_goal(mover, t->end);
}

/* Stable insertion sort, best first; sorts indexes to avoid moving big structs. */
static void sort_turns(const Turn *turns, int *order, int n, int mover) {
    int keys[MAX_TURNS];
    for (int i = 0; i < n; ++i) { order[i] = i; keys[i] = order_key(&turns[i], mover); }
    for (int i = 1; i < n; ++i) {
        int item = order[i], key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { order[j + 1] = order[j]; keys[j + 1] = keys[j]; --j; }
        order[j + 1] = item; keys[j + 1] = key;
    }
}

/* ------------------------------------------------------------- search */
static Turn TURNS[MAX_PLY][MAX_TURNS];
static long long node_limit = DEFAULT_NODES, nodes = 0;
static int time_guard_ms = TIME_GUARD_MS, last_depth = 0, aborted = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000;
}

static int out_of_time(void) { return elapsed_ms() > time_guard_ms; }

static void child_of(const Pitch *pitch, const Turn *t, Pitch *next) {
    *next = *pitch;
    pitch_apply(next, t->digits, t->len);
}

static int negamax(Pitch *pitch, int player, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if (out_of_time()) { fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0; }
    if (depth == 0) return evaluate(pitch, player);
    Turn *turns = TURNS[ply];
    int n = generate_turns(pitch, player, turns);
    if (n == 0) return -(WIN - ply);
    int order[MAX_TURNS];
    sort_turns(turns, order, n, player);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        const Turn *t = &turns[order[i]];
        if (t->outcome == WON) return WIN - ply;
        int value;
        if (t->outcome == LOST) value = -(WIN - ply);
        else {
            Pitch next;
            child_of(pitch, t, &next);
            value = -negamax(&next, 1 - player, depth - 1, -beta, -alpha, ply + 1);
            if (aborted) return 0;
        }
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

static const Turn *search_turn(Pitch *pitch, int player) {
    nodes = 0; aborted = 0; last_depth = 0;
    Turn *turns = TURNS[0];
    int n = generate_turns(pitch, player, turns);
    int order[MAX_TURNS];
    sort_turns(turns, order, n, player);
    int best = order[0];
    if (turns[best].outcome != WON) {
        for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
            int alpha = -INF, bestValue = -INF, bestIndex = 0;
            for (int i = 0; i < n; ++i) {
                const Turn *t = &turns[order[i]];
                int value;
                if (t->outcome == LOST) value = -WIN;
                else {
                    Pitch next;
                    child_of(pitch, t, &next);
                    value = -negamax(&next, 1 - player, depth - 1, -INF, -alpha, 1);
                }
                if (aborted) break;
                if (value > bestValue) { bestValue = value; bestIndex = i; }
                if (value > alpha) alpha = value;
            }
            if (aborted) break;
            best = order[bestIndex];
            last_depth = depth;
            for (int i = bestIndex; i > 0; --i) order[i] = order[i - 1];
            order[0] = best;
            if (bestValue >= WIN - MAX_DEPTH || bestValue <= -(WIN - MAX_DEPTH)) break;
        }
    }
    return &turns[best];
}

/* ------------------------------------------------------------- I/O */
static void trim_right(char *s) {
    int n = (int)strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) s[--n] = 0;
}

int main(void) {
    build_neighbours();
    const char *env = getenv("CG_NODES");
    long long cli_nodes = env ? atoll(env) : -1;
    static char line[1024], opponent_move[1024];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int me = atoi(line);
    static Pitch pitch;
    pitch_reset(&pitch);
    int first_turn = 1;
    while (fgets(line, sizeof line, stdin)) {
        if (!fgets(opponent_move, sizeof opponent_move, stdin)) break;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        trim_right(opponent_move);
        if (strcmp(opponent_move, "-") != 0) pitch_apply(&pitch, opponent_move, (int)strlen(opponent_move));
        node_limit = cli_nodes > 0 ? cli_nodes : (first_turn ? FIRST_TURN_NODES : DEFAULT_NODES);
        time_guard_ms = first_turn ? FIRST_TURN_GUARD_MS : TIME_GUARD_MS;
        const Turn *t = search_turn(&pitch, me);
        char digits[MAX_TURN_LEN + 1];
        memcpy(digits, t->digits, t->len);
        digits[t->len] = 0;
        pitch_apply(&pitch, digits, t->len);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%s\n", digits);
        fflush(stdout);
        first_turn = 0;
    }
    return 0;
}
