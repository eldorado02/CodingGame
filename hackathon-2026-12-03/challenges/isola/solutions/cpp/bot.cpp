// Isola 9x9 — C++ (submitted), v1. Port of solutions/python/bot.py.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int W = 9, N = 81;
// Budgets calibrated on CG logs: Voronoi BFS per leaf, ~150k nodes/s on CG.
constexpr long long FIRST_TURN_NODES = 60000, TURN_NODES = 10000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 70;
constexpr int MOBILITY_WEIGHT = 3;
int NB[N][8], NBN[N];

struct State { bool gone[N]; int pos[2]; int side; };
struct Action { int mv, rm; };

void initTables() {
    for (int i = 0; i < N; ++i) {
        int x = i % W, y = i / W, n = 0;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                if (!dx && !dy) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < W && ny >= 0 && ny < W) NB[i][n++] = ny * W + nx;
            }
        NBN[i] = n;
    }
}

inline bool isFree(const State &s, int t) { return !s.gone[t] && t != s.pos[0] && t != s.pos[1]; }

inline int mobility(const State &s, int p) {
    int n = 0;
    for (int k = 0; k < NBN[s.pos[p]]; ++k) if (isFree(s, NB[s.pos[p]][k])) ++n;
    return n;
}

int genActions(State &s, Action *out) {
    int me = s.side, op = 1 - s.side, n = 0;
    int from = s.pos[me];
    for (int k = 0; k < NBN[from]; ++k) {
        int t = NB[from][k];
        if (!isFree(s, t)) continue;
        s.pos[me] = t;
        int cnt = 0;
        for (int j = 0; j < NBN[s.pos[op]]; ++j) {
            int r = NB[s.pos[op]][j];
            if (isFree(s, r)) { out[n++] = {t, r}; ++cnt; }
        }
        if (!cnt) for (int r = 0; r < N; ++r) if (isFree(s, r)) { out[n++] = {t, r}; break; }
        s.pos[me] = from;
    }
    return n;
}

inline State applyAction(const State &s, const Action &a) {
    State t = s;
    t.pos[s.side] = a.mv;
    t.gone[a.rm] = true;
    t.side = 1 - s.side;
    return t;
}

int voronoi(const State &s) {
    int dist[2][N], q[N];
    for (int p = 0; p < 2; ++p) {
        for (int i = 0; i < N; ++i) dist[p][i] = -1;
        int h = 0, qt = 0;
        dist[p][s.pos[p]] = 0; q[qt++] = s.pos[p];
        while (h < qt) {
            int i = q[h++];
            for (int k = 0; k < NBN[i]; ++k) {
                int t = NB[i][k];
                if (dist[p][t] < 0 && isFree(s, t)) { dist[p][t] = dist[p][i] + 1; q[qt++] = t; }
            }
        }
    }
    int me = s.side, op = 1 - s.side, sc = 0;
    for (int i = 0; i < N; ++i) {
        int a = dist[me][i], b = dist[op][i];
        if (a > 0 && (b < 0 || a < b)) ++sc;
        else if (b > 0 && (a < 0 || b < a)) --sc;
    }
    return sc;
}

int evaluate(const State &s) {
    return voronoi(s) + MOBILITY_WEIGHT * (mobility(s, s.side) - mobility(s, 1 - s.side));
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
constexpr int MAX_ACT = 8 * 9;
long long NODE_LIMIT = TURN_NODES;
int TIME_MS = TURN_MS;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int negamax(const State &s0, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    State s = s0;
    if (!mobility(s, s.side)) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Action mv[MAX_ACT];
    int n = genActions(s, mv);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        int v = -negamax(applyAction(s, mv[i]), depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

Action search(State root) {
    lastDepth = 0;
    Action mv[MAX_ACT];
    int n = genActions(root, mv);
    Action best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        Action cur = mv[0];
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyAction(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = mv[i]; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        lastDepth = depth;
        Action pv = mv[curIdx];
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
    int mx, my;
    if (!(cin >> mx >> my)) return 0;
    State s;
    memset(s.gone, 0, sizeof s.gone);
    int me = mx == 0 ? 0 : 1, mePos = my * W + mx;
    bool first = true;
    int ox, oy, rx, ry;
    while (cin >> ox >> oy >> rx >> ry) {
        t0 = chrono::steady_clock::now();
        if (rx >= 0 && rx < W && ry >= 0 && ry < W) s.gone[ry * W + rx] = true;
        s.pos[me] = mePos;
        s.pos[1 - me] = oy * W + ox;
        s.side = me;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = first ? FIRST_TURN_MS : TURN_MS;
        first = false;
        if (!mobility(s, me)) { cout << "RANDOM" << endl; continue; }
        Action a = search(s);
        mePos = a.mv;
        s.gone[a.rm] = true;
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << a.mv % W << ' ' << a.mv / W << ' ' << a.rm % W << ' ' << a.rm / W << endl;
    }
    return 0;
}
