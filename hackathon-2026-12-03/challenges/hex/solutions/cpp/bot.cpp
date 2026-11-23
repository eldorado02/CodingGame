// Hex 11x11 (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Cell = row*11+col. Neighbours (-1,0)(-1,1)(0,-1)(0,1)(1,-1)(1,0).
// Red(1): row 0 -> row 10. Blue(2): col 0 -> col 10.
// v1: immediate win / forced block, then MCTS + RAVE (no exploration),
// random-fill playouts. Budget = iterations (env CG_NODES) -> byte-identical.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

constexpr int N = 11, NC = N * N, RED = 1, BLUE = 2;
constexpr double RAVE_B = 0.04;
constexpr int MAX_NODES = 2000000;
constexpr int EXPAND_N = 3;  // expand a leaf after this many visits
constexpr int OPENING = 1 * N + 2;  // "c2"
int NB[NC][6], NBN[NC];

void initNb() {
    const int dr[6] = {-1, -1, 0, 0, 1, 1}, dc[6] = {0, 1, -1, 1, -1, 0};
    for (int c = 0; c < NC; ++c) {
        int r = c / N, q = c % N, k = 0;
        for (int i = 0; i < 6; ++i) {
            int a = r + dr[i], b = q + dc[i];
            if (a >= 0 && a < N && b >= 0 && b < N) NB[c][k++] = a * N + b;
        }
        NBN[c] = k;
    }
}

bool connected(const int8_t *board, int p) {
    int stack[NC], sp = 0;
    bool seen[NC];
    memset(seen, 0, sizeof seen);
    if (p == RED) { for (int c = 0; c < N; ++c) if (board[c] == RED) { stack[sp++] = c; seen[c] = true; } }
    else { for (int r = 0; r < N; ++r) if (board[r * N] == BLUE) { stack[sp++] = r * N; seen[r * N] = true; } }
    while (sp) {
        int c = stack[--sp];
        if ((p == RED && c / N == N - 1) || (p == BLUE && c % N == N - 1)) return true;
        for (int i = 0; i < NBN[c]; ++i) {
            int d = NB[c][i];
            if (!seen[d] && board[d] == p) { seen[d] = true; stack[sp++] = d; }
        }
    }
    return false;
}

struct Rng {
    uint64_t s;
    uint64_t next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s; }
};

// Tree in static arrays (vectors are slow without -O2 on CodinGame).
static int T_cell[MAX_NODES], T_first[MAX_NODES], T_nch[MAX_NODES];
static int T_n[MAX_NODES], T_w[MAX_NODES], T_an[MAX_NODES], T_aw[MAX_NODES];
int T_size = 0;

struct Mcts {
    const int8_t *rootBoard;
    int me, iters, done = 0;
    Rng rng{0x9E3779B97F4A7C15ULL};
    chrono::steady_clock::time_point t0;
    int timeMs;

    int newNode(int c) {
        int i = T_size++;
        T_cell[i] = c; T_first[i] = -1; T_nch[i] = 0; T_n[i] = 0; T_w[i] = 0; T_an[i] = 0; T_aw[i] = 0;
        return i;
    }

    void expand(int node, const int8_t *board) {
        if (T_size + NC > MAX_NODES) return;
        T_first[node] = T_size;
        int k = 0;
        for (int c = 0; c < NC; ++c)
            if (board[c] == 0) { newNode(c); ++k; }
        T_nch[node] = k;
    }

    int select(int node) const {
        int best = -1;
        double bestv = -1.0;
        int f = T_first[node], e = f + T_nch[node];
        for (int i = f; i < e; ++i) {
            int ni = T_n[i], ai = T_an[i];
            double v;
            if (ai == 0) v = ni > 0 ? (double)T_w[i] / ni : 0.5;
            else if (ni == 0) v = (double)T_aw[i] / ai;
            else {
                double beta = (double)ai / ((double)ai + (double)ni + RAVE_B * (double)ai * (double)ni);
                v = (1.0 - beta) * ((double)T_w[i] / ni) + beta * ((double)T_aw[i] / ai);
            }
            if (v > bestv) { bestv = v; best = i; }
        }
        return best;
    }

    int run() {
        T_size = 0;
        newNode(-1);
        expand(0, rootBoard);
        int8_t board[NC];
        int path[NC + 2], empt[NC];
        for (int it = 0; it < iters; ++it) {
            if ((it & 255) == 0 && it > 0 &&
                chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > timeMs) {
                cerr << "TIMEGUARD" << endl;
                break;
            }
            memcpy(board, rootBoard, NC);
            int node = 0, p = me, plen = 0;
            path[plen++] = 0;
            while (T_first[node] >= 0 && T_nch[node] > 0) {
                node = select(node);
                board[T_cell[node]] = (int8_t)p;
                p = 3 - p;
                path[plen++] = node;
            }
            if (T_first[node] < 0 && T_n[node] >= EXPAND_N) {
                expand(node, board);
                if (T_first[node] >= 0 && T_nch[node] > 0) {
                    node = T_first[node];
                    board[T_cell[node]] = (int8_t)p;
                    p = 3 - p;
                    path[plen++] = node;
                }
            }
            int k = 0;
            for (int c = 0; c < NC; ++c) if (board[c] == 0) empt[k++] = c;
            for (int i = k - 1; i > 0; --i) {
                int j = (int)(((rng.next() >> 32) * (uint64_t)(i + 1)) >> 32);
                int t = empt[i]; empt[i] = empt[j]; empt[j] = t;
            }
            int q = p;
            for (int i = 0; i < k; ++i) { board[empt[i]] = (int8_t)q; q = 3 - q; }
            int winner = connected(board, RED) ? RED : BLUE;
            int mover = me, pl = me;
            for (int d = 0; d < plen; ++d) {
                int nd = path[d];
                T_n[nd]++;
                if (d > 0) { if (winner == mover) T_w[nd]++; mover = 3 - mover; }
                int f = T_first[nd];
                if (f >= 0) {
                    int win = winner == pl ? 1 : 0, e = f + T_nch[nd];
                    for (int i = f; i < e; ++i)
                        if (board[T_cell[i]] == pl) { T_an[i]++; T_aw[i] += win; }
                }
                pl = 3 - pl;
            }
            ++done;
        }
        int f = T_first[0], best = -1, bn = -1;
        for (int i = f; i < f + T_nch[0]; ++i) if (T_n[i] > bn) { bn = T_n[i]; best = i; }
        return T_cell[best];
    }
};

string nameOf(int c) { return string(1, char('a' + c % N)) + to_string(c / N + 1); }

string choose(int8_t *board, int me, const string &last, int iters, int timeMs,
              chrono::steady_clock::time_point t0, string &info) {
    int reds = 0, blues = 0;
    for (int c = 0; c < NC; ++c) { reds += board[c] == RED; blues += board[c] == BLUE; }
    if (reds == 0 && blues == 0) { info = "opening"; return nameOf(OPENING); }
    if (me == BLUE && reds == 1 && blues == 0 && last != "empty") {
        int c = 0;
        while (board[c] != RED) ++c;
        int r = c / N, q = c % N;
        if (r >= 1 && r <= N - 2 && q >= 1 && q <= N - 2) { info = "swap"; return "swap-pieces"; }
    }
    int opp = 3 - me;
    for (int c = 0; c < NC; ++c)
        if (board[c] == 0) {
            board[c] = (int8_t)me; bool ok = connected(board, me); board[c] = 0;
            if (ok) { info = "win"; return nameOf(c); }
        }
    for (int c = 0; c < NC; ++c)
        if (board[c] == 0) {
            board[c] = (int8_t)opp; bool ok = connected(board, opp); board[c] = 0;
            if (ok) { info = "block"; return nameOf(c); }
        }
    Mcts m;
    m.rootBoard = board; m.me = me; m.iters = iters; m.t0 = t0; m.timeMs = timeMs;
    int c = m.run();
    info = "mcts it=" + to_string(m.done) + " nodes=" + to_string(T_size);
    return nameOf(c);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initNb();
    string line;
    if (!getline(cin, line)) return 0;
    int me = line.find("red") != string::npos ? RED : BLUE;
    long long cli = -1;
    if (const char *e = getenv("CG_NODES")) cli = atoll(e);
    int turn = 0;
    while (getline(cin, line)) {
        auto t0 = chrono::steady_clock::now();
        string last;
        { size_t a = line.find_first_not_of(" \t\r"); if (a == string::npos) last = "empty";
          else { size_t b = line.find_first_of(" \t\r", a); last = line.substr(a, b == string::npos ? string::npos : b - a); } }
        int8_t board[NC];
        for (int r = 0; r < N; ++r) {
            string row, raw;
            getline(cin, raw);
            for (char ch : raw) if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') row += ch;
            for (int q = 0; q < N; ++q) {
                char ch = q < (int)row.size() ? row[q] : '.';
                board[r * N + q] = (int8_t)(ch == 'r' ? RED : ch == 'b' ? BLUE : 0);
            }
        }
        // CG ~ budget calibrated from the d=/ms= stderr line of test games
        int iters = cli > 0 ? (int)cli : (turn == 0 ? 30000 : 2600);
        int timeMs = turn == 0 ? 700 : 70;
        string info;
        string mv = choose(board, me, last, iters, timeMs, t0, info);
        cerr << info << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << mv << endl;
        ++turn;
    }
    return 0;
}
