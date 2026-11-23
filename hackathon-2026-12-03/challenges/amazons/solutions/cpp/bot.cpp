// Amazons — C++ (submitted), v1 one-ply territory search. Port of solutions/python/bot.py.
// Eval = queen-distance territory + MOBILITY_WEIGHT * mobility difference.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int TOP_MOVES = 10;  // moves kept for the arrow stage (env CG_NODES overrides)
constexpr int MOBILITY_WEIGHT = 1;
constexpr int DX[8] = {-1, 0, 1, -1, 1, -1, 0, 1}, DY[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
constexpr char EMPTY = '.', WALL = '-';

int n = 8;
vector<vector<int>> rays[100];  // rays[i][d] = cells along direction d (excluding i)

void initBoard(int size) {
    n = size;
    for (int i = 0; i < n * n; ++i) {
        rays[i].assign(8, {});
        int x = i % n, y = i / n;
        for (int d = 0; d < 8; ++d)
            for (int cx = x + DX[d], cy = y + DY[d]; cx >= 0 && cx < n && cy >= 0 && cy < n; cx += DX[d], cy += DY[d])
                rays[i][d].push_back(cy * n + cx);
    }
}

int reach(const string &cells, int start, int *out) {
    int k = 0;
    for (const auto &ray : rays[start])
        for (int t : ray) {
            if (cells[t] != EMPTY) break;
            out[k++] = t;
        }
    return k;
}

void queenDistance(const string &cells, const vector<int> &sources, int *dist) {
    for (int i = 0; i < n * n; ++i) dist[i] = -1;
    vector<int> frontier = sources, next;
    for (int s : sources) dist[s] = 0;
    int d = 0, buf[100];
    while (!frontier.empty()) {
        ++d;
        next.clear();
        for (int c : frontier) {
            int k = reach(cells, c, buf);
            for (int j = 0; j < k; ++j)
                if (dist[buf[j]] < 0) { dist[buf[j]] = d; next.push_back(buf[j]); }
        }
        frontier.swap(next);
    }
}

vector<int> pieces(const string &cells, char color) {
    vector<int> out;
    for (int i = 0; i < n * n; ++i) if (cells[i] == color) out.push_back(i);
    return out;
}

int evaluate(const string &cells, char me, char op) {
    vector<int> mine = pieces(cells, me), theirs = pieces(cells, op);
    int dm[100], dt[100], buf[100];
    queenDistance(cells, mine, dm);
    queenDistance(cells, theirs, dt);
    int territory = 0;
    for (int i = 0; i < n * n; ++i) {
        if (cells[i] != EMPTY) continue;
        int a = dm[i], b = dt[i];
        if (a >= 0 && (b < 0 || a < b)) ++territory;
        else if (b >= 0 && (a < 0 || b < a)) --territory;
    }
    int mobility = 0;
    for (int p : mine) mobility += reach(cells, p, buf);
    for (int p : theirs) mobility -= reach(cells, p, buf);
    return territory + MOBILITY_WEIGHT * mobility;
}

struct Action { int src, dst, arrow; };

// Two stages: rank every queen move without arrow, keep the `topMoves` best
// (stable insertion sort), then score every arrow of those moves.
bool bestAction(string &cells, char me, char op, int topMoves, Action &best) {
    struct Ranked { int value, src, dst; };
    vector<Ranked> ranked;
    int moves[100], arrows[100];
    for (int src : pieces(cells, me)) {
        int nm = reach(cells, src, moves);
        for (int a = 0; a < nm; ++a) {
            int dst = moves[a];
            cells[src] = EMPTY; cells[dst] = me;
            ranked.push_back({evaluate(cells, me, op), src, dst});
            cells[src] = me; cells[dst] = EMPTY;
        }
    }
    for (size_t i = 1; i < ranked.size(); ++i) {
        Ranked x = ranked[i];
        size_t j = i;
        while (j > 0 && ranked[j - 1].value < x.value) { ranked[j] = ranked[j - 1]; --j; }
        ranked[j] = x;
    }
    bool found = false;
    int bestV = 0;
    for (size_t r = 0; r < ranked.size() && (int)r < topMoves; ++r) {
        int src = ranked[r].src, dst = ranked[r].dst;
        cells[src] = EMPTY; cells[dst] = me;
        int na = reach(cells, dst, arrows);
        for (int b = 0; b < na; ++b) {
            cells[arrows[b]] = WALL;
            int v = evaluate(cells, me, op);
            cells[arrows[b]] = EMPTY;
            if (!found || v > bestV) { found = true; bestV = v; best = {src, dst, arrows[b]}; }
        }
        cells[src] = me; cells[dst] = EMPTY;
    }
    return found;
}

string squareName(int i) { return string(1, char('a' + i % n)) + to_string(n - i / n); }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int topMoves = TOP_MOVES;
    if (const char *e = getenv("CG_NODES")) topMoves = atoi(e);
    string line;
    if (!getline(cin, line)) return 0;
    int size = atoi(line.c_str());
    if (size < 1 || size > 10) size = 8;
    initBoard(size);
    while (getline(cin, line)) {
        auto t0 = chrono::steady_clock::now();
        char me = line.empty() ? 'w' : line[0], op = me == 'w' ? 'b' : 'w';
        string cells;
        for (int y = 0; y < n; ++y) {
            if (!getline(cin, line)) return 0;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line += string(n, WALL);
            cells += line.substr(0, n);
        }
        if (!getline(cin, line) || !getline(cin, line)) return 0;
        Action a;
        if (!bestAction(cells, me, op, topMoves, a)) { cout << "random" << endl; continue; }
        cerr << "ms=" << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << squareName(a.src) << squareName(a.dst) << squareName(a.arrow) << endl;
    }
}
