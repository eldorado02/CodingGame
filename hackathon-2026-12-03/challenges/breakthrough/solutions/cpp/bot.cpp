// Breakthrough 8x8 — C++ (submitted version). Port of solutions/python/bot.py.
// Template board-2p: negamax + alpha-beta + iterative deepening, node budget.
// Board index = rank * 8 + file (rank 0 = "1", file 0 = "a").
// Cell: 0 empty, 1 White (to rank 7), 2 Black (to rank 0).
// Compile: g++ -O2 -std=c++20 -Wall -Wextra bot.cpp -o bot
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------- GAME: state
constexpr int WHITE = 1, BLACK = 2;
constexpr int DIR[3] = {0, 1, -1};
constexpr int GOAL[3] = {0, 7, 0};
constexpr int ADV[8] = {0, 4, 10, 20, 36, 60, 110, 0};
constexpr int PIECE = 1000, DEFENDED = 12, HOME_GUARD = 18;
constexpr int MAX_MOVES = 48;

struct Move { int from = 0, to = 0; };
struct State { array<int, 64> cell{}; int toMove = WHITE; };

State initialState() {
    State s;
    for (int i = 0; i < 16; ++i) { s.cell[i] = WHITE; s.cell[63 - i] = BLACK; }
    return s;
}

int winner(const State &s) {
    for (int i = 0; i < 8; ++i) {
        if (s.cell[56 + i] == WHITE) return WHITE;
        if (s.cell[i] == BLACK) return BLACK;
    }
    int w = 0, b = 0;
    for (int v : s.cell) { if (v == WHITE) ++w; else if (v == BLACK) ++b; }
    if (w == 0) return BLACK;
    if (b == 0) return WHITE;
    return 0;
}

int genMoves(const State &s, Move *out) {
    int p = s.toMove, d = DIR[p], n = 0;
    for (int sq = 0; sq < 64; ++sq) {
        if (s.cell[sq] != p) continue;
        int r = sq >> 3, f = sq & 7, nr = r + d;
        if (nr < 0 || nr > 7) continue;
        for (int df = -1; df <= 1; ++df) {
            int nf = f + df;
            if (nf < 0 || nf > 7) continue;
            int t = nr * 8 + nf, v = s.cell[t];
            if (v == 0 || (df != 0 && v != p)) out[n++] = {sq, t};
        }
    }
    return n;
}

State applyMove(const State &s, const Move &m) {
    State t = s;
    t.cell[m.to] = t.cell[m.from];
    t.cell[m.from] = 0;
    t.toMove = 3 - s.toMove;
    return t;
}

int sideScore(const State &s, int p) {
    int d = DIR[p], sc = 0;
    for (int sq = 0; sq < 64; ++sq) {
        if (s.cell[sq] != p) continue;
        int r = sq >> 3, f = sq & 7;
        int pr = (p == WHITE) ? r : 7 - r;
        sc += PIECE + ADV[pr];
        if (pr == 0) sc += HOME_GUARD;
        int br = r - d;
        if (br >= 0 && br <= 7) {
            if ((f > 0 && s.cell[br * 8 + f - 1] == p) || (f < 7 && s.cell[br * 8 + f + 1] == p))
                sc += DEFENDED;
        }
    }
    return sc;
}

int evaluate(const State &s) { return sideScore(s, s.toMove) - sideScore(s, 3 - s.toMove); }

int orderKey(const State &s, const Move &m) {
    int k = 0;
    if ((m.to >> 3) == GOAL[s.toMove]) k += 4;
    if (s.cell[m.to] != 0) k += 2;
    return k;
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 150000;
int TIME_MS = 60;

long long nodes = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

// stable insertion sort, key descending (no STL in the hot path: CG builds
// without -O2, only the pragma applies)
void orderMoves(const State &s, Move *mv, int n) {
    int key[MAX_MOVES];
    for (int i = 0; i < n; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        Move x = mv[i]; int k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int w = winner(s);
    if (w) return (w == s.toMove) ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) return -(WIN - ply);
    orderMoves(s, mv, n);
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

int lastDepth = 0;

Move search(const State &root, vector<Move> mv) {
    lastDepth = 0;
    int n = (int)mv.size();
    orderMoves(root, mv.data(), n);
    Move best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        Move cur = mv[0];
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = mv[i]; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        lastDepth = depth;
        rotate(mv.begin(), mv.begin() + curIdx, mv.begin() + curIdx + 1);
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// ---------------------------------------------------------------- GAME: I/O
int sqOf(const string &t, int i) { return (t[i + 1] - '1') * 8 + (t[i] - 'a'); }

string nameOf(const Move &m) {
    string r;
    r += char('a' + (m.from & 7)); r += char('1' + (m.from >> 3));
    r += char('a' + (m.to & 7));   r += char('1' + (m.to >> 3));
    return r;
}

bool validMoveText(const string &t) {
    return t.size() >= 4 && t[0] >= 'a' && t[0] <= 'h' && t[1] >= '1' && t[1] <= '8'
        && t[2] >= 'a' && t[2] <= 'h' && t[3] >= '1' && t[3] <= '8';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    State s = initialState();
    int me = 0;
    bool first = true;
    string line;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        string opp;
        { size_t a = line.find_first_not_of(" \t\r"); size_t b = line.find_first_of(" \t\r", a);
          opp = (a == string::npos) ? "None" : line.substr(a, b == string::npos ? string::npos : b - a); }
        string nl;
        if (!getline(cin, nl)) break;
        int n = atoi(nl.c_str());
        vector<string> legal;
        for (int i = 0; i < n; ++i) {
            string t;
            if (!getline(cin, t)) break;
            size_t a = t.find_first_not_of(" \t\r");
            if (a == string::npos) continue;
            t = t.substr(a);
            if (validMoveText(t)) legal.push_back(t.substr(0, 4));
        }
        if (me == 0) me = (opp == "None") ? WHITE : BLACK;
        if (validMoveText(opp)) {
            State t = s; t.toMove = 3 - me;
            s = applyMove(t, {sqOf(opp, 0), sqOf(opp, 2)});
        }
        s.toMove = me;
        // first turn: 1000 ms budget, later 100 ms (CG server ~2x slower)
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? 600000 : 60000);
        TIME_MS = first ? 700 : 75;
        first = false;
        set<string> legalSet(legal.begin(), legal.end());
        Move all[MAX_MOVES];
        int na = genMoves(s, all);
        vector<Move> mine;
        for (int i = 0; i < na; ++i) if (legalSet.count(nameOf(all[i]))) mine.push_back(all[i]);
        string out;
        if (mine.empty()) {
            out = legal.empty() ? "a1a2" : legal[0];
            if (!legal.empty()) s = applyMove(s, {sqOf(out, 0), sqOf(out, 2)});
        } else {
            Move m = search(s, mine);
            out = nameOf(m);
            s = applyMove(s, m);
        }
        // diagnostics on stderr only (ignored by the fuzz comparison)
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << out << endl;
    }
    return 0;
}
