// TEMPLATE board-2p — two-player, perfect-information, turn-based game.
// Engine: negamax + alpha-beta + iterative deepening, deterministic node budget.
//
// Replace every block marked "GAME:" (state, move generation, apply, eval,
// I/O). Keep the ENGINE part identical across the 4 languages so the fuzz
// stays byte-identical. The toy game shipped here is 3x3 tic-tac-toe in the
// CodinGame `tic-tac-toe` (Wood) protocol, so the template is a working bot.
//
// Determinism rules (see bot-programming/ALGO.md §0):
//   * stop on a NODE budget (identical in all languages), never on time alone;
//   * the time guard only aborts in emergency and logs "TIMEGUARD" on stderr;
//   * move order = generation order, then stable sort on an integer key;
//   * ties broken by first-found (strict > comparison).
// Compile: g++ -O2 -std=c++20 -Wall -Wextra bot.cpp -o bot
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------- GAME: state
constexpr int MAX_MOVES = 9;        // GAME: upper bound of legal moves
struct Move { int r = -1, c = -1; };
struct State {
    array<int, 9> cell{};           // 0 empty, 1 side to move at root, 2 other
    int toMove = 1;                 // 1 or 2
};

// GAME: winner of the position: 0 none, 1/2 player, 3 draw (board full)
int winner(const State &s) {
    static const int L[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (auto &l : L) {
        int a = s.cell[l[0]];
        if (a && a == s.cell[l[1]] && a == s.cell[l[2]]) return a;
    }
    for (int v : s.cell) if (!v) return 0;
    return 3;
}

// GAME: legal moves in a fixed, deterministic order
int genMoves(const State &s, Move *out) {
    int n = 0;
    for (int i = 0; i < 9; ++i) if (!s.cell[i]) out[n++] = {i / 3, i % 3};
    return n;
}

// GAME: copy-make apply
State applyMove(const State &s, const Move &m) {
    State t = s;
    t.cell[m.r * 3 + m.c] = s.toMove;
    t.toMove = 3 - s.toMove;
    return t;
}

// GAME: static evaluation from the point of view of s.toMove (higher = better)
int evaluate(const State &s) {
    static const int W[9] = {3,2,3,2,4,2,3,2,3};
    int sc = 0;
    for (int i = 0; i < 9; ++i) {
        if (s.cell[i] == s.toMove) sc += W[i];
        else if (s.cell[i]) sc -= W[i];
    }
    return sc;
}

// GAME: ordering key, higher searched first (stable sort keeps gen order on ties)
int orderKey(const State &, const Move &m) { return (m.r == 1 && m.c == 1) ? 1 : 0; }

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 200000;      // overridable with env CG_NODES (fuzz speed)
constexpr int TIME_MS = 80;         // emergency guard, keep >= 2x margin under CG limit

long long nodes = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int w = winner(s);
    if (w == 3) return 0;
    if (w) return (w == s.toMove) ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) return evaluate(s);  // GAME: pass / stalemate rule
    stable_sort(mv, mv + n, [&](const Move &a, const Move &b) { return orderKey(s, a) > orderKey(s, b); });
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

Move search(const State &root) {
    Move mv[MAX_MOVES];
    int n = genMoves(root, mv);
    stable_sort(mv, mv + n, [&](const Move &a, const Move &b) { return orderKey(root, a) > orderKey(root, b); });
    Move best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF;
        Move cur = mv[0];
        int curIdx = 0;
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = mv[i]; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;           // keep the last COMPLETED iteration
        best = cur;
        rotate(mv, mv + curIdx, mv + curIdx + 1);  // PV move first next iteration
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;  // proven
        if (depth >= 9) break;        // GAME: max useful depth (remaining plies)
    }
    return best;
}

// ---------------------------------------------------------------- GAME: I/O
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (const char *e = getenv("CG_NODES")) NODE_LIMIT = atoll(e);
    State s;
    int oppRow, oppCol;
    while (cin >> oppRow >> oppCol) {
        t0 = chrono::steady_clock::now();
        if (oppRow >= 0 && oppRow < 3 && oppCol >= 0 && oppCol < 3)
            s.cell[oppRow * 3 + oppCol] = 2;
        int k;
        if (!(cin >> k)) break;
        for (int i = 0; i < k; ++i) { int r, c; cin >> r >> c; }  // recomputed locally
        s.toMove = 1;
        Move m = search(s);
        s.cell[m.r * 3 + m.c] = 1;
        cout << m.r << ' ' << m.c << endl;  // endl flushes: mandatory on CG
    }
    return 0;
}
