// Counting TicTacToe 10x10 — C++ (submitted), v1. Port of solutions/python/bot.py.
// Score = sliding windows of 3 same-colour cells (referee RezaSi/counting-tic-tac-toe).
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

constexpr int N = 10, CELLS = N * N;
constexpr long long FIRST_TURN_NODES = 2500, TURN_NODES = 2500;
constexpr int TURN_MS = 70;
constexpr int WEIGHTS[4] = {0, 1, 10, 100};  // window value by own stones when unopposed
constexpr int MINE = 1, THEIRS = 2;
constexpr int BEAM = 10;  // inner nodes only search the BEAM best moves by immediate gain

struct Window { int a, b, c; };
vector<Window> windows;
vector<int> windowsOf[CELLS];

// cell: 0 empty, 1/2 sides; score = pattern score for player 1 (incremental)
struct State { array<int, CELLS> cell; int me; int score; };

inline int windowValue(const array<int, CELLS> &c, const Window &w) {
    int ones = (c[w.a] == 1) + (c[w.b] == 1) + (c[w.c] == 1);
    int twos = (c[w.a] == 2) + (c[w.b] == 2) + (c[w.c] == 2);
    if (!twos) return WEIGHTS[ones];
    if (!ones) return -WEIGHTS[twos];
    return 0;
}

int fullScore(const array<int, CELLS> &c) {
    int sc = 0;
    for (const Window &w : windows) sc += windowValue(c, w);
    return sc;
}

void buildWindows() {
    const int DR[4] = {1, 0, 1, 1}, DC[4] = {0, 1, 1, -1};
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            for (int d = 0; d < 4; ++d) {
                int r2 = r + 2 * DR[d], c2 = c + 2 * DC[d];
                if (r2 >= 0 && r2 < N && c2 >= 0 && c2 < N)
                    windows.push_back({r * N + c, (r + DR[d]) * N + c + DC[d], r2 * N + c2});
            }
    for (int k = 0; k < (int)windows.size(); ++k) {
        windowsOf[windows[k].a].push_back(k);
        windowsOf[windows[k].b].push_back(k);
        windowsOf[windows[k].c].push_back(k);
    }
}

int genMoves(const State &s, int *out) {
    int n = 0;
    for (int i = 0; i < CELLS; ++i) if (!s.cell[i]) out[n++] = i;
    return n;
}

// Only the windows through cell i change: update the score by delta.
inline State applyMove(const State &s, int i) {
    State t = s;
    int before = 0, after = 0;
    for (int k : windowsOf[i]) before += windowValue(t.cell, windows[k]);
    t.cell[i] = s.me;
    for (int k : windowsOf[i]) after += windowValue(t.cell, windows[k]);
    t.score = s.score + after - before;
    t.me = 3 - s.me;
    return t;
}

inline int evaluate(const State &s) { return s.me == 1 ? s.score : -s.score; }

// Moves sorted by the mover's immediate gain (stable insertion sort), at most `limit`.
int orderedMoves(const State &s, int *out, int limit) {
    int mv[CELLS], gain[CELLS];
    int n = genMoves(s, mv);
    for (int i = 0; i < n; ++i) gain[i] = -evaluate(applyMove(s, mv[i]));
    for (int i = 1; i < n; ++i) {
        int x = mv[i], g = gain[i], j = i - 1;
        while (j >= 0 && gain[j] < g) { mv[j + 1] = mv[j]; gain[j + 1] = gain[j]; --j; }
        mv[j + 1] = x; gain[j + 1] = g;
    }
    if (n > limit) n = limit;
    for (int i = 0; i < n; ++i) out[i] = mv[i];
    return n;
}

int finalTriples(const State &s) {
    int diff = 0;
    for (const Window &w : windows) {
        int a = s.cell[w.a];
        if (a && a == s.cell[w.b] && a == s.cell[w.c]) diff += a == s.me ? 1 : -1;
    }
    return diff;
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = TURN_NODES;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TURN_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int mv[CELLS];
    int n = genMoves(s, mv);
    if (!n) {
        int diff = finalTriples(s);
        return diff > 0 ? WIN - ply : diff < 0 ? -(WIN - ply) : 0;
    }
    if (depth == 0) return evaluate(s);
    n = orderedMoves(s, mv, BEAM);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        int v = -negamax(applyMove(s, mv[i]), depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

int search(const State &root) {
    int mv[CELLS];
    int n = orderedMoves(root, mv, CELLS), best = mv[0];
    lastDepth = 0; nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[curIdx];
        lastDepth = depth;
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];  // PV first
        mv[0] = best;
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    buildWindows();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    array<bool, CELLS> myCells{};
    bool first = true;
    int orow, ocol;
    while (cin >> orow >> ocol) {
        t0 = chrono::steady_clock::now();
        int k;
        if (!(cin >> k)) return 0;
        array<bool, CELLS> empty{};
        for (int i = 0; i < k; ++i) {
            int r, c;
            if (!(cin >> r >> c)) return 0;
            if (r >= 0 && r < N && c >= 0 && c < N) empty[r * N + c] = true;
        }
        bool newMatch = false;
        for (int i = 0; i < CELLS; ++i) if (myCells[i] && empty[i]) { newMatch = true; break; }
        if (newMatch) myCells.fill(false);  // the board was cleared
        State s;
        s.me = MINE;
        bool anyEmpty = false;
        for (int i = 0; i < CELLS; ++i) {
            s.cell[i] = empty[i] ? 0 : (myCells[i] ? MINE : THEIRS);
            anyEmpty |= empty[i];
        }
        if (!anyEmpty) { cout << "0 0" << endl; continue; }
        s.score = fullScore(s.cell);
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        first = false;
        int m = search(s);
        myCells[m] = true;
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << m / N << ' ' << m % N << endl;
    }
}
