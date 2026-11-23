// Checkers (English draughts) — C++ (submitted version), v1.
// Port of solutions/python/bot.py (reference of the differential fuzz).
// Index = y * 8 + x, y = 0 is row 8, x = 0 is column A. Black moves up.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int EMPTY = 0, BM = 1, BK = 2, RM = 3, RK = 4;
constexpr int SIDE_B = 0, SIDE_R = 1;
constexpr int MAN_V = 100, KING_V = 160;
constexpr int DDX[4] = {-1, 1, -1, 1}, DDY[4] = {-1, -1, 1, 1};
constexpr int MAX_PATH = 16, MAX_MOVES = 96;

struct Move { int len = 0; int sq[MAX_PATH]; };
struct State { int cell[64]; int side; };

inline int sideOf(int v) { return (v == BM || v == BK) ? SIDE_B : SIDE_R; }
inline bool isKing(int v) { return v == BK || v == RK; }
inline int crownRow(int v) { return sideOf(v) == SIDE_B ? 0 : 7; }
inline void dirRange(int v, int &d0, int &d1) {
    if (isKing(v)) { d0 = 0; d1 = 4; }
    else if (sideOf(v) == SIDE_B) { d0 = 0; d1 = 2; }
    else { d0 = 2; d1 = 4; }
}

int nOut;
Move *outBuf;

void jumpsFrom(int *c, int sq, int v, Move &path) {
    int x0 = sq % 8, y0 = sq / 8, d0, d1;
    dirRange(v, d0, d1);
    bool found = false;
    for (int d = d0; d < d1; ++d) {
        int mx = x0 + DDX[d], my = y0 + DDY[d];
        int tx = x0 + 2 * DDX[d], ty = y0 + 2 * DDY[d];
        if (tx < 0 || tx > 7 || ty < 0 || ty > 7) continue;
        int mid = my * 8 + mx, to = ty * 8 + tx, mv = c[mid];
        if (mv == EMPTY || sideOf(mv) == sideOf(v) || c[to] != EMPTY) continue;
        if (path.len >= MAX_PATH - 1) continue;
        found = true;
        c[mid] = EMPTY;
        path.sq[path.len++] = to;
        if (!isKing(v) && ty == crownRow(v)) {
            if (nOut < MAX_MOVES) outBuf[nOut++] = path;
        } else {
            c[to] = v;
            jumpsFrom(c, to, v, path);
            c[to] = EMPTY;
        }
        --path.len;
        c[mid] = mv;
    }
    if (!found && path.len > 1 && nOut < MAX_MOVES) outBuf[nOut++] = path;
}

int genMoves(State &s, Move *out) {
    int *c = s.cell;
    nOut = 0; outBuf = out;
    for (int sq = 0; sq < 64; ++sq) {
        int v = c[sq];
        if (v == EMPTY || sideOf(v) != s.side) continue;
        c[sq] = EMPTY;
        Move p; p.len = 1; p.sq[0] = sq;
        jumpsFrom(c, sq, v, p);
        c[sq] = v;
    }
    if (nOut) return nOut;
    int n = 0;
    for (int sq = 0; sq < 64; ++sq) {
        int v = c[sq];
        if (v == EMPTY || sideOf(v) != s.side) continue;
        int x0 = sq % 8, y0 = sq / 8, d0, d1;
        dirRange(v, d0, d1);
        for (int d = d0; d < d1; ++d) {
            int x = x0 + DDX[d], y = y0 + DDY[d];
            if (x >= 0 && x < 8 && y >= 0 && y < 8 && c[y * 8 + x] == EMPTY && n < MAX_MOVES) {
                out[n].len = 2; out[n].sq[0] = sq; out[n].sq[1] = y * 8 + x; ++n;
            }
        }
    }
    return n;
}

State applyMove(const State &s, const Move &m) {
    State t = s;
    int *c = t.cell;
    int v = c[m.sq[0]];
    c[m.sq[0]] = EMPTY;
    for (int i = 0; i + 1 < m.len; ++i) {
        int a = m.sq[i], b = m.sq[i + 1];
        int ax = a % 8, ay = a / 8, bx = b % 8, by = b / 8;
        if (abs(bx - ax) == 2) c[((ay + by) / 2) * 8 + (ax + bx) / 2] = EMPTY;
    }
    int end = m.sq[m.len - 1];
    if (!isKing(v) && end / 8 == crownRow(v)) v = (v == BM) ? BK : RK;
    c[end] = v;
    t.side = 1 - s.side;
    return t;
}

int evaluate(const State &s) {
    int sc = 0;
    for (int sq = 0; sq < 64; ++sq) {
        int v = s.cell[sq];
        if (v == EMPTY) continue;
        int y = sq / 8, val;
        if (isKing(v)) val = KING_V;
        else {
            int adv = sideOf(v) == SIDE_B ? 7 - y : y;
            val = MAN_V + 2 * adv;
            if (adv == 0) val += 6;
        }
        sc += sideOf(v) == SIDE_B ? val : -val;
    }
    return s.side == SIDE_B ? sc : -sc;
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 40000;
int TIME_MS = 70;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

// stable insertion sort by path length, descending
void orderMoves(Move *mv, int n) {
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int j = i - 1;
        while (j >= 0 && mv[j].len < x.len) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

int negamax(const State &s0, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    State s = s0;
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
    orderMoves(mv, n);
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
    orderMoves(mv.data(), n);
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
bool parsePath(const string &txt, Move &m) {
    string t;
    for (char ch : txt) if (ch != ' ' && ch != '\r' && ch != '\n') t += (char)toupper((unsigned char)ch);
    if (t.size() < 4 || t.size() % 2 || t.size() / 2 > MAX_PATH) return false;
    m.len = 0;
    for (size_t i = 0; i < t.size(); i += 2) {
        char col = t[i], row = t[i + 1];
        if (col < 'A' || col > 'H' || row < '1' || row > '8') return false;
        m.sq[m.len++] = (8 - (row - '0')) * 8 + (col - 'A');
    }
    return true;
}

string nameOf(const Move &m) {
    string r;
    for (int i = 0; i < m.len; ++i) { r += char('A' + m.sq[i] % 8); r += char('0' + 8 - m.sq[i] / 8); }
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int side = SIDE_R;
    for (char ch : line) if (ch != ' ') { side = (tolower((unsigned char)ch) == 'b') ? SIDE_B : SIDE_R; break; }
    while (true) {
        State s;
        bool ok = true;
        for (int y = 0; y < 8 && ok; ++y) {
            string r;
            if (!getline(cin, r)) { ok = false; break; }
            for (int x = 0; x < 8; ++x) {
                char ch = x < (int)r.size() ? r[x] : '.';
                s.cell[y * 8 + x] = ch == 'b' ? BM : ch == 'B' ? BK : ch == 'r' ? RM : ch == 'R' ? RK : EMPTY;
            }
        }
        if (!ok) break;
        t0 = chrono::steady_clock::now();
        string nl;
        if (!getline(cin, nl)) break;
        int n = atoi(nl.c_str());
        vector<Move> given;
        for (int i = 0; i < n; ++i) {
            string t;
            if (!getline(cin, t)) break;
            size_t a = t.find_first_not_of(" \t");
            if (a == string::npos) continue;
            size_t b = t.find_first_of(" \t\r", a);
            Move m;
            if (parsePath(t.substr(a, b == string::npos ? string::npos : b - a), m)) given.push_back(m);
        }
        s.side = side;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : 25000;
        if (given.empty()) { cout << "A1B2" << endl; continue; }
        Move best = search(s, given);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << nameOf(best) << endl;
    }
    return 0;
}
