/* Abalone — C port of cpp/bot.cpp (same simulation, same score, same tie-break).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { RADIUS = 4, ROWS = 2 * RADIUS + 1, DIRS = 6, MAX_MOVES = 1024, LINE_MAX = 16, ROW_BUFFER = 32 };
static const int DIRECTIONS[DIRS][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};
static const long long CAPTURE_WEIGHT = 10000, CENTER_WEIGHT = 10, EDGE_WEIGHT = 5, COHESION_WEIGHT = 2;
#define EMPTY '0'

typedef char Board[ROWS][ROWS]; /* [q + RADIUS][r + RADIUS] */
typedef struct { int q, r; } Hex;

static int max_i(int a, int b) { return a > b ? a : b; }
static int min_i(int a, int b) { return a < b ? a : b; }
static int q_min(int r) { return max_i(-RADIUS, -r - RADIUS); }
static int q_max(int r) { return min_i(RADIUS, -r + RADIUS); }
static Hex to_axial(int x, int y) { int r = y - RADIUS; return (Hex){x + q_min(r), r}; }
static int on_board(Hex h) { return h.r >= -RADIUS && h.r <= RADIUS && h.q >= q_min(h.r) && h.q <= q_max(h.r); }
static int distance(int q, int r) { return (abs(q) + abs(r) + abs(q + r)) / 2; }
static int sign(int v) { return (v > 0) - (v < 0); }
static char *cell(Board b, Hex h) { return &b[h.q + RADIUS][h.r + RADIUS]; }

/* Plays our move on `b`; returns the enemy marbles pushed off. */
static int play(Board b, char me, const int *m) {
    Hex first = to_axial(m[0], m[1]), last = to_axial(m[2], m[3]);
    int steps = distance(last.q - first.q, last.r - first.r);
    Hex axis = {sign(last.q - first.q), sign(last.r - first.r)};
    Hex line[LINE_MAX];
    int n = 0;
    for (int k = 0; k <= steps && n < LINE_MAX; ++k) {
        Hex h = {first.q + k * axis.q, first.r + k * axis.r};
        if (on_board(h)) line[n++] = h;
    }
    if (!n) return 0;
    int d = ((m[4] % DIRS) + DIRS) % DIRS, dq = DIRECTIONS[d][0], dr = DIRECTIONS[d][1];
    int in_line = steps == 0 || (dq == axis.q && dr == axis.r) || (dq == -axis.q && dr == -axis.r);
    Board before;
    memcpy(before, b, sizeof before);
    int captured = 0;
    if (in_line) {
        Hex head = line[0], chain[ROWS];
        for (int k = 1; k < n; ++k)
            if (line[k].q * dq + line[k].r * dr > head.q * dq + head.r * dr) head = line[k];
        int c = 0;
        Hex cursor = {head.q + dq, head.r + dr};
        while (on_board(cursor) && *cell(before, cursor) != EMPTY && *cell(before, cursor) != me && c < ROWS) {
            chain[c++] = cursor;
            cursor = (Hex){cursor.q + dq, cursor.r + dr};
        }
        for (int k = c - 1; k >= 0; --k) {
            Hex target = {chain[k].q + dq, chain[k].r + dr};
            if (on_board(target)) *cell(b, target) = *cell(before, chain[k]);
            else ++captured;
            *cell(b, chain[k]) = EMPTY;
        }
    }
    for (int k = 0; k < n; ++k) *cell(b, line[k]) = EMPTY;
    for (int k = 0; k < n; ++k) {
        Hex target = {line[k].q + dq, line[k].r + dr};
        if (on_board(target)) *cell(b, target) = me;
    }
    return captured;
}

static long long evaluate(Board b, char me) {
    long long score = 0;
    for (int r = -RADIUS; r <= RADIUS; ++r)
        for (int q = q_min(r); q <= q_max(r); ++q) {
            char v = *cell(b, (Hex){q, r});
            if (v == me) {
                score -= CENTER_WEIGHT * distance(q, r);
                for (int d = 0; d < DIRS; ++d) {
                    Hex nb = {q + DIRECTIONS[d][0], r + DIRECTIONS[d][1]};
                    if (on_board(nb) && *cell(b, nb) == me) score += COHESION_WEIGHT;
                }
            } else if (v != EMPTY) {
                score += EDGE_WEIGHT * distance(q, r);
            }
        }
    return score;
}

int main(void) {
    static int moves[MAX_MOVES][5];
    char me[ROW_BUFFER], row[ROW_BUFFER];
    int my_score, their_score, last[5], count;
    if (scanf("%31s", me) != 1) return 0;
    while (scanf("%d %d", &my_score, &their_score) == 2) {
        Board board;
        memset(board, EMPTY, sizeof board);
        for (int y = 0; y < ROWS; ++y) {
            if (scanf("%31s", row) != 1) return 0;
            int r = y - RADIUS, width = q_max(r) - q_min(r) + 1, len = (int)strlen(row);
            for (int x = 0; x < width; ++x) *cell(board, to_axial(x, y)) = x < len ? row[x] : EMPTY;
        }
        for (int k = 0; k < 5; ++k)
            if (scanf("%d", &last[k]) != 1) return 0;
        if (scanf("%d", &count) != 1) return 0;
        if (count > MAX_MOVES) count = MAX_MOVES;
        for (int i = 0; i < count; ++i)
            for (int k = 0; k < 5; ++k)
                if (scanf("%d", &moves[i][k]) != 1) return 0;
        int best = -1;
        long long best_score = 0;
        for (int i = 0; i < count; ++i) {
            Board after;
            memcpy(after, board, sizeof after);
            long long score = CAPTURE_WEIGHT * play(after, me[0], moves[i]) + evaluate(after, me[0]);
            if (best < 0 || score > best_score) { best = i; best_score = score; }
        }
        if (best < 0) printf("0 0 0 0 0\n");
        else printf("%d %d %d %d %d\n", moves[best][0], moves[best][1], moves[best][2], moves[best][3], moves[best][4]);
        fflush(stdout);
    }
    return 0;
}
