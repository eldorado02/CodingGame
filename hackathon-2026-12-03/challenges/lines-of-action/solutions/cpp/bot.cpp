// Lines of Action 8x8 — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Index = y * 8 + x, y = 0 is the first input line (rank 8).
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int N = 8, CELLS = N * N;
constexpr long long FIRST_TURN_NODES = 100000, TURN_NODES = 20000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 100;
constexpr int GROUP_WEIGHT = 10;
constexpr int DX[8] = {0, 0, -1, 1, -1, 1, -1, 1}, DY[8] = {-1, 1, 0, 0, -1, 1, 1, -1};
constexpr char EMPTY = '.';
constexpr int MAX_MOVES = 12 * 8;

struct Move { int src, dst; };
struct State { char cell[CELLS]; char me, op; };

int lineCount(const char *c, int x, int y, int dx, int dy) {
    int n = 1;  // the moving checker itself
    for (int sgn = 1; sgn >= -1; sgn -= 2)
        for (int cx = x + sgn * dx, cy = y + sgn * dy; cx >= 0 && cx < N && cy >= 0 && cy < N; cx += sgn * dx, cy += sgn * dy)
            if (c[cy * N + cx] != EMPTY) ++n;
    return n;
}

int genMoves(const State &s, Move *out) {
    int n = 0;
    for (int i = 0; i < CELLS; ++i) {
        if (s.cell[i] != s.me) continue;
        int x = i % N, y = i / N;
        for (int d = 0; d < 8; ++d) {
            int k = lineCount(s.cell, x, y, DX[d], DY[d]);
            int tx = x + k * DX[d], ty = y + k * DY[d];
            if (tx < 0 || tx >= N || ty < 0 || ty >= N) continue;
            int t = ty * N + tx;
            if (s.cell[t] == s.me) continue;
            bool blocked = false;
            for (int j = 1; j < k && !blocked; ++j) blocked = s.cell[(y + j * DY[d]) * N + x + j * DX[d]] == s.op;
            if (!blocked) out[n++] = {i, t};
        }
    }
    return n;
}

inline State applyMove(const State &s, const Move &m) {
    State t = s;
    t.cell[m.dst] = s.cell[m.src];
    t.cell[m.src] = EMPTY;
    t.me = s.op;
    t.op = s.me;
    return t;
}

int groups(const char *c, char color) {
    bool seen[CELLS] = {false};
    int stack[CELLS], count = 0;
    for (int i = 0; i < CELLS; ++i) {
        if (c[i] != color || seen[i]) continue;
        ++count;
        int top = 0;
        stack[top++] = i;
        seen[i] = true;
        while (top) {
            int j = stack[--top], x = j % N, y = j / N;
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
                    int k = ny * N + nx;
                    if (!seen[k] && c[k] == color) { seen[k] = true; stack[top++] = k; }
                }
        }
    }
    return count;
}

// s is the position after a move by s.op: the mover wins ties.
char winnerAfterMove(const State &s) {
    if (groups(s.cell, s.op) <= 1) return s.op;
    if (groups(s.cell, s.me) <= 1) return s.me;
    return 0;
}

int sideScore(const char *c, char color) {
    int cells[CELLS], n = 0, sx = 0, sy = 0;
    for (int i = 0; i < CELLS; ++i) if (c[i] == color) { cells[n++] = i; sx += i % N; sy += i / N; }
    if (!n) return 0;
    int cx = sx / n, cy = sy / n, spread = 0;
    for (int k = 0; k < n; ++k) spread += max(abs(cells[k] % N - cx), abs(cells[k] / N - cy));
    return -spread - GROUP_WEIGHT * (groups(c, color) - 1);
}

inline int evaluate(const State &s) { return sideScore(s.cell, s.me) - sideScore(s.cell, s.op); }

inline int orderKey(const State &s, const Move &m) { return s.cell[m.dst] == s.op ? 1 : 0; }

void orderMoves(const State &s, Move *mv, int n) {  // stable, captures first
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int k = orderKey(s, x), j = i - 1;
        while (j >= 0 && orderKey(s, mv[j]) < k) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
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

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    char w = winnerAfterMove(s);
    if (w) return w == s.me ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (!n) {
        State passed = s;
        passed.me = s.op;
        passed.op = s.me;
        return -negamax(passed, depth - 1, -beta, -alpha, ply + 1);
    }
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
    int n = (int)mv.size();
    orderMoves(root, mv.data(), n);
    Move best = mv[0];
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

int square(const string &t, size_t at) {
    char c = t[at], r = t[at + 1];
    if (c < 'a' || c > 'h' || r < '1' || r > '8') return -1;
    return (N - (r - '0')) * N + (c - 'a');
}

string nameOf(int i) { return string(1, char('a' + i % N)) + char('0' + N - i / N); }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    char me = line.empty() ? 'b' : line[0], op = me == 'b' ? 'w' : 'b';
    bool first = true;
    while (true) {
        State s;
        for (int y = 0; y < N; ++y) {
            if (!getline(cin, line)) return 0;
            if (y == 0) t0 = chrono::steady_clock::now();
            for (int x = 0; x < N; ++x) s.cell[y * N + x] = x < (int)line.size() && line[x] != '\r' ? line[x] : EMPTY;
        }
        s.me = me;
        s.op = op;
        if (!getline(cin, line) || !getline(cin, line)) return 0;
        int n = atoi(line.c_str());
        vector<Move> legal;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            size_t a = line.find_first_not_of(" \t");
            if (a == string::npos || line.size() < a + 4) continue;
            string t = line.substr(a, 4);
            int p = square(t, 0), q = square(t, 2);
            if (p >= 0 && q >= 0) legal.push_back({p, q});
        }
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = first ? FIRST_TURN_MS : TURN_MS;
        first = false;
        if (legal.empty()) { cout << "pass" << endl; continue; }
        Move m = search(s, legal);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << nameOf(m.src) << nameOf(m.dst) << endl;
    }
}
