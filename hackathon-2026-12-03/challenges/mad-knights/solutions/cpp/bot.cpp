// Mad Knights (3-player knight isolation) — C++ (submitted), v1.
// Port of solutions/python/bot.py: paranoid alpha-beta, node budget.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using U64 = uint64_t;

const int KX[8] = {1, 2, 2, 1, -1, -2, -2, -1}, KY[8] = {2, 1, -1, -2, -2, -1, 1, 2};
int TG[64][8], TGN[64];

struct State { U64 blocked; int pos[3]; bool alive[3]; int turn; };
int ME = 0;

void initTables() {
    for (int i = 0; i < 64; ++i) {
        int x = i % 8, y = i / 8, n = 0;
        for (int k = 0; k < 8; ++k) {
            int nx = x + KX[k], ny = y + KY[k];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) TG[i][n++] = ny * 8 + nx;
        }
        TGN[i] = n;
    }
}

inline int movesOf(U64 b, int sq, int *out) {
    int n = 0;
    for (int k = 0; k < TGN[sq]; ++k) if (!((b >> TG[sq][k]) & 1)) out[n++] = TG[sq][k];
    return n;
}

inline int mob(U64 b, int sq) {
    int n = 0;
    for (int k = 0; k < TGN[sq]; ++k) if (!((b >> TG[sq][k]) & 1)) ++n;
    return n;
}

inline int nextAlive(const bool *alive, int turn) {
    for (int k = 1; k < 4; ++k) { int j = (turn + k) % 3; if (alive[j]) return j; }
    return turn;
}

inline State applyMove(const State &s, int t) {
    State r = s;
    r.pos[s.turn] = t;
    r.blocked = s.blocked | (1ULL << t);
    r.turn = nextAlive(s.alive, s.turn);
    return r;
}

inline State eliminate(const State &s) {
    State r = s;
    r.alive[s.turn] = false;
    r.turn = nextAlive(r.alive, s.turn);
    return r;
}

int evaluate(const State &s) {
    int sc = 0, mv[8];
    int n = movesOf(s.blocked, s.pos[ME], mv);
    for (int i = 0; i < n; ++i) sc += 10 + mob(s.blocked | (1ULL << mv[i]), mv[i]);
    for (int j = 0; j < 3; ++j) if (j != ME && s.alive[j]) sc -= 6 * mob(s.blocked, s.pos[j]);
    return sc;
}

inline int orderKey(const State &s, int t) { return mob(s.blocked | (1ULL << t), t); }

void orderMoves(const State &s, int *mv, int n) {
    int key[64];
    for (int i = 0; i < n; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        int x = mv[i], k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 60000;
int TIME_MS = 70;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int minimax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    if (!s.alive[ME]) return -(WIN - ply);
    if ((int)s.alive[0] + (int)s.alive[1] + (int)s.alive[2] == 1) return WIN - ply;
    int mv[8];
    int n = movesOf(s.blocked, s.pos[s.turn], mv);
    if (n == 0) return minimax(eliminate(s), depth, alpha, beta, ply);
    if (depth == 0) return evaluate(s);
    orderMoves(s, mv, n);
    if (s.turn == ME) {
        int best = -INF;
        for (int i = 0; i < n; ++i) {
            int v = minimax(applyMove(s, mv[i]), depth - 1, alpha, beta, ply + 1);
            if (aborted) return 0;
            if (v > best) best = v;
            if (v > alpha) alpha = v;
            if (alpha >= beta) break;
        }
        return best;
    }
    int best = INF;
    for (int i = 0; i < n; ++i) {
        int v = minimax(applyMove(s, mv[i]), depth - 1, alpha, beta, ply + 1);
        if (aborted) return 0;
        if (v < best) best = v;
        if (v < beta) beta = v;
        if (alpha >= beta) break;
    }
    return best;
}

int search(const State &root, vector<int> mv) {
    lastDepth = 0;
    int n = (int)mv.size();
    orderMoves(root, mv.data(), n);
    int best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, cur = mv[0], curIdx = 0;
        for (int i = 0; i < n; ++i) {
            int v = minimax(applyMove(root, mv[i]), depth - 1, alpha, INF, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = mv[i]; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        lastDepth = depth;
        int pv = mv[curIdx];
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// ---------------------------------------------------------------- GAME: I/O
int colorIdx(char c) { return c == 'r' ? 0 : c == 'g' ? 1 : c == 'b' ? 2 : -1; }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initTables();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    for (char ch : line) if (ch != ' ') { ME = colorIdx(ch) < 0 ? 0 : colorIdx(ch); break; }
    bool first = true;
    while (true) {
        State s;
        s.alive[0] = s.alive[1] = s.alive[2] = true;
        bool ok = true;
        for (int k = 0; k < 3; ++k) {
            if (!getline(cin, line)) { ok = false; break; }
            if (k == 0) t0 = chrono::steady_clock::now();
            size_t a = line.find_first_not_of(" \t");
            if (a == string::npos) continue;
            int c = colorIdx(line[a]);
            size_t b = line.find_first_not_of(" \t", a + 1);
            if (c >= 0 && b != string::npos) s.alive[c] = line[b] == '1';
        }
        if (!ok) break;
        s.blocked = 0; s.pos[0] = s.pos[1] = s.pos[2] = 0;
        for (int y = 0; y < 8; ++y) {
            if (!getline(cin, line)) line = "";
            for (int x = 0; x < 8; ++x) {
                char ch = x < (int)line.size() ? line[x] : '#';
                if (ch == '\r') ch = '#';
                if (ch != '.') s.blocked |= 1ULL << (y * 8 + x);
                int c = colorIdx(ch);
                if (c >= 0) s.pos[c] = y * 8 + x;
            }
        }
        if (!getline(cin, line)) break;
        int n = atoi(line.c_str());
        vector<int> legal;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) break;
            size_t a = line.find_first_not_of(" \t");
            if (a == string::npos || a + 1 >= line.size()) continue;
            char c0 = line[a], c1 = line[a + 1];
            if (c0 >= 'a' && c0 <= 'h' && c1 >= '1' && c1 <= '8') legal.push_back((8 - (c1 - '0')) * 8 + (c0 - 'a'));
        }
        s.alive[ME] = true;
        s.turn = ME;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? 400000 : 60000);
        TIME_MS = first ? 700 : 70;
        first = false;
        if (legal.empty()) { cout << "random" << endl; continue; }
        int m = search(s, legal);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << char('a' + m % 8) << char('0' + 8 - m / 8) << endl;
    }
    return 0;
}
