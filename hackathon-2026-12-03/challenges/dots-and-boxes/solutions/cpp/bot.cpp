// Dots and Boxes — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Edges: horizontal h(x, line) = line * n + x, vertical H + y * (n + 1) + line.
// Closing a box scores it and the same player moves again (no sign flip).
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <array>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr long long FIRST_TURN_NODES = 200000, TURN_NODES = 20000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 70;
constexpr int CAPTURABLE_WEIGHT = 1;  // a box with one free side is ours to take
const string SIDE_LETTERS = "LTRB";
constexpr int MAX_EDGES = 2 * 10 * 11;

int n = 2, edgeCount = 0;
vector<array<int, 4>> boxEdges;          // box -> edges in L, T, R, B order
vector<vector<int>> edgeBoxes;

struct State { bool drawn[MAX_EDGES]; int diff; };  // diff = side to move - other

void initBoard(int size) {
    n = size;
    int hCount = n * (n + 1);
    edgeCount = 2 * n * (n + 1);
    boxEdges.clear();
    edgeBoxes.assign(edgeCount, {});
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x) {
            int left = hCount + y * (n + 1) + x, top = (y + 1) * n + x;
            boxEdges.push_back({left, top, left + 1, y * n + x});
        }
    for (int b = 0; b < (int)boxEdges.size(); ++b)
        for (int e : boxEdges[b]) edgeBoxes[e].push_back(b);
}

inline int freeSides(const bool *drawn, int b) {
    int f = 0;
    for (int e : boxEdges[b]) f += !drawn[e];
    return f;
}

int genMoves(const State &s, int *out) {
    int k = 0;
    for (int e = 0; e < edgeCount; ++e) if (!s.drawn[e]) out[k++] = e;
    return k;
}

// Returns true when the same player moves again (a box was closed).
bool applyMove(const State &s, int e, State &child) {
    child = s;
    child.drawn[e] = true;
    int closed = 0;
    for (int b : edgeBoxes[e]) closed += freeSides(child.drawn, b) == 0;
    if (closed) { child.diff = s.diff + closed; return true; }
    child.diff = -s.diff;
    return false;
}

int evaluate(const State &s) {
    int capturable = 0;
    for (int b = 0; b < n * n; ++b) capturable += freeSides(s.drawn, b) == 1;
    return s.diff + CAPTURABLE_WEIGHT * capturable;
}

// 2 = closes a box, 1 = safe (gives no third side), 0 = gift.
int orderKey(const State &s, int e) {
    bool closes = false, gift = false;
    for (int b : edgeBoxes[e]) {
        int f = freeSides(s.drawn, b);
        if (f == 1) closes = true;
        else if (f == 2) gift = true;
    }
    return closes ? 2 : (gift ? 0 : 1);
}

void orderMoves(const State &s, int *mv, int k) {  // stable insertion sort
    int key[MAX_EDGES];
    for (int i = 0; i < k; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < k; ++i) {
        int x = mv[i], kk = key[i], j = i - 1;
        while (j >= 0 && key[j] < kk) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = kk;
    }
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = TURN_NODES;
int TIME_MS = TURN_MS;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int value(const State &s, int depth, int alpha, int beta) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int mv[MAX_EDGES];
    int k = genMoves(s, mv);
    if (!k) return s.diff > 0 ? WIN + s.diff : (s.diff < 0 ? -WIN + s.diff : 0);
    if (depth == 0) return evaluate(s);
    orderMoves(s, mv, k);
    int best = -INF;
    State child;
    for (int i = 0; i < k; ++i) {
        bool again = applyMove(s, mv[i], child);
        int v = again ? value(child, depth - 1, alpha, beta) : -value(child, depth - 1, -beta, -alpha);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

int search(const State &root) {
    int mv[MAX_EDGES];
    int k = genMoves(root, mv);
    orderMoves(root, mv, k);
    int best = mv[0];
    lastDepth = 0; nodes = 0; aborted = false;
    State child;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        for (int i = 0; i < k; ++i) {
            bool again = applyMove(root, mv[i], child);
            int v = again ? value(child, depth - 1, alpha, INF) : -value(child, depth - 1, -INF, -alpha);
            if (aborted) break;
            if (v > bestV) { bestV = v; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[curIdx];
        lastDepth = depth;
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];  // PV first
        mv[0] = best;
        if (depth >= k + 1) break;
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int size = atoi(line.c_str());
    if (size < 1 || size > 10) size = 2;
    initBoard(size);
    if (!getline(cin, line)) return 0;  // our id: the state is given in our point of view
    bool first = true;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        int mine = 0, theirs = 0;
        { istringstream in(line); in >> mine >> theirs; }
        if (!getline(cin, line)) return 0;
        int k = atoi(line.c_str());
        State s;
        for (int e = 0; e < edgeCount; ++e) s.drawn[e] = true;
        map<int, string> ownerBox;  // edge -> first listed "box side" that can draw it
        for (int i = 0; i < k; ++i) {
            if (!getline(cin, line)) return 0;
            istringstream in(line);
            string name, sides;
            in >> name >> sides;
            if (name.size() < 2) continue;
            int x = name[0] - 'A', y = atoi(name.c_str() + 1) - 1;
            if (x < 0 || x >= n || y < 0 || y >= n) continue;
            for (char ch : sides) {
                size_t idx = SIDE_LETTERS.find(ch);
                if (idx == string::npos) continue;
                int e = boxEdges[y * n + x][idx];
                s.drawn[e] = false;
                if (!ownerBox.count(e)) ownerBox[e] = name + " " + ch;
            }
        }
        s.diff = mine - theirs;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = first ? FIRST_TURN_MS : TURN_MS;
        first = false;
        if (ownerBox.empty()) { cout << "A1 L" << endl; continue; }
        int e = search(s);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << ownerBox[e] << endl;
    }
}
