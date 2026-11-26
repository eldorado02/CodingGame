/* Back to the Code — C port of cpp/bot.cpp (same rectangles, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { WIDTH = 35, HEIGHT = 20 };
enum { MAX_SIDE = 12 };  /* bigger loops get cut by the opponents */
enum { MAX_OPPONENTS = 3, ROW_BUFFER = 64 };
#define NEUTRAL '.'
#define MINE '0'

typedef struct { int x, y; } Point;
typedef struct { int x0, y0, x1, y1; } Rect;
typedef struct { int gain, cost; } Value;
/* Count of cells of one kind in any rectangle, O(1). */
typedef struct { int sum[HEIGHT + 1][WIDTH + 1]; } Prefix;
typedef struct { Prefix neutral, mine, foe; } Counts;
typedef enum { KIND_NEUTRAL, KIND_MINE, KIND_FOE } Kind;

static int max3(int a, int b, int c) { int m = a > b ? a : b; return m > c ? m : c; }
static int min_i(int a, int b) { return a < b ? a : b; }
static int max_i(int a, int b) { return a > b ? a : b; }

static bool is_kind(char ch, Kind kind) {
    if (kind == KIND_NEUTRAL) return ch == NEUTRAL;
    if (kind == KIND_MINE) return ch == MINE;
    return ch != NEUTRAL && ch != MINE;
}

static void prefix_build(Prefix *p, char grid[HEIGHT][ROW_BUFFER], Kind kind) {
    memset(p->sum, 0, sizeof p->sum);
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            p->sum[y + 1][x + 1] = p->sum[y][x + 1] + p->sum[y + 1][x] - p->sum[y][x] + (is_kind(grid[y][x], kind) ? 1 : 0);
}

static int prefix_count(const Prefix *p, int x0, int y0, int x1, int y1) {
    if (x0 > x1 || y0 > y1) return 0;
    return p->sum[y1 + 1][x1 + 1] - p->sum[y0][x1 + 1] - p->sum[y1 + 1][x0] + p->sum[y0][x0];
}

static int distance_to_rect(Point p, Rect r) { return max3(r.x0 - p.x, 0, p.x - r.x1) + max3(r.y0 - p.y, 0, p.y - r.y1); }

/* Gain and cost of claiming this rectangle; false when it is not worth it. */
static bool rect_value(Rect r, const Counts *c, const Point *opponents, int n_opp, Value *value) {
    if (prefix_count(&c->foe, r.x0, r.y0, r.x1, r.y1)) return false;
    int w = r.x1 - r.x0 + 1, h = r.y1 - r.y0 + 1;
    int gain = prefix_count(&c->neutral, r.x0, r.y0, r.x1, r.y1);
    int border = w > 1 && h > 1 ? w * h - (w - 2) * (h - 2) : w * h;
    int inner_mine = prefix_count(&c->mine, r.x0 + 1, r.y0 + 1, r.x1 - 1, r.y1 - 1);
    int cost = border - (prefix_count(&c->mine, r.x0, r.y0, r.x1, r.y1) - inner_mine);
    if (gain == 0 || cost == 0) return false;
    for (int i = 0; i < n_opp; ++i)
        if (2 * distance_to_rect(opponents[i], r) < cost) return false; /* they could cut the loop */
    value->gain = gain;
    value->cost = cost;
    return true;
}

/* Rectangle with us on its border maximising gain / cost; ties: bigger gain. */
static bool best_rect(Point me, const Counts *c, const Point *opponents, int n_opp, Rect *best) {
    bool found = false;
    Value best_value = {0, 1};
    for (int y0 = max_i(0, me.y - MAX_SIDE + 1); y0 <= me.y; ++y0)
        for (int y1 = me.y; y1 < min_i(HEIGHT, y0 + MAX_SIDE); ++y1)
            for (int x0 = max_i(0, me.x - MAX_SIDE + 1); x0 <= me.x; ++x0)
                for (int x1 = me.x; x1 < min_i(WIDTH, x0 + MAX_SIDE); ++x1) {
                    if (me.x != x0 && me.x != x1 && me.y != y0 && me.y != y1) continue;
                    Rect r = {x0, y0, x1, y1};
                    Value v;
                    if (!rect_value(r, c, opponents, n_opp, &v)) continue;
                    long long lhs = (long long)v.gain * best_value.cost, rhs = (long long)best_value.gain * v.cost;
                    if (!found || lhs > rhs || (lhs == rhs && v.gain > best_value.gain)) {
                        *best = r;
                        best_value = v;
                        found = true;
                    }
                }
    return found;
}

/* Clockwise corner to aim at from our cell on the border. */
static Point next_corner(Point me, Rect r) {
    if (me.y == r.y0 && me.x < r.x1) return (Point){r.x1, r.y0};
    if (me.x == r.x1 && me.y < r.y1) return (Point){r.x1, r.y1};
    if (me.y == r.y1 && me.x > r.x0) return (Point){r.x0, r.y1};
    return (Point){r.x0, r.y0};
}

static bool on_border(Point me, Rect r) {
    return r.x0 <= me.x && me.x <= r.x1 && r.y0 <= me.y && me.y <= r.y1 &&
           (me.x == r.x0 || me.x == r.x1 || me.y == r.y0 || me.y == r.y1);
}

static Point nearest_neutral(Point me, char grid[HEIGHT][ROW_BUFFER]) {
    Point best = me;
    int best_d = -1;
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x) {
            int d = abs(x - me.x) + abs(y - me.y);
            if (grid[y][x] == NEUTRAL && (best_d < 0 || d < best_d)) { best = (Point){x, y}; best_d = d; }
        }
    return best;
}

/* Rectangle followed across turns. */
typedef struct { bool active; Rect rect; } Plan;

static Point decide(Point me, const Point *opponents, int n_opp, char grid[HEIGHT][ROW_BUFFER], Plan *plan) {
    static Counts c;
    prefix_build(&c.neutral, grid, KIND_NEUTRAL);
    prefix_build(&c.mine, grid, KIND_MINE);
    prefix_build(&c.foe, grid, KIND_FOE);
    Value unused;
    if (!plan->active || !on_border(me, plan->rect) || !rect_value(plan->rect, &c, opponents, n_opp, &unused))
        plan->active = best_rect(me, &c, opponents, n_opp, &plan->rect);
    return plan->active ? next_corner(me, plan->rect) : nearest_neutral(me, grid);
}

/* Reads one grid row, padded with neutral cells to WIDTH. */
static bool read_row(char *row) {
    char buffer[ROW_BUFFER];
    if (scanf("%63s", buffer) != 1) return false;
    size_t n = strlen(buffer);
    for (int x = 0; x < WIDTH; ++x) row[x] = (size_t)x < n ? buffer[x] : NEUTRAL;
    row[WIDTH] = '\0';
    return true;
}

int main(void) {
    int opponent_count, round;
    if (scanf("%d", &opponent_count) != 1) return 0;
    if (opponent_count > MAX_OPPONENTS) opponent_count = MAX_OPPONENTS;
    Plan plan = {false, {0, 0, 0, 0}};
    static char grid[HEIGHT][ROW_BUFFER];
    while (scanf("%d", &round) == 1) {
        Point me, opponents[MAX_OPPONENTS];
        int back, n_opp = 0;
        if (scanf("%d %d %d", &me.x, &me.y, &back) != 3) return 0;
        for (int i = 0; i < opponent_count; ++i) {
            Point o;
            if (scanf("%d %d %d", &o.x, &o.y, &back) != 3) return 0;
            if (o.x >= 0) opponents[n_opp++] = o;
        }
        for (int y = 0; y < HEIGHT; ++y)
            if (!read_row(grid[y])) return 0;
        Point target = decide(me, opponents, n_opp, grid, &plan);
        printf("%d %d\n", target.x, target.y);
        fflush(stdout);
    }
    return 0;
}
