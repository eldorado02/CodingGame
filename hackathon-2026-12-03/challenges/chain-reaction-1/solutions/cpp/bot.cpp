// Chain Reaction 6x6 — C++ (submitted version), v1. Port of solutions/python/bot.py.
// Index = y * 6 + x, y = 0 is row 6 (first input line), x = 0 is column a.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int N = 36, MAX_ROUNDS = 200;
int NB[N][4], NBN[N], CRIT[N];

struct State { int own[N]; int orb[N]; int side; };

void initTables() {
    for (int i = 0; i < N; ++i) {
        int x = i % 6, y = i / 6, n = 0;
        const int dx[4] = {1, -1, 0, 0}, dy[4] = {0, 0, 1, -1};
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < 6 && ny >= 0 && ny < 6) NB[i][n++] = ny * 6 + nx;
        }
        NBN[i] = n;
        CRIT[i] = n;
    }
}

int genMoves(const State &s, int *out) {
    int n = 0;
    for (int i = 0; i < N; ++i) if (s.own[i] == 0 || s.own[i] == s.side) out[n++] = i;
    return n;
}

State applyMove(const State &s, int i) {
    State t = s;
    int p = s.side, o = 3 - p;
    t.own[i] = p;
    t.orb[i] += 1;
    int crit[N];
    for (int rounds = 0; rounds < MAX_ROUNDS; ++rounds) {
        int nc = 0;
        for (int j = 0; j < N; ++j) if (t.orb[j] >= CRIT[j]) crit[nc++] = j;
        if (!nc) break;
        for (int c = 0; c < nc; ++c) {
            int j = crit[c];
            t.orb[j] -= CRIT[j];
            if (t.orb[j] == 0) t.own[j] = 0;
            for (int k = 0; k < NBN[j]; ++k) { t.orb[NB[j][k]] += 1; t.own[NB[j][k]] = p; }
        }
        bool any = false;
        for (int j = 0; j < N; ++j) if (t.own[j] == o) { any = true; break; }
        if (!any) break;
    }
    t.side = o;
    return t;
}

int winner(const State &s) {
    int total = 0, r = 0, b = 0;
    for (int i = 0; i < N; ++i) {
        total += s.orb[i];
        if (s.own[i] == 1) ++r; else if (s.own[i] == 2) ++b;
    }
    if (total < 2) return 0;
    if (r == 0 && b > 0) return 2;
    if (b == 0 && r > 0) return 1;
    return 0;
}

int sideScore(const State &s, int p) {
    int o = 3 - p, sc = 0;
    for (int i = 0; i < N; ++i) {
        if (s.own[i] != p) continue;
        sc += s.orb[i];
        bool vulnerable = false;
        for (int k = 0; k < NBN[i]; ++k) {
            int q = NB[i][k];
            if (s.own[q] == o && s.orb[q] == CRIT[q] - 1) { vulnerable = true; sc -= 5 - CRIT[i]; }
        }
        if (!vulnerable) {
            if (CRIT[i] == 2) sc += 3;
            else if (CRIT[i] == 3) sc += 2;
            if (s.orb[i] == CRIT[i] - 1) sc += 2;
        }
    }
    return sc;
}

int evaluate(const State &s) { return sideScore(s, s.side) - sideScore(s, 3 - s.side); }

int orderKey(const State &s, int i) {
    int k = 0;
    if (s.own[i] == s.side && s.orb[i] == CRIT[i] - 1) k += 4;
    return k + 4 - CRIT[i];
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 20000;
int TIME_MS = 70;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

void orderMoves(const State &s, int *mv, int n) {
    int key[N];
    for (int i = 0; i < n; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        int x = mv[i], k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int w = winner(s);
    if (w) return (w == s.side) ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    int mv[N];
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

int search(const State &root, vector<int> mv) {
    lastDepth = 0;
    int n = (int)mv.size();
    orderMoves(root, mv.data(), n);
    int best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0, cur = mv[0];
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
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

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initTables();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int me = 1;
    for (char ch : line) if (ch != ' ') { me = (ch == 'b' || ch == 'B') ? 2 : 1; break; }
    bool first = true;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        State s;
        bool ok = true;
        for (int y = 0; y < 6; ++y) {
            string row;
            if (!getline(cin, row)) { ok = false; break; }
            for (int x = 0; x < 6; ++x) {
                int i = y * 6 + x;
                s.own[i] = 0; s.orb[i] = 0;
                if ((int)row.size() >= 2 * x + 2) {
                    char a = row[2 * x], b = row[2 * x + 1];
                    if ((a == 'r' || a == 'b') && b >= '1' && b <= '3') {
                        s.own[i] = a == 'r' ? 1 : 2;
                        s.orb[i] = b - '0';
                    }
                }
            }
        }
        if (!ok) break;
        s.side = me;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? 250000 : 50000);
        TIME_MS = first ? 700 : 70;
        first = false;
        int all[N];
        int n = genMoves(s, all);
        if (!n) { cout << "random" << endl; continue; }
        int m = search(s, vector<int>(all, all + n));
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << char('a' + m % 6) << char('0' + 6 - m / 6) << endl;
    }
    return 0;
}
