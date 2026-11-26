/* Volcanoes — C port of cpp/bot.cpp (same simulation, same distance, same tie-break).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_LEVEL = 4, DIST_WEIGHT = 100, BLOCKED = 1000, WALL = -1 };  /* BLOCKED: no chain possible */
enum { MAX_TILES = 128, NAME_SIZE = 16, LINE_SIZE = 4096, MAX_MOVES = 256 };

static char names[MAX_TILES][NAME_SIZE];
static int neighbours[MAX_TILES][3], tiles, pairs[MAX_TILES][2], pair_count;

static int find_tile(const char *name) {
    for (int i = 0; i < tiles; ++i)
        if (!strcmp(names[i], name)) return i;
    return -1;
}

static void play(int *after, int tile) {
    static int queue[MAX_TILES * 4];
    bool erupted[MAX_TILES] = {false};
    int head = 0, tail = 0;
    after[tile] = after[tile] > 0 ? after[tile] + 1 : 1;
    if (after[tile] >= MAX_LEVEL) queue[tail++] = tile;
    while (head < tail) {
        int t = queue[head++];
        if (erupted[t]) continue;
        erupted[t] = true;
        for (int k = 0; k < 3; ++k) {
            int n = neighbours[t][k];
            if (after[n] == 0) after[n] = 1;
            else if (after[n] > 0 && !erupted[n]) {
                after[n] = after[n] + 1 < MAX_LEVEL ? after[n] + 1 : MAX_LEVEL;
                if (after[n] >= MAX_LEVEL && tail < MAX_TILES * 4) queue[tail++] = n;
            } else if (after[n] < 0) after[n] = 0;
        }
    }
}

static int cost(const int *levels, int sign, int tile) {
    int v = levels[tile] * sign;
    return v > 0 ? 0 : v == 0 ? 1 : WALL;
}

/* Fewest empty tiles to fill for a N<k>-S<k> chain of `sign` volcanoes. */
static int distance(const int *levels, int sign) {
    static int dist[MAX_TILES], deque[MAX_TILES * 8];
    int best = BLOCKED;
    for (int p = 0; p < pair_count; ++p) {
        int north = pairs[p][0], south = pairs[p][1], start = cost(levels, sign, north);
        if (start == WALL) continue;
        for (int i = 0; i < tiles; ++i) dist[i] = BLOCKED;
        dist[north] = start;
        int head = MAX_TILES * 4, tail = head; /* double-ended queue in a flat array */
        deque[tail++] = north;
        while (head < tail) {
            int t = deque[head++];
            for (int k = 0; k < 3; ++k) {
                int n = neighbours[t][k], step = cost(levels, sign, n);
                if (step == WALL || dist[t] + step >= dist[n]) continue;
                dist[n] = dist[t] + step;
                if (step == 0 && head > 0) deque[--head] = n;
                else if (tail < MAX_TILES * 8) deque[tail++] = n;
            }
        }
        if (dist[south] < best) best = dist[south];
    }
    return best;
}

int main(void) {
    static char line[LINE_SIZE];
    static char moves[MAX_MOVES][NAME_SIZE];
    if (!fgets(line, sizeof line, stdin)) return 0;
    tiles = atoi(line);
    if (tiles > MAX_TILES) tiles = MAX_TILES;
    for (int i = 0; i < tiles; ++i) {
        if (!fgets(line, sizeof line, stdin)) return 0;
        if (sscanf(line, "%15s %d %d %d", names[i], &neighbours[i][0], &neighbours[i][1], &neighbours[i][2]) != 4) return 0;
    }
    for (int i = 0; i < tiles; ++i) {
        char south[NAME_SIZE + 1];
        if (names[i][0] != 'N') continue;
        snprintf(south, sizeof south, "S%s", names[i] + 1);
        int s = find_tile(south);
        if (s >= 0) { pairs[pair_count][0] = i; pairs[pair_count][1] = s; ++pair_count; }
    }
    while (fgets(line, sizeof line, stdin)) {
        int levels[MAX_TILES] = {0}, n = 0, offset = 0, used;
        while (n < tiles && sscanf(line + offset, "%d%n", &levels[n], &used) == 1) { offset += used; ++n; }
        if (!n && !strpbrk(line, "0123456789")) return 0;
        if (!fgets(line, sizeof line, stdin)) line[0] = '\0';
        int count = 0;
        for (char *tok = strtok(line, " \r\n"); tok && count < MAX_MOVES; tok = strtok(NULL, " \r\n"))
            snprintf(moves[count++], NAME_SIZE, "%s", tok);
        int best = -1, best_score = 0;
        for (int i = 0; i < count; ++i) {
            int tile = find_tile(moves[i]);
            if (tile < 0) continue;
            int after[MAX_TILES];
            memcpy(after, levels, sizeof after);
            play(after, tile);
            int total = 0;
            for (int t = 0; t < tiles; ++t)
                if (after[t] > 0) total += after[t];
            int score = DIST_WEIGHT * (distance(after, -1) - distance(after, 1)) + total;
            if (best < 0 || score > best_score) { best = i; best_score = score; }
        }
        printf("%s\n", best >= 0 ? moves[best] : (count ? moves[0] : "RANDOM"));
        fflush(stdout);
    }
    return 0;
}
