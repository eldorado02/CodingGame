/* Oware Abapa — C port of solutions/python/bot.py (v1 alpha-beta).
 * Rules from github.com/MaximeCheramy/cg-oware (see the Python header).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { HOUSES = 12, SIDE = 6, WIN_SCORE = 24, MAX_TURNS = 200, TOTAL_SEEDS = 48 };
enum { SCORE_WEIGHT = 4 };  /* captured seed vs seed still on our side */
enum { FIRST_TURN_NODES = 600000, TURN_NODES = 60000, FIRST_TURN_MS = 700, TURN_MS = 35 };
#define WIN 1000000
#define INF (1 << 30)
#define MAX_DEPTH 64

typedef struct { int h[HOUSES]; int mine, theirs, turn; } State;  /* h[0..5] = side to move */

static int legal_moves(const State *s, int *out) {
    int opponent_has_seeds = 0, n = 0;
    for (int i = SIDE; i < HOUSES; ++i) if (s->h[i]) { opponent_has_seeds = 1; break; }
    for (int i = 0; i < SIDE; ++i)
        if (s->h[i] > 0 && (opponent_has_seeds || s->h[i] >= SIDE - i)) out[n++] = i;
    return n;
}

/* next state from the opponent's point of view */
static State apply_move(const State *s, int i) {
    int h[HOUSES];
    memcpy(h, s->h, sizeof h);
    int seeds = h[i], pos = i;
    h[i] = 0;
    while (seeds) {
        pos = (pos + 1) % HOUSES;
        if (pos == i) continue;
        ++h[pos];
        --seeds;
    }
    int captured[HOUSES] = {0};
    for (int cur = pos; cur >= SIDE && (h[cur] == 2 || h[cur] == 3); --cur) captured[cur] = 1;
    int grand_slam = 1;
    for (int j = SIDE; j < HOUSES; ++j) if (h[j] && !captured[j]) { grand_slam = 0; break; }
    int gain = 0;
    if (!grand_slam)
        for (int j = SIDE; j < HOUSES; ++j) if (captured[j]) { gain += h[j]; h[j] = 0; }
    State t;
    for (int j = 0; j < HOUSES; ++j) t.h[j] = h[(j + SIDE) % HOUSES];
    t.mine = s->theirs;
    t.theirs = s->mine + gain;
    t.turn = s->turn + 1;
    return t;
}

static int side_seeds(const State *s, int from) {
    int n = 0;
    for (int j = from; j < from + SIDE; ++j) n += s->h[j];
    return n;
}

static int evaluate(const State *s) {
    return SCORE_WEIGHT * (s->mine - s->theirs) + side_seeds(s, 0) - side_seeds(s, SIDE);
}

static long long node_limit = TURN_NODES, nodes = 0;
static int time_ms = TURN_MS, aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int final_score(int mine, int theirs, int ply) {
    return mine > theirs ? WIN - ply : mine < theirs ? -(WIN - ply) : 0;
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > time_ms) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    if (s->theirs > WIN_SCORE) return -(WIN - ply);
    if (s->mine > WIN_SCORE) return WIN - ply;
    int mv[SIDE];
    int n = legal_moves(s, mv);
    if (!n) return final_score(s->mine + side_seeds(s, 0) + side_seeds(s, SIDE), s->theirs, ply);
    if (s->turn >= MAX_TURNS) return final_score(s->mine, s->theirs, ply);
    if (depth == 0) return evaluate(s);
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

static int search(const State *root) {
    int mv[SIDE];
    int n = legal_moves(root, mv), best = mv[0];
    last_depth = 0; nodes = 0; aborted = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, best_v = -INF, cur_idx = 0;
        for (int i = 0; i < n; ++i) {
            State t = apply_move(root, mv[i]);
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
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    int me_id = -1, my_score = 0, their_score = 0, seeds_after_my_move = TOTAL_SEEDS, my_turns = 0;
    for (;;) {
        State s;
        for (int j = 0; j < HOUSES; ++j) if (scanf("%d", &s.h[j]) != 1) return 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        if (me_id < 0) {
            me_id = 0;
            for (int j = 0; j < HOUSES; ++j) if (s.h[j] != 4) { me_id = 1; break; }
        }
        /* the opponent's captures are the seeds that vanished since our move */
        their_score += seeds_after_my_move - (side_seeds(&s, 0) + side_seeds(&s, SIDE));
        s.mine = my_score;
        s.theirs = their_score;
        s.turn = 2 * my_turns + me_id;
        node_limit = cli_nodes > 0 ? cli_nodes : (my_turns == 0 ? FIRST_TURN_NODES : TURN_NODES);
        time_ms = my_turns == 0 ? FIRST_TURN_MS : TURN_MS;
        int tmp[SIDE];
        if (!legal_moves(&s, tmp)) { printf("0\n"); fflush(stdout); continue; }
        int m = search(&s);
        State next = apply_move(&s, m);
        my_score = next.theirs;
        seeds_after_my_move = side_seeds(&next, 0) + side_seeds(&next, SIDE);
        ++my_turns;
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        printf("%d\n", m);  /* relative house: the referee adds 6 for player 1 */
        fflush(stdout);
    }
}
