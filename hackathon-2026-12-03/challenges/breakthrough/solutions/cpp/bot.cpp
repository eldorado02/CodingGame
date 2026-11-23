// Breakthrough 8x8 — C++ (submitted version), v3 bitboards.
// Port of solutions/python/bot.py (reference of the differential fuzz).
// Bit index = rank * 8 + file (rank 0 = "1", file 0 = "a").
// White (1) moves to rank 7, Black (2) to rank 0.
// Runner rule at every node: side to move with a piece one step from goal
// wins next move; opponent with >=2 runners, or 1 we cannot capture -> lost.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;
using U64 = uint64_t;

constexpr int WHITE = 1, BLACK = 2;
constexpr U64 FILE_A = 0x0101010101010101ULL;
constexpr U64 FILE_H = 0x8080808080808080ULL;
constexpr U64 rankMask(int r) { return 0xFFULL << (8 * r); }
constexpr int ADV[8] = {0, 4, 10, 20, 36, 60, 110, 0};
constexpr int PIECE = 1000, DEFENDED = 12, HOME_GUARD = 18;
constexpr int MAX_MOVES = 48;

struct Move { int from = 0, to = 0; };
struct State {
    U64 w = 0xFFFFULL, b = 0xFFFFULL << 48;
    int toMove = WHITE;
    U64 own(int p) const { return p == WHITE ? w : b; }
};

inline int popc(U64 x) { return __builtin_popcountll(x); }

inline U64 attacks(int p, U64 x) {
    if (p == WHITE) return ((x << 7) & ~FILE_H) | ((x << 9) & ~FILE_A);
    return ((x >> 9) & ~FILE_H) | ((x >> 7) & ~FILE_A);
}

inline U64 runners(int p, U64 x) { return x & (p == WHITE ? rankMask(6) : rankMask(1)); }

inline int winner(const State &s) {
    if (s.w & rankMask(7)) return WHITE;
    if (s.b & rankMask(0)) return BLACK;
    if (!s.w) return BLACK;
    if (!s.b) return WHITE;
    return 0;
}

int genMoves(const State &s, Move *out) {
    int p = s.toMove, d = (p == WHITE) ? 1 : -1, n = 0;
    U64 mine = s.own(p), occ = s.w | s.b, x = mine;
    while (x) {
        int sq = __builtin_ctzll(x);
        x &= x - 1;
        int r = sq >> 3, f = sq & 7, nr = r + d;
        if (nr < 0 || nr > 7) continue;
        for (int df = -1; df <= 1; ++df) {
            int nf = f + df;
            if (nf < 0 || nf > 7) continue;
            int t = nr * 8 + nf;
            U64 tb = 1ULL << t;
            if (tb & mine) continue;
            if (df == 0 && (tb & occ)) continue;
            out[n++] = {sq, t};
        }
    }
    return n;
}

inline State applyMove(const State &s, const Move &m) {
    U64 fb = 1ULL << m.from, tb = 1ULL << m.to;
    State t;
    if (s.toMove == WHITE) { t.w = (s.w ^ fb) | tb; t.b = s.b & ~tb; t.toMove = BLACK; }
    else { t.w = s.w & ~tb; t.b = (s.b ^ fb) | tb; t.toMove = WHITE; }
    return t;
}

int sideScore(int p, U64 x) {
    int sc = PIECE * popc(x);
    for (int r = 0; r < 8; ++r) {
        int c = popc(x & rankMask(r));
        if (c) sc += c * ADV[p == WHITE ? r : 7 - r];
    }
    sc += HOME_GUARD * popc(x & (p == WHITE ? rankMask(0) : rankMask(7)));
    sc += DEFENDED * popc(x & attacks(p, x));
    return sc;
}

int evaluate(const State &s) {
    int p = s.toMove, o = 3 - p;
    return sideScore(p, s.own(p)) - sideScore(o, s.own(o));
}

inline int orderKey(const State &s, const Move &m) {
    int k = 0, goal = (s.toMove == WHITE) ? 7 : 0;
    if ((m.to >> 3) == goal) k += 4;
    if ((1ULL << m.to) & (s.w | s.b)) k += 2;
    return k;
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 60000;
int TIME_MS = 75;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

inline int runnerRule(const State &s, int ply) {
    int p = s.toMove, o = 3 - p;
    if (runners(p, s.own(p))) return WIN - (ply + 1);
    U64 r = runners(o, s.own(o));
    if (r && ((r & (r - 1)) || !(r & attacks(p, s.own(p))))) return -(WIN - (ply + 2));
    return 0;
}

// stable insertion sort, key descending (no STL in the hot path)
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
    int q = runnerRule(s, ply);
    if (q) return q;
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
        Move pv = mv[curIdx];
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
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

string firstToken(const string &line) {
    size_t a = line.find_first_not_of(" \t\r");
    if (a == string::npos) return "";
    size_t b = line.find_first_of(" \t\r", a);
    return line.substr(a, b == string::npos ? string::npos : b - a);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    State s;
    int me = 0;
    bool first = true;
    string line;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        string opp = firstToken(line);
        if (opp.empty()) opp = "None";
        string nl;
        if (!getline(cin, nl)) break;
        int n = atoi(nl.c_str());
        vector<string> legal;
        for (int i = 0; i < n; ++i) {
            string t;
            if (!getline(cin, t)) break;
            t = firstToken(t);
            if (validMoveText(t)) legal.push_back(t.substr(0, 4));
        }
        if (me == 0) me = (opp == "None") ? WHITE : BLACK;
        if (validMoveText(opp)) {
            s.toMove = 3 - me;
            s = applyMove(s, {sqOf(opp, 0), sqOf(opp, 2)});
        }
        s.toMove = me;
        // CG server ~1.2 M nodes/s: 600k on turn 1 (1000 ms), 60k later (100 ms)
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
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << out << endl;
    }
    return 0;
}
