/* Yinsh — C port of cpp/bot.cpp (same simulation, same score, same tie-break).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { COLUMNS = 11, NUMBERS = 12 };  /* numbers 1..11 */
enum { ROW = 5, RING_VALUE = 1000, MAX_CELL_TEXT = 3 };  /* ROW: markers in a winning row; "k11" */
enum { MAX_WINDOWS = 512, MAX_ACTIONS = 4096, ACTION_SIZE = 128, MAX_LINE = 16 };
static const int COLUMN_START[COLUMNS] = {2, 1, 1, 1, 1, 2, 2, 3, 4, 5, 7};
static const int COLUMN_SIZE[COLUMNS] = {4, 7, 8, 9, 10, 9, 10, 9, 8, 7, 4};
#define EMPTY '.'
#define MY_RING 'R'
#define MY_MARKER 'S'
#define THEIR_RING 'r'
#define THEIR_MARKER 's'
static const int AXES[3][2] = {{0, 1}, {1, 0}, {1, 1}};
static const int DIRECTIONS[6][2] = {{0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1}};
static const char *STEAL = "STEAL";

typedef struct { int col, num; } Cell;
typedef char Board[COLUMNS][NUMBERS];

static Cell windows[MAX_WINDOWS][ROW];
static int n_windows;

static bool valid(int col, int num) {
    return col >= 0 && col < COLUMNS && num >= COLUMN_START[col] && num < COLUMN_START[col] + COLUMN_SIZE[col];
}

static void build_windows(void) { /* every line of ROW consecutive valid cells */
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num)
            for (int a = 0; a < 3; ++a) {
                bool ok = true;
                for (int k = 0; k < ROW; ++k) {
                    windows[n_windows][k] = (Cell){col + k * AXES[a][0], num + k * AXES[a][1]};
                    ok = ok && valid(windows[n_windows][k].col, windows[n_windows][k].num);
                }
                if (ok) ++n_windows;
            }
}

/* "f10" -> (5, 10); false when it is not a board cell. */
static bool parse_cell(const char *text, size_t len, Cell *out) {
    if (len < 2 || len > MAX_CELL_TEXT) return false;
    for (size_t i = 1; i < len; ++i)
        if (!isdigit((unsigned char)text[i])) return false;
    char digits[MAX_CELL_TEXT + 1] = {0};
    memcpy(digits, text + 1, len - 1);
    out->col = tolower((unsigned char)text[0]) - 'a';
    out->num = atoi(digits);
    return valid(out->col, out->num);
}

static int sign(int v) { return (v > 0) - (v < 0); }

/* Cells from a to b inclusive when they share an axis; returns their count (0 otherwise). */
static int segment(Cell a, Cell b, Cell *line) {
    int dc = b.col - a.col, dn = b.num - a.num;
    if (!(dc == 0 || dn == 0 || dc == dn)) return 0;
    int steps = abs(dc) > abs(dn) ? abs(dc) : abs(dn);
    for (int k = 0; k <= steps; ++k) line[k] = (Cell){a.col + k * sign(dc), a.num + k * sign(dn)};
    return steps + 1;
}

static char flipped(char v) { return v == MY_MARKER ? THEIR_MARKER : v == THEIR_MARKER ? MY_MARKER : v; }

static void apply_move(Board b, Cell src, Cell dst) {
    Cell line[MAX_LINE];
    int n = segment(src, dst, line);
    b[dst.col][dst.num] = b[src.col][src.num];
    b[src.col][src.num] = MY_MARKER;
    for (int k = 1; k + 1 < n; ++k) b[line[k].col][line[k].num] = flipped(b[line[k].col][line[k].num]);
}

/* "xA-BxC" (without its end): clears the markers A..B and the ring C; returns 1 if a ring went. */
static int apply_removal(Board b, const char *text, size_t len) {
    const char *body = text + 1, *end = text + len;
    const char *second_x = memchr(body, 'x', end - body);
    const char *row_end = second_x ? second_x : end;
    const char *dash = memchr(body, '-', row_end - body);
    if (dash && !memchr(dash + 1, '-', row_end - dash - 1)) {
        Cell a, z, line[MAX_LINE];
        if (parse_cell(body, dash - body, &a) && parse_cell(dash + 1, row_end - dash - 1, &z)) {
            int n = segment(a, z, line);
            for (int k = 0; k < n; ++k) {
                char *v = &b[line[k].col][line[k].num];
                if (*v == MY_MARKER || *v == THEIR_MARKER) *v = EMPTY;
            }
        }
    }
    if (!second_x) return 0;
    const char *third_x = memchr(second_x + 1, 'x', end - second_x - 1);
    const char *ring_end = third_x ? third_x : end;
    Cell ring;
    if (parse_cell(second_x + 1, ring_end - second_x - 1, &ring) &&
        (b[ring.col][ring.num] == MY_RING || b[ring.col][ring.num] == THEIR_RING)) {
        b[ring.col][ring.num] = EMPTY;
        return 1;
    }
    return 0;
}

/* Plays our action on the board; returns the rings it removed. */
static int simulate(Board b, const char *action) {
    int removed = 0;
    const char *step = action;
    for (;;) {
        const char *semi = strchr(step, ';');
        size_t len = semi ? (size_t)(semi - step) : strlen(step);
        const char *dash = memchr(step, '-', len);
        Cell src, dst;
        if (len > 0 && step[0] == 'x') {
            removed += apply_removal(b, step, len);
        } else if (dash) {
            if (parse_cell(step, dash - step, &src) && parse_cell(dash + 1, len - (dash - step) - 1, &dst)) apply_move(b, src, dst);
        } else if (parse_cell(step, len, &src) && b[src.col][src.num] == EMPTY) {
            b[src.col][src.num] = MY_RING;
        }
        if (!semi) break;
        step = semi + 1;
    }
    return removed;
}

static int mobility(Board b, char ring) { /* destinations of every ring of this colour */
    int total = 0;
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num) {
            if (!valid(col, num) || b[col][num] != ring) continue;
            for (int d = 0; d < 6; ++d) {
                int c = col + DIRECTIONS[d][0], n = num + DIRECTIONS[d][1];
                bool jumped = false;
                while (valid(c, n) && b[c][n] != MY_RING && b[c][n] != THEIR_RING) {
                    if (b[c][n] == EMPTY) {
                        ++total;
                        if (jumped) break;
                    } else {
                        jumped = true;
                    }
                    c += DIRECTIONS[d][0];
                    n += DIRECTIONS[d][1];
                }
            }
        }
    return total;
}

static int evaluate(Board b) {
    int score = 0;
    for (int w = 0; w < n_windows; ++w) {
        int mine = 0, theirs = 0;
        for (int k = 0; k < ROW; ++k) {
            char v = b[windows[w][k].col][windows[w][k].num];
            mine += v == MY_MARKER;
            theirs += v == THEIR_MARKER;
        }
        if (theirs == 0) score += mine * mine;
        else if (mine == 0) score -= theirs * theirs;
    }
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num)
            if (valid(col, num)) score += (b[col][num] == MY_MARKER) - (b[col][num] == THEIR_MARKER);
    return score + mobility(b, MY_RING) - mobility(b, THEIR_RING);
}

static bool is_steal(const char *action) {
    if (strlen(action) != strlen(STEAL)) return false;
    for (size_t i = 0; action[i]; ++i)
        if (toupper((unsigned char)action[i]) != STEAL[i]) return false;
    return true;
}

static const char *choose(Board board, char actions[][ACTION_SIZE], int n) {
    int best = -1;
    long long best_score = 0;
    for (int i = 0; i < n; ++i) {
        if (is_steal(actions[i])) continue;
        Board after;
        memcpy(after, board, sizeof after);
        long long score = (long long)RING_VALUE * simulate(after, actions[i]) + evaluate(after);
        if (best < 0 || score > best_score) { best = i; best_score = score; }
    }
    if (best >= 0) return actions[best];
    return n > 0 ? actions[0] : STEAL;
}

int main(void) {
    static char actions[MAX_ACTIONS][ACTION_SIZE];
    char token[ACTION_SIZE];
    build_windows();
    if (scanf("%127s", token) != 1) return 0;
    printf("yes\n");
    fflush(stdout);
    int count, action_count;
    while (scanf("%d", &count) == 1) {
        Board board;
        memset(board, EMPTY, sizeof board);
        for (int col = 0; col < count; ++col) {
            if (scanf("%127s", token) != 1) return 0;
            if (col >= COLUMNS) continue;
            for (int k = 0; k < COLUMN_SIZE[col] && token[k]; ++k) board[col][COLUMN_START[col] + k] = token[k];
        }
        if (scanf("%d", &action_count) != 1) return 0;
        if (action_count > MAX_ACTIONS) action_count = MAX_ACTIONS;
        for (int i = 0; i < action_count; ++i)
            if (scanf("%127s", actions[i]) != 1) return 0;
        printf("%s\n", choose(board, actions, action_count < 0 ? 0 : action_count));
        fflush(stdout);
    }
    return 0;
}
