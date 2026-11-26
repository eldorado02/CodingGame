/* Platinum Rift 2 — C port of cpp/bot.cpp (same orders, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { GUARD_MARGIN = 1 };  /* spare defenders when enemies stand next to our HQ */
enum { UNREACHABLE = 1 << 20, NONE = -1 };
enum { MAX_ZONES = 512, MAX_DEGREE = 16, MAX_MOVES = MAX_ZONES * MAX_DEGREE };

typedef struct { int id, owner, pods[2], visible, platinum; } Zone;
typedef struct { int count, from, to; } Move;

static int zone_count, me;
static int adj[MAX_ZONES][MAX_DEGREE], degree[MAX_ZONES];
static int platinum[MAX_ZONES];  /* last seen platinum per zone */
static int dist[MAX_ZONES];
static int my_hq = NONE, enemy_hq = NONE;

static void add_link(int a, int b) {
    for (int i = 0; i < degree[a]; ++i)
        if (adj[a][i] == b) return;
    if (degree[a] < MAX_DEGREE) adj[a][degree[a]++] = b;
}

static int compare_ints(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static void bfs(int start) {
    static int queue[MAX_ZONES];
    int head = 0, tail = 0;
    for (int z = 0; z < zone_count; ++z) dist[z] = UNREACHABLE;
    if (start < 0) return;
    dist[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int z = queue[head++];
        for (int i = 0; i < degree[z]; ++i) {
            int n = adj[z][i];
            if (dist[n] == UNREACHABLE) { dist[n] = dist[z] + 1; queue[tail++] = n; }
        }
    }
}

static void observe(const Zone *zones) {
    for (int z = 0; z < zone_count; ++z)
        if (zones[z].visible) platinum[z] = zones[z].platinum;
    if (my_hq != NONE) return;
    for (int z = 0; z < zone_count; ++z) {
        if (zones[z].owner == me && my_hq == NONE) my_hq = z;
        else if (zones[z].owner == 1 - me && enemy_hq == NONE) enemy_hq = z;
    }
    bfs(enemy_hq);
}

/* Pods kept home: the enemies next to our HQ plus a margin. */
static int guards(int z, const int *theirs) {
    if (z != my_hq) return 0;
    int threat = 0;
    for (int i = 0; i < degree[z]; ++i) threat += theirs[adj[z][i]];
    return threat ? threat + GUARD_MARGIN : 0;
}

/* Spread order: more platinum, closer to the enemy HQ, lower id. */
static int before(int a, int b) {
    if (platinum[a] != platinum[b]) return platinum[a] > platinum[b];
    if (dist[a] != dist[b]) return dist[a] < dist[b];
    return a < b;
}

static int spread(int z, int available, const int *owner, const int *theirs, Move *moves, int *n_moves) {
    int targets[MAX_DEGREE], n = 0;
    for (int i = 0; i < degree[z]; ++i)
        if (owner[adj[z][i]] != me) targets[n++] = adj[z][i];
    for (int i = 1; i < n; ++i) { /* stable insertion sort */
        int x = targets[i], j = i - 1;
        while (j >= 0 && before(x, targets[j])) { targets[j + 1] = targets[j]; --j; }
        targets[j + 1] = x;
    }
    for (int i = 0; i < n && available > 0; ++i) {
        int send = theirs[targets[i]] + 1 < available ? theirs[targets[i]] + 1 : available;
        moves[(*n_moves)++] = (Move){send, z, targets[i]};
        available -= send;
    }
    return available;
}

static void march(int z, int available, Move *moves, int *n_moves) {
    if (available <= 0) return;
    int best = NONE;
    for (int i = 0; i < degree[z]; ++i) {
        int n = adj[z][i];
        if (dist[n] < dist[z] && (best == NONE || dist[n] < dist[best])) best = n;
    }
    if (best != NONE) moves[(*n_moves)++] = (Move){available, z, best};
}

static int orders(const Zone *zones, Move *moves) {
    static int owner[MAX_ZONES], mine[MAX_ZONES], theirs[MAX_ZONES];
    int n_moves = 0;
    for (int z = 0; z < zone_count; ++z) {
        owner[z] = zones[z].owner;
        mine[z] = zones[z].pods[me];
        theirs[z] = zones[z].pods[1 - me];
    }
    for (int z = 0; z < zone_count; ++z) {
        if (mine[z] == 0 || theirs[z] > 0) continue;
        int available = mine[z] - guards(z, theirs);
        available = spread(z, available, owner, theirs, moves, &n_moves);
        march(z, available, moves, &n_moves);
    }
    return n_moves;
}

int main(void) {
    int player_count, link_count, ignored, stock;
    if (scanf("%d %d %d %d", &player_count, &me, &zone_count, &link_count) != 4) return 0;
    if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;
    for (int i = 0; i < 2 * zone_count; ++i)
        if (scanf("%d", &ignored) != 1) return 0;
    for (int i = 0; i < link_count; ++i) {
        int a, b;
        if (scanf("%d %d", &a, &b) != 2) return 0;
        add_link(a, b);
        add_link(b, a);
    }
    for (int z = 0; z < zone_count; ++z) qsort(adj[z], degree[z], sizeof(int), compare_ints);
    static Zone zones[MAX_ZONES];
    static Move moves[MAX_MOVES];
    while (scanf("%d", &stock) == 1) { /* pods are bought automatically: the stock is unused */
        for (int i = 0; i < zone_count; ++i) {
            Zone z;
            if (scanf("%d %d %d %d %d %d", &z.id, &z.owner, &z.pods[0], &z.pods[1], &z.visible, &z.platinum) != 6) return 0;
            if (z.id >= 0 && z.id < zone_count) zones[z.id] = z;
        }
        observe(zones);
        int n = orders(zones, moves);
        if (!n) printf("WAIT");
        for (int i = 0; i < n; ++i) printf(i ? " %d %d %d" : "%d %d %d", moves[i].count, moves[i].from, moves[i].to);
        printf("\nWAIT\n");
        fflush(stdout);
    }
    return 0;
}
