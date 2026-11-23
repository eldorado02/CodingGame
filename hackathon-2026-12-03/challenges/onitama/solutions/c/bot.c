/* Onitama 5x5 — C port of solutions/python/bot.py (v1 alpha-beta).
 * Cells: index = (row - 1) * 5 + x, row 1 = last input line; cards stored in
 * player 0's frame (player 1 negates).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 5
#define CELLS (SIZE * SIZE)
#define MAX_CARDS 64
#define MAX_MOVES (2 * CELLS * 4)
#define MAX_DEPTH 64
#define WIN 1000000
#define INF (1 << 30)

enum { TURN_NODES = 30000, TURN_MS = 35, STUDENT_VALUE = 100, SHRINE_DISTANCE_WEIGHT = 4 };
enum { EMPTY = 0, MASTER = 1, STUDENT = 2 };
static const int SHRINE[2] = {2 + 4 * SIZE, 2};  /* player 0 targets C5, player 1 targets C1 */

typedef struct { int card, src, dst; } Move;  /* src = -1 for PASS */
typedef struct { int kind[CELLS], owner[CELLS]; int hands[2][2], center, side; } State;

static int card_dx[MAX_CARDS][4], card_dy[MAX_CARDS][4], card_moves[MAX_CARDS];

static int cell_index(int x, int row) { return (row - 1) * SIZE + x; }

static int gen_moves(const State *s, Move *out) {
    int n = 0, sign = s->side == 0 ? 1 : -1;
    for (int h = 0; h < 2; ++h) {
        int card = s->hands[s->side][h];
        for (int src = 0; src < CELLS; ++src) {
            if (s->kind[src] == EMPTY || s->owner[src] != s->side) continue;
            int x = src % SIZE, row = src / SIZE + 1;
            for (int k = 0; k < card_moves[card]; ++k) {
                int nx = x + sign * card_dx[card][k], nrow = row + sign * card_dy[card][k];
                if (nx < 0 || nx >= SIZE || nrow < 1 || nrow > SIZE) continue;
                int dst = cell_index(nx, nrow);
                if (s->kind[dst] != EMPTY && s->owner[dst] == s->side) continue;
                out[n].card = card; out[n].src = src; out[n].dst = dst; ++n;
            }
        }
    }
    if (!n) for (int h = 0; h < 2; ++h) { out[n].card = s->hands[s->side][h]; out[n].src = out[n].dst = -1; ++n; }
    return n;
}

static State apply_move(const State *s, Move m) {
    State t = *s;
    if (m.src >= 0) {
        t.kind[m.dst] = s->kind[m.src];
        t.owner[m.dst] = s->owner[m.src];
        t.kind[m.src] = EMPTY;
    }
    for (int h = 0; h < 2; ++h)
        if (t.hands[s->side][h] == m.card) { t.hands[s->side][h] = s->center; break; }
    t.center = m.card;
    t.side = 1 - s->side;
    return t;
}

static int master_of(const State *s, int p) {
    for (int i = 0; i < CELLS; ++i) if (s->kind[i] == MASTER && s->owner[i] == p) return i;
    return -1;
}

static int winner(const State *s) {
    for (int p = 0; p < 2; ++p) {
        int m = master_of(s, p);
        if (m < 0) return 1 - p;
        if (m == SHRINE[p]) return p;
    }
    return -1;
}

static int distance(int a, int b) {
    int dx = abs(a % SIZE - b % SIZE), dy = abs(a / SIZE - b / SIZE);
    return dx > dy ? dx : dy;
}

static int side_score(const State *s, int p) {
    int students = 0;
    for (int i = 0; i < CELLS; ++i) students += s->kind[i] == STUDENT && s->owner[i] == p;
    return STUDENT_VALUE * students - SHRINE_DISTANCE_WEIGHT * distance(master_of(s, p), SHRINE[p]);
}

static int evaluate(const State *s) { return side_score(s, s->side) - side_score(s, 1 - s->side); }

static int order_key(const State *s, Move m) { return (m.src >= 0 && s->kind[m.dst] != EMPTY) ? 1 : 0; }

static void order_moves(const State *s, Move *mv, int n) {  /* stable, captures first */
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = order_key(s, x), j = i - 1;
        while (j >= 0 && order_key(s, mv[j]) < k) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

static long long node_limit = TURN_NODES, nodes = 0;
static int aborted = 0, last_depth = 0;
static struct timespec t0;

static long elapsed_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long)((t.tv_sec - t0.tv_sec) * 1000 + (t.tv_nsec - t0.tv_nsec) / 1000000);
}

static int negamax(const State *s, int depth, int alpha, int beta, int ply) {
    if (++nodes > node_limit) { aborted = 1; return 0; }
    if ((nodes & 1023) == 0 && elapsed_ms() > TURN_MS) {
        fprintf(stderr, "TIMEGUARD\n"); aborted = 1; return 0;
    }
    int w = winner(s);
    if (w >= 0) return w == s->side ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = gen_moves(s, mv);
    order_moves(s, mv, n);
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
    order_moves(root, mv, n);
    Move best = mv[0];
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

static int square(const char *t) {
    if (t[0] < 'A' || t[0] > 'E' || t[1] < '1' || t[1] > '5') return -1;
    return cell_index(t[0] - 'A', t[1] - '0');
}

int main(void) {
    long long cli_nodes = -1;
    const char *env = getenv("CG_NODES");
    if (env) cli_nodes = atoll(env);
    char line[256];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int me = atoi(line);
    static Move legal[MAX_MOVES * 2];
    for (;;) {
        State s;
        for (int li = 0; li < SIZE; ++li) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            if (li == 0) clock_gettime(CLOCK_MONOTONIC, &t0);
            size_t len = strcspn(line, "\r\n");
            int row = SIZE - li;
            for (int x = 0; x < SIZE; ++x) {
                char ch = (size_t)x < len ? line[x] : '.';
                int i = cell_index(x, row);
                s.kind[i] = EMPTY; s.owner[i] = 0;
                if (ch == 'W' || ch == 'w') { s.kind[i] = ch == 'W' ? MASTER : STUDENT; s.owner[i] = 0; }
                else if (ch == 'B' || ch == 'b') { s.kind[i] = ch == 'B' ? MASTER : STUDENT; s.owner[i] = 1; }
            }
        }
        int hand_count[2] = {0, 0};
        s.center = -1;
        for (int k = 0; k < 5; ++k) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            int v[10] = {0};
            sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9]);
            int owner = v[0], id = v[1];
            if (id < 0 || id >= MAX_CARDS) continue;
            int user = owner == -1 ? me : owner;  /* the centre card's next user is us */
            int sign = user == 0 ? 1 : -1, m = 0;
            for (int j = 2; j < 10; j += 2)
                if (v[j] || v[j + 1]) { card_dx[id][m] = sign * v[j]; card_dy[id][m] = sign * v[j + 1]; ++m; }
            card_moves[id] = m;
            if (owner == -1) s.center = id;
            else if ((owner == 0 || owner == 1) && hand_count[owner] < 2) s.hands[owner][hand_count[owner]++] = id;
        }
        if (!fgets(line, sizeof line, stdin)) return 0;
        int n = atoi(line), nl = 0;
        for (int i = 0; i < n; ++i) {
            if (!fgets(line, sizeof line, stdin)) return 0;
            int card;
            char act[32] = {0};
            if (sscanf(line, "%d %31s", &card, act) != 2) continue;
            if (strcmp(act, "PASS") == 0) { legal[nl].card = card; legal[nl].src = legal[nl].dst = -1; ++nl; continue; }
            if (strlen(act) < 4) continue;
            int a = square(act), b = square(act + 2);
            if (a >= 0 && b >= 0) { legal[nl].card = card; legal[nl].src = a; legal[nl].dst = b; ++nl; }
        }
        s.side = me;
        node_limit = cli_nodes > 0 ? cli_nodes : TURN_NODES;
        if (nl == 0) { printf("%d PASS\n", s.hands[me][0]); fflush(stdout); continue; }
        Move m = search(&s, legal, nl);
        fprintf(stderr, "d=%d n=%lld ms=%ld\n", last_depth, nodes, elapsed_ms());
        if (m.src < 0) printf("%d PASS\n", m.card);
        else printf("%d %c%c%c%c\n", m.card, 'A' + m.src % SIZE, '1' + m.src / SIZE, 'A' + m.dst % SIZE, '1' + m.dst / SIZE);
        fflush(stdout);
    }
}
