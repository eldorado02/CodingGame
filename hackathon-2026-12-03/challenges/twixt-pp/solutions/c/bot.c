/* TwixT-PP 12x12 (CodinGame) — C port of solutions/python/bot.py (v1).
 * Same geometry, evaluation, move order and tie-breaks.
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { SIZE = 12, CELLS = SIZE * SIZE };
enum { EMPTY = 0, ME = 1, OPP = 2 };
enum { TOP_BOTTOM = 0, LEFT_RIGHT = 1 };
enum { MAX_SEGMENTS = 440, MAX_CROSSES = 16, UNREACHABLE = 99 };
enum { WIN_SCORE = 1000000 };
enum { DISTANCE_WEIGHT = 100 }; /* one peg of path length outweighs every link count */
enum { LINK_WEIGHT = 3 };       /* more links = more alternative routes */
enum { DEFAULT_TOP_K = 16 };    /* 2-ply candidates (300 ms per turn on CodinGame) */
enum { SWAP_LOW = 3, SWAP_HIGH = 8 };      /* swap an opening peg inside this square */
enum { OPENING_CELL = 3 * SIZE + 5 };      /* "F4" */
static const int KNIGHT_DX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
static const int KNIGHT_DY[8] = {2, 1, -1, -2, -2, -1, 1, 2};

/* ------------------------------------------------------------- geometry */
typedef struct { int a, b; } Segment;
static Segment SEGMENTS[MAX_SEGMENTS];
static int SEGMENT_COUNT = 0;
static int SEG_ID[CELLS][CELLS];
static int NEIGHBOUR[CELLS][8], NEIGHBOUR_SEG[CELLS][8], NEIGHBOUR_COUNT[CELLS];
static int CROSSES[MAX_SEGMENTS][MAX_CROSSES], CROSS_COUNT[MAX_SEGMENTS];

static int imin(int a, int b) { return a < b ? a : b; }
static int imax(int a, int b) { return a > b ? a : b; }
static int cell_of(int x, int y) { return y * SIZE + x; }
static int x_of(int cell) { return cell % SIZE; }
static int y_of(int cell) { return cell / SIZE; }

static void name_of(int cell, char *out) { sprintf(out, "%c%d", 'A' + x_of(cell), y_of(cell) + 1); }
static int parse_cell(const char *t) { return cell_of(t[0] - 'A', atoi(t + 1) - 1); }

static int orientation(int a, int b, int c) {
    long v = (long)(x_of(b) - x_of(a)) * (y_of(c) - y_of(a)) - (long)(y_of(b) - y_of(a)) * (x_of(c) - x_of(a));
    return (v > 0) - (v < 0);
}

static int bounding_boxes_overlap(Segment s, Segment t) {
    return imin(x_of(s.a), x_of(s.b)) <= imax(x_of(t.a), x_of(t.b)) && imin(x_of(t.a), x_of(t.b)) <= imax(x_of(s.a), x_of(s.b))
        && imin(y_of(s.a), y_of(s.b)) <= imax(y_of(t.a), y_of(t.b)) && imin(y_of(t.a), y_of(t.b)) <= imax(y_of(s.a), y_of(s.b));
}

/* Proper crossing of two segments that share no endpoint. */
static int segments_cross(Segment s, Segment t) {
    if (s.a == t.a || s.a == t.b || s.b == t.a || s.b == t.b) return 0;
    return orientation(s.a, s.b, t.a) * orientation(s.a, s.b, t.b) < 0
        && orientation(t.a, t.b, s.a) * orientation(t.a, t.b, s.b) < 0;
}

static void build_geometry(void) {
    memset(SEG_ID, -1, sizeof SEG_ID);
    for (int cell = 0; cell < CELLS; ++cell) {
        for (int k = 0; k < 8; ++k) {
            int nx = x_of(cell) + KNIGHT_DX[k], ny = y_of(cell) + KNIGHT_DY[k];
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
            int other = cell_of(nx, ny), lo = imin(cell, other), hi = imax(cell, other);
            if (SEG_ID[lo][hi] < 0) {
                SEG_ID[lo][hi] = SEG_ID[hi][lo] = SEGMENT_COUNT;
                SEGMENTS[SEGMENT_COUNT].a = lo;
                SEGMENTS[SEGMENT_COUNT].b = hi;
                ++SEGMENT_COUNT;
            }
            int n = NEIGHBOUR_COUNT[cell]++;
            NEIGHBOUR[cell][n] = other;
            NEIGHBOUR_SEG[cell][n] = SEG_ID[lo][hi];
        }
    }
    for (int i = 0; i < SEGMENT_COUNT; ++i)
        for (int j = i + 1; j < SEGMENT_COUNT; ++j)
            if (bounding_boxes_overlap(SEGMENTS[i], SEGMENTS[j]) && segments_cross(SEGMENTS[i], SEGMENTS[j])) {
                CROSSES[i][CROSS_COUNT[i]++] = j;
                CROSSES[j][CROSS_COUNT[j]++] = i;
            }
}

/* ------------------------------------------------------------- state */
/* board[cell] in {EMPTY, ME, OPP}; linked[p][seg] = p owns that link;
 * cut[p][seg] = number of opponent links crossing seg (unusable when > 0). */
typedef struct {
    int side[3];
    int8_t board[CELLS];
    int8_t linked[3][MAX_SEGMENTS];
    int16_t cut[3][MAX_SEGMENTS];
    int link_count[3];
} State;

static void state_init(State *s, int my_side) {
    memset(s, 0, sizeof *s);
    s->side[ME] = my_side;
    s->side[OPP] = 1 - my_side;
}

static void add_link(State *s, int player, int seg) {
    if (s->linked[player][seg]) return;
    s->linked[player][seg] = 1;
    ++s->link_count[player];
    int other = player == ME ? OPP : ME;
    for (int i = 0; i < CROSS_COUNT[seg]; ++i) ++s->cut[other][CROSSES[seg][i]];
}

/* Drop a peg and create every legal link, as the referee does. */
static void place(State *s, int player, int cell) {
    s->board[cell] = (int8_t)player;
    for (int i = 0; i < NEIGHBOUR_COUNT[cell]; ++i) {
        int seg = NEIGHBOUR_SEG[cell][i];
        if (s->board[NEIGHBOUR[cell][i]] == player && s->cut[player][seg] == 0) add_link(s, player, seg);
    }
}

static int can_play(const State *s, int player, int cell) {
    if (s->board[cell] != EMPTY) return 0;
    int x = x_of(cell), y = y_of(cell);
    int x_edge = x == 0 || x == SIZE - 1, y_edge = y == 0 || y == SIZE - 1;
    if (x_edge && y_edge) return 0; /* corners are not holes */
    return s->side[player] == TOP_BOTTOM ? !x_edge : !y_edge;
}

static int legal_moves(const State *s, int player, int *out) {
    int n = 0;
    for (int c = 0; c < CELLS; ++c) if (can_play(s, player, c)) out[n++] = c;
    return n;
}

/* ------------------------------------------------------------- evaluation */
static int on_start_border(int side, int cell) { return side == TOP_BOTTOM ? y_of(cell) == 0 : x_of(cell) == 0; }
static int on_goal_border(int side, int cell) { return side == TOP_BOTTOM ? y_of(cell) == SIZE - 1 : x_of(cell) == SIZE - 1; }

/* Fewest pegs `player` still needs to connect his borders (0-1 BFS, ring deque). */
static int path_distance(const State *s, int player) {
    enum { RING = 4 * CELLS * 8 };
    static int ring[RING];
    int head = RING / 2, tail = RING / 2;
    int dist[CELLS];
    int side = s->side[player];
    for (int c = 0; c < CELLS; ++c) dist[c] = UNREACHABLE;
    for (int c = 0; c < CELLS; ++c) {
        if (!on_start_border(side, c)) continue;
        if (s->board[c] == player) { dist[c] = 0; ring[--head] = c; }
        else if (can_play(s, player, c)) { dist[c] = 1; ring[tail++] = c; }
    }
    while (head < tail) {
        int cell = ring[head++], d = dist[cell];
        if (on_goal_border(side, cell)) return d;
        for (int i = 0; i < NEIGHBOUR_COUNT[cell]; ++i) {
            int seg = NEIGHBOUR_SEG[cell][i], other = NEIGHBOUR[cell][i];
            if (s->cut[player][seg]) continue;
            int owner = s->board[other];
            if (owner == player) {
                /* two own pegs not linked: an older opponent link separates them */
                if (s->board[cell] == player && !s->linked[player][seg]) continue;
                if (d < dist[other]) { dist[other] = d; ring[--head] = other; }
            } else if (owner == EMPTY && can_play(s, player, other) && d + 1 < dist[other]) {
                dist[other] = d + 1;
                ring[tail++] = other;
            }
        }
    }
    return UNREACHABLE;
}

static int evaluate(const State *s) {
    int mine = path_distance(s, ME);
    if (mine == 0) return WIN_SCORE;
    int theirs = path_distance(s, OPP);
    if (theirs == 0) return -WIN_SCORE;
    int links = s->link_count[ME] - s->link_count[OPP];
    return DISTANCE_WEIGHT * (theirs - mine) + LINK_WEIGHT * links;
}

/* ------------------------------------------------------------- search */
/* Insertion sort on integer keys, descending, stable (same in 4 languages). */
static void stable_sort_desc(int *items, int *keys, int n) {
    for (int i = 1; i < n; ++i) {
        int item = items[i], key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { items[j + 1] = items[j]; keys[j + 1] = keys[j]; --j; }
        items[j + 1] = item;
        keys[j + 1] = key;
    }
}

/* Lowest score the opponent can force; stops once it is <= floor. */
static int worst_reply(const State *s, int floor) {
    int moves[CELLS], n = legal_moves(s, OPP, moves), worst = WIN_SCORE;
    State child;
    for (int i = 0; i < n; ++i) {
        child = *s;
        place(&child, OPP, moves[i]);
        int score = evaluate(&child);
        if (score < worst) {
            worst = score;
            if (worst <= floor) break;
        }
    }
    return worst;
}

static int choose_move(const State *s, int top_k, char *info) {
    int moves[CELLS], scores[CELLS] = {0}, n = legal_moves(s, ME, moves);
    State child;
    for (int i = 0; i < n; ++i) {
        child = *s;
        place(&child, ME, moves[i]);
        scores[i] = evaluate(&child);
    }
    stable_sort_desc(moves, scores, n);
    if (scores[0] == WIN_SCORE) { strcpy(info, "win"); return moves[0]; }
    int best_cell = moves[0], best_score = -WIN_SCORE - 1;
    for (int i = 0; i < n && i < top_k; ++i) {
        child = *s;
        place(&child, ME, moves[i]);
        int score = worst_reply(&child, best_score);
        if (score > best_score) { best_score = score; best_cell = moves[i]; }
    }
    sprintf(info, "score=%d", best_score);
    return best_cell;
}

/* ------------------------------------------------------------- I/O */
static int should_swap(int cell) {
    int x = x_of(cell), y = y_of(cell);
    return x >= SWAP_LOW && x <= SWAP_HIGH && y >= SWAP_LOW && y <= SWAP_HIGH;
}

static int read_line(char *buf, int size) {
    if (!fgets(buf, size, stdin)) return 0;
    int n = (int)strlen(buf);
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r' || buf[n - 1] == ' ')) buf[--n] = 0;
    return 1;
}

static int read_int(void) {
    char buf[64];
    return read_line(buf, sizeof buf) ? atoi(buf) : 0;
}

/* Rebuild the state from one turn of input (pegs, then links, for each player). */
static void read_state(State *s) {
    const int players[2] = {ME, OPP};
    char line[64], a[16], b[16];
    for (int k = 0; k < 2; ++k) {
        int player = players[k];
        int pegs = read_int();
        for (int i = 0; i < pegs; ++i) {
            read_line(line, sizeof line);
            sscanf(line, "%15s", a);
            s->board[parse_cell(a)] = (int8_t)player;
        }
        int links = read_int();
        for (int i = 0; i < links; ++i) {
            read_line(line, sizeof line);
            sscanf(line, "%15s %15s", a, b);
            add_link(s, player, SEG_ID[parse_cell(a)][parse_cell(b)]);
        }
    }
}

static void decide(const State *s, const char *last, int first_turn, int top_k, char *move, char *info) {
    int my_pegs = 0, their_pegs = 0, their_cell = -1;
    for (int c = 0; c < CELLS; ++c) {
        if (s->board[c] == ME) ++my_pegs;
        if (s->board[c] == OPP) { ++their_pegs; if (their_cell < 0) their_cell = c; }
    }
    int moves[CELLS];
    if (strcmp(last, "FIRST") == 0) { strcpy(info, "opening"); name_of(OPENING_CELL, move); }
    else if (first_turn && my_pegs == 0 && their_pegs == 1 && should_swap(their_cell)) { strcpy(info, "swap"); strcpy(move, "SWAP"); }
    else if (legal_moves(s, ME, moves) == 0) { strcpy(info, "no move"); name_of(0, move); }
    else name_of(choose_move(s, top_k, info), move);
}

int main(void) {
    build_geometry();
    const char *env = getenv("CG_NODES");
    int top_k = env ? atoi(env) : DEFAULT_TOP_K;
    int my_side = -1;
    char last[64];
    static State s;
    while (read_line(last, sizeof last) && last[0]) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        int first_turn = my_side < 0;
        if (first_turn) my_side = strcmp(last, "FIRST") == 0 ? TOP_BOTTOM : LEFT_RIGHT;
        state_init(&s, my_side);
        read_state(&s);
        char move[16], info[64];
        decide(&s, last, first_turn, top_k, move, info);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        fprintf(stderr, "%s ms=%ld\n", info, (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000);
        printf("%s\n", move);
        fflush(stdout);
    }
    return 0;
}
