// Volcanoes — C++ (submitted), v1 connection distance. Port of solutions/python/bot.py.
// Each valid move is played (level 1 on empty / +1 on ours, eruptions at level 4
// cascade once per tile: empty -> ours, ours +1, enemy destroyed) and scored by
// DIST_WEIGHT * (their chain distance - ours) + our total level. Distance =
// min over k of the 0-1 BFS from N<k> to S<k> (ours 0, empty 1, enemy blocked).
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

constexpr int MAX_LEVEL = 4;
constexpr int DIST_WEIGHT = 100;
constexpr int BLOCKED = 1000;  // no chain possible
constexpr int WALL = -1;

struct Board {
    vector<string> names;
    map<string, int> index;
    vector<vector<int>> neighbours;
    vector<pair<int, int>> pairs;

    void build() {
        for (int i = 0; i < (int)names.size(); ++i) index[names[i]] = i;
        for (int i = 0; i < (int)names.size(); ++i)
            if (names[i][0] == 'N' && index.count("S" + names[i].substr(1))) pairs.push_back({i, index["S" + names[i].substr(1)]});
    }

    vector<int> play(vector<int> after, int tile) const {
        after[tile] = after[tile] > 0 ? after[tile] + 1 : 1;
        set<int> erupted;
        deque<int> queue;
        if (after[tile] >= MAX_LEVEL) queue.push_back(tile);
        while (!queue.empty()) {
            int t = queue.front();
            queue.pop_front();
            if (!erupted.insert(t).second) continue;
            for (int n : neighbours[t]) {
                if (after[n] == 0) after[n] = 1;
                else if (after[n] > 0 && !erupted.count(n)) {
                    after[n] = min(after[n] + 1, MAX_LEVEL);
                    if (after[n] >= MAX_LEVEL) queue.push_back(n);
                } else if (after[n] < 0) after[n] = 0;
            }
        }
        return after;
    }

    static int cost(const vector<int> &levels, int sign, int tile) {
        int v = levels[tile] * sign;
        return v > 0 ? 0 : v == 0 ? 1 : WALL;
    }

    // Fewest empty tiles to fill for a N<k>-S<k> chain of `sign` volcanoes.
    int distance(const vector<int> &levels, int sign) const {
        int best = BLOCKED;
        vector<int> dist(names.size());
        for (const auto &[north, south] : pairs) {
            int start = cost(levels, sign, north);
            if (start == WALL) continue;
            fill(dist.begin(), dist.end(), BLOCKED);
            dist[north] = start;
            deque<int> queue{north};
            while (!queue.empty()) {
                int t = queue.front();
                queue.pop_front();
                for (int n : neighbours[t]) {
                    int step = cost(levels, sign, n);
                    if (step == WALL || dist[t] + step >= dist[n]) continue;
                    dist[n] = dist[t] + step;
                    if (step == 0) queue.push_front(n);
                    else queue.push_back(n);
                }
            }
            best = min(best, dist[south]);
        }
        return best;
    }

    string choose(const vector<int> &levels, const vector<string> &moves) const {
        int best = -1, bestScore = 0;
        for (int i = 0; i < (int)moves.size(); ++i) {
            auto it = index.find(moves[i]);
            if (it == index.end()) continue;
            vector<int> after = play(levels, it->second);
            int total = 0;
            for (int v : after)
                if (v > 0) total += v;
            int score = DIST_WEIGHT * (distance(after, -1) - distance(after, 1)) + total;
            if (best < 0 || score > bestScore) { best = i; bestScore = score; }
        }
        if (best >= 0) return moves[best];
        return moves.empty() ? "RANDOM" : moves[0];
    }
};

int main() {
    ios::sync_with_stdio(false);
    Board board;
    string line;
    if (!getline(cin, line) || line.find_first_not_of(" \r\n") == string::npos) return 0;
    int count = stoi(line);
    for (int i = 0; i < count; ++i) {
        getline(cin, line);
        istringstream ss(line);
        string name;
        vector<int> n(3);
        ss >> name >> n[0] >> n[1] >> n[2];
        board.names.push_back(name);
        board.neighbours.push_back(n);
    }
    board.build();
    while (getline(cin, line)) {
        if (line.find_first_not_of(" \r\n") == string::npos) return 0;
        istringstream ss(line);
        vector<int> levels;
        int v;
        while (ss >> v && levels.size() < board.names.size()) levels.push_back(v);
        levels.resize(board.names.size(), 0);
        getline(cin, line);
        istringstream ms(line);
        vector<string> moves;
        string m;
        while (ms >> m) moves.push_back(m);
        cout << board.choose(levels, moves) << endl;
    }
}
