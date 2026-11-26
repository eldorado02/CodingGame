/* Tulips and Daisies — C port of cpp/bot.cpp (same score, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum { ROW = 4, MAX_SIDE = 16, MAX_CELLS = MAX_SIDE * MAX_SIDE, NOT_ALLOWED = -1, NAME_SIZE = 32 };
static const int DIRECTIONS[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
static const long long NET_WEIGHT = 10, THREAT_WEIGHT = 10;
static const long long ATTACK[ROW + 1] = {0, 1, 4, 16, 64};   /* by our flowers in a clean window */
static const long long DEFENCE[ROW + 1] = {0, 0, 2, 12, 48};  /* by their flowers in a window we block */
static const long long RESERVE_TURNS = 10;
static const long long GAP_WEIGHT = 5;  /* a gap harvest is worth half of a sure one */
#define SOIL 'S'
#define GRASS 'G'
#define ROCKS 'R'

static int width, height, cost_soil, cost_grass, cost_rocks, cost_flower;
static char mine, theirs;
static char grid[MAX_SIDE][MAX_SIDE + 1];
static int placed_r = -1, placed_c = -1;  /* cell holding our flower in the current hypothesis */

static long long fib_sum(int n) {
    long long total = 0, a = 1, b = 1;
    for (int i = 0; i < n; ++i) {
        total += a;
        long long next = a + b;
        a = b;
        b = next;
    }
    return total;
}

static bool inside(int r, int c) { return r >= 0 && r < height && c >= 0 && c < width; }
static char at(int r, int c) { return r == placed_r && c == placed_c ? mine : grid[r][c]; }

/* Planting cost of `flower` on `tile`, NOT_ALLOWED on its own flower. */
static int cost(char tile, char flower) {
    if (tile == flower) return NOT_ALLOWED;
    if (tile == SOIL) return cost_soil;
    if (tile == GRASS) return cost_grass;
    if (tile == ROCKS) return cost_rocks;
    return cost_flower;
}

/* Gold earned by planting `flower` at (r, c). */
static long long harvest(int r, int c, char flower) {
    int count = 1;
    bool harvested = false;
    for (int d = 0; d < 4; ++d) {
        int length = 1;
        for (int sign = 1; sign >= -1; sign -= 2) {
            int nr = r + sign * DIRECTIONS[d][0], nc = c + sign * DIRECTIONS[d][1];
            while (inside(nr, nc) && at(nr, nc) == flower) {
                ++length;
                nr += sign * DIRECTIONS[d][0];
                nc += sign * DIRECTIONS[d][1];
            }
        }
        if (length >= ROW) { count += length - 1; harvested = true; }
    }
    return harvested ? fib_sum(count) : 0;
}

/* Our flowers after (r, c) along (dr, dc); (r, c) becomes the first other cell. */
static int run_end(int *r, int *c, int dr, int dc) {
    int count = 0;
    *r += dr;
    *c += dc;
    while (inside(*r, *c) && at(*r, *c) == mine) { ++count; *r += dr; *c += dc; }
    return count;
}

/* Best harvest - cost of filling one cell next to our run through (r, c). */
static long long gap_potential(int r, int c) {
    long long best = 0;
    for (int d = 0; d < 4; ++d) {
        int dr = DIRECTIONS[d][0], dc = DIRECTIONS[d][1];
        int fr = r, fc = c, br = r, bc = c;
        int forward = run_end(&fr, &fc, dr, dc), backward = run_end(&br, &bc, -dr, -dc);
        int length = 1 + forward + backward;
        int ends[2][4] = {{fr, fc, dr, dc}, {br, bc, -dr, -dc}};
        for (int k = 0; k < 2; ++k) {
            if (!inside(ends[k][0], ends[k][1])) continue;
            int er = ends[k][0], ec = ends[k][1];
            int total = length + run_end(&er, &ec, ends[k][2], ends[k][3]) + 1;
            if (total < ROW) continue;
            long long value = fib_sum(total) - cost(at(ends[k][0], ends[k][1]), mine);
            if (value > best) best = value;
        }
    }
    return best;
}

/* Best opponent harvest - cost among the threat cells (0 when none pays). */
static long long reply(int (*threats)[2], int n, long long gold) {
    long long best = 0;
    for (int i = 0; i < n; ++i) {
        int price = cost(at(threats[i][0], threats[i][1]), theirs);
        if (price == NOT_ALLOWED || price > gold) continue;
        long long earned = harvest(threats[i][0], threats[i][1], theirs);
        if (earned && earned - price > best) best = earned - price;
    }
    return best;
}

static long long windows(int r, int c) {
    long long score = 0;
    for (int d = 0; d < 4; ++d)
        for (int shift = 0; shift < ROW; ++shift) {
            int ours = 0, theirs_now = 0, ours_before = 0, theirs_before = 0;
            bool ok = true;
            for (int k = 0; k < ROW; ++k) {
                int cr = r + (k - shift) * DIRECTIONS[d][0], cc = c + (k - shift) * DIRECTIONS[d][1];
                if (!inside(cr, cc)) { ok = false; break; }
                ours += at(cr, cc) == mine;
                theirs_now += at(cr, cc) == theirs;
                ours_before += !(cr == r && cc == c) && grid[cr][cc] == mine;
                theirs_before += grid[cr][cc] == theirs;
            }
            if (!ok) continue;
            if (theirs_now == 0) score += ATTACK[ours];
            if (ours_before == 0) score += DEFENCE[theirs_before];
        }
    return score;
}

static int find_threats(long long gold, int (*out)[2]) {
    int n = 0;
    placed_r = placed_c = -1;
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c) {
            int price = cost(grid[r][c], theirs);
            if (price != NOT_ALLOWED && price <= gold && harvest(r, c, theirs)) { out[n][0] = r; out[n][1] = c; ++n; }
        }
    return n;
}

static void choose(long long gold, long long opponent_gold, int *row, int *col) {
    static int threats[MAX_CELLS][2];
    int n = find_threats(opponent_gold, threats);
    long long reserve = gold < RESERVE_TURNS * cost_grass ? gold : RESERVE_TURNS * cost_grass;
    bool have_best = false, have_cheap = false;
    long long best_score = 0, cheap_cost = 0, cheap_score = 0;
    int best_r = 0, best_c = 0, cheap_r = 0, cheap_c = 0;
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c) {
            int price = cost(grid[r][c], mine);
            if (price == NOT_ALLOWED || price > gold) continue;
            placed_r = placed_c = -1;
            long long earned = harvest(r, c, mine);
            placed_r = r;
            placed_c = c;
            long long score = NET_WEIGHT * (earned - price) - THREAT_WEIGHT * reply(threats, n, opponent_gold) + windows(r, c) +
                              (earned ? 0 : GAP_WEIGHT * gap_potential(r, c));
            if ((price == 0 || gold - price + earned >= reserve) && (!have_best || score > best_score)) {
                best_r = r; best_c = c; best_score = score; have_best = true;
            }
            if (!have_cheap || price < cheap_cost || (price == cheap_cost && score > cheap_score)) {
                cheap_r = r; cheap_c = c; cheap_cost = price; cheap_score = score; have_cheap = true;
            }
        }
    *row = have_best ? best_r : cheap_r;
    *col = have_best ? best_c : cheap_c;
}

int main(void) {
    char my_name[NAME_SIZE], their_name[NAME_SIZE], line[MAX_SIDE * 4];
    if (scanf("%d %d %d %d %d %d %31s %31s", &width, &height, &cost_soil, &cost_grass, &cost_rocks, &cost_flower, my_name,
              their_name) != 8)
        return 0;
    if (width > MAX_SIDE) width = MAX_SIDE;
    if (height > MAX_SIDE) height = MAX_SIDE;
    mine = (char)toupper((unsigned char)my_name[0]);
    theirs = (char)toupper((unsigned char)their_name[0]);
    int turns_left;
    long long gold, opponent_gold;
    while (scanf("%d %lld %lld", &turns_left, &gold, &opponent_gold) == 3) {
        for (int r = 0; r < height; ++r) {
            if (scanf("%63s", line) != 1) return 0;
            size_t len = strlen(line);
            for (int c = 0; c < width; ++c) grid[r][c] = (size_t)c < len ? line[c] : SOIL;
            grid[r][width] = '\0';
        }
        int row, col;
        choose(gold, opponent_gold, &row, &col);
        printf("%d %d\n", row, col);
        fflush(stdout);
    }
    return 0;
}
