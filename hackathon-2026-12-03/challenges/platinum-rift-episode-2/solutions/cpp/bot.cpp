// Platinum Rift 2 — C++ (submitted), v1 spread and march. Port of solutions/python/bot.py.
// HQs = zones owned on the first turn. Zone by zone (increasing id): fighting
// pods stay; our HQ keeps the adjacent enemy count + GUARD_MARGIN; one pod
// (or enough to outnumber visible enemies) to each neighbour we do not own,
// best first (remembered platinum, closer to the enemy HQ, lower id); the rest
// marches one BFS step towards the enemy HQ.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int GUARD_MARGIN = 1;  // spare defenders when enemies stand next to our HQ
constexpr int UNREACHABLE = 1 << 20;
constexpr int NONE = -1;

struct Zone { int id, owner, pods[2], visible, platinum; };
struct Move { int count, from, to; };

vector<int> bfs(const vector<vector<int>> &adj, int start) {
    vector<int> dist(adj.size(), UNREACHABLE);
    if (start < 0) return dist;
    dist[start] = 0;
    deque<int> queue{start};
    while (!queue.empty()) {
        int z = queue.front();
        queue.pop_front();
        for (int n : adj[z])
            if (dist[n] == UNREACHABLE) { dist[n] = dist[z] + 1; queue.push_back(n); }
    }
    return dist;
}

struct Game {
    int me;
    vector<vector<int>> adj;
    vector<int> platinum;  // last seen platinum per zone
    int myHq = NONE, enemyHq = NONE;
    vector<int> dist;

    Game(int me_, vector<vector<int>> adj_) : me(me_), adj(move(adj_)), platinum(adj.size(), 0), dist(adj.size(), UNREACHABLE) {}

    void observe(const vector<Zone> &zones) {
        for (const Zone &z : zones)
            if (z.visible) platinum[z.id] = z.platinum;
        if (myHq != NONE) return;
        for (const Zone &z : zones) {
            if (z.owner == me && myHq == NONE) myHq = z.id;
            else if (z.owner == 1 - me && enemyHq == NONE) enemyHq = z.id;
        }
        dist = bfs(adj, enemyHq);
    }

    // Pods kept home: the enemies next to our HQ plus a margin.
    int guards(int z, const vector<int> &theirs) const {
        if (z != myHq) return 0;
        int threat = 0;
        for (int n : adj[z]) threat += theirs[n];
        return threat ? threat + GUARD_MARGIN : 0;
    }

    int spread(int z, int available, const vector<int> &owner, const vector<int> &theirs, vector<Move> &moves) const {
        vector<int> targets;
        for (int n : adj[z])
            if (owner[n] != me) targets.push_back(n);
        stable_sort(targets.begin(), targets.end(), [&](int a, int b) {
            if (platinum[a] != platinum[b]) return platinum[a] > platinum[b];
            if (dist[a] != dist[b]) return dist[a] < dist[b];
            return a < b;
        });
        for (int n : targets) {
            if (available <= 0) break;
            int send = min(available, theirs[n] + 1);
            moves.push_back({send, z, n});
            available -= send;
        }
        return available;
    }

    void march(int z, int available, vector<Move> &moves) const {
        if (available <= 0) return;
        int best = NONE;
        for (int n : adj[z])
            if (dist[n] < dist[z] && (best == NONE || dist[n] < dist[best])) best = n;
        if (best != NONE) moves.push_back({available, z, best});
    }

    vector<Move> orders(const vector<Zone> &zones) const {
        vector<int> owner, mine, theirs;
        for (const Zone &z : zones) {
            owner.push_back(z.owner);
            mine.push_back(z.pods[me]);
            theirs.push_back(z.pods[1 - me]);
        }
        vector<Move> moves;
        for (int z = 0; z < (int)zones.size(); ++z) {
            if (mine[z] == 0 || theirs[z] > 0) continue;
            int available = mine[z] - guards(z, theirs);
            available = spread(z, available, owner, theirs, moves);
            march(z, available, moves);
        }
        return moves;
    }
};

int main() {
    ios::sync_with_stdio(false);
    int playerCount, me, zoneCount, linkCount;
    if (!(cin >> playerCount >> me >> zoneCount >> linkCount)) return 0;
    for (int i = 0, ignored; i < 2 * zoneCount; ++i) cin >> ignored;
    vector<vector<int>> adj(zoneCount);
    for (int i = 0; i < linkCount; ++i) {
        int a, b;
        cin >> a >> b;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }
    for (auto &n : adj) {
        sort(n.begin(), n.end());
        n.erase(unique(n.begin(), n.end()), n.end());
    }
    Game game(me, adj);
    int stock;
    while (cin >> stock) {  // pods are bought automatically: the stock is unused
        vector<Zone> zones(zoneCount);
        for (Zone &z : zones) cin >> z.id >> z.owner >> z.pods[0] >> z.pods[1] >> z.visible >> z.platinum;
        if (!cin) return 0;
        sort(zones.begin(), zones.end(), [](const Zone &a, const Zone &b) { return a.id < b.id; });
        game.observe(zones);
        vector<Move> moves = game.orders(zones);
        string line;
        for (const Move &m : moves) {
            if (!line.empty()) line += ' ';
            line += to_string(m.count) + ' ' + to_string(m.from) + ' ' + to_string(m.to);
        }
        cout << (line.empty() ? "WAIT" : line) << "\nWAIT" << endl;
    }
}
