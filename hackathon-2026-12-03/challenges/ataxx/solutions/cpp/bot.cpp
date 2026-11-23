// Ataxx 7x7 — C++ (submitted), v2 alpha-beta. Port of solutions/python/bot.py.
// Bit index = y * 7 + x, y = 0 is the first input line (row 7).
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

constexpr int SIZE = 7, CELLS = SIZE * SIZE;
constexpr U64 ALL = (1ULL << CELLS) - 1;
constexpr long long FIRST_TURN_NODES = 600000, TURN_NODES = 120000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 70;
constexpr int SPAWN_ORDER_BONUS = 1;  // spawns add a piece: try them before equal jumps
constexpr int MAX_MOVES = 49 * 25;

U64 ADJ[CELLS], JUMP[CELLS];

struct Move { int src, dst; };  // src = -1 for a spawn
struct State {
    U64 mine, theirs, walls;  // mine = pieces of the side to move
    U64 empty() const { return ALL & ~(mine | theirs | walls); }
};

void initTables() {
    for (int i = 0; i < CELLS; ++i) {
        ADJ[i] = JUMP[i] = 0;
        for (int j = 0; j < CELLS; ++j) {
            int d = max(abs(j % SIZE - i % SIZE), abs(j / SIZE - i / SIZE));
            if (d == 1) ADJ[i] |= 1ULL << j;
            else if (d == 2) JUMP[i] |= 1ULL << j;
        }
    }
}

inline int popc(U64 x) { return __builtin_popcountll(x); }

int genMoves(const State &s, Move *out) {
    U64 empty = s.empty(), spawnTargets = 0;
    for (U64 x = s.mine; x; x &= x - 1) spawnTargets |= ADJ[__builtin_ctzll(x)];
    int n = 0;
    for (U64 t = spawnTargets & empty; t; t &= t - 1) out[n++] = {-1, __builtin_ctzll(t)};
    for (U64 x = s.mine; x; x &= x - 1) {
        int p = __builtin_ctzll(x);
        for (U64 t = JUMP[p] & empty; t; t &= t - 1) out[n++] = {p, __builtin_ctzll(t)};
    }
    return n;
}

inline State applyMove(const State &s, const Move &m) {
    U64 mine = s.mine | (1ULL << m.dst);
    if (m.src >= 0) mine &= ~(1ULL << m.src);
    U64 flipped = ADJ[m.dst] & s.theirs;
    mine |= flipped;
    return {s.theirs & ~flipped, mine, s.walls};  // point of view switches
}

inline int orderKey(const State &s, const Move &m) {
    return 2 * popc(ADJ[m.dst] & s.theirs) + (m.src < 0 ? SPAWN_ORDER_BONUS : 0);
}

inline int evaluate(const State &s) { return popc(s.mine) - popc(s.theirs); }

void orderMoves(const State &s, Move *mv, int n) {
    static int key[MAX_MOVES];
    for (int i = 0; i < n; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < n; ++i) {  // stable insertion sort, key descending
        Move x = mv[i];
        int k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
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

inline int finalScore(const State &s, int ply) {
    int diff = evaluate(s);
    return diff > 0 ? WIN - ply : diff < 0 ? -(WIN - ply) : 0;
}

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    if (!s.mine || !s.theirs) return finalScore(s, ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) {
        State passed = {s.theirs, s.mine, s.walls};
        Move tmp[MAX_MOVES];
        if (!genMoves(passed, tmp)) return finalScore(s, ply);
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

// ---------------------------------------------------------------- I/O
int square(const string &t, size_t at) {
    if (at + 2 > t.size()) return -1;
    char c = t[at], r = t[at + 1];
    if (c < 'a' || c > 'g' || r < '1' || r > '7') return -1;
    return (7 - (r - '0')) * SIZE + (c - 'a');
}

bool parseAction(const string &t, Move &m) {
    if (t.size() == 2) { m = {-1, square(t, 0)}; return m.dst >= 0; }
    if (t.size() == 4) { m = {square(t, 0), square(t, 2)}; return m.src >= 0 && m.dst >= 0; }
    return false;
}

string nameOf(int i) { return string(1, char('a' + i % SIZE)) + char('0' + 7 - i / SIZE); }

string actionText(const Move &m) { return m.src < 0 ? nameOf(m.dst) : nameOf(m.src) + nameOf(m.dst); }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initTables();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    char me = line.empty() ? '0' : line[0];
    bool first = true;
    while (true) {
        State s = {0, 0, 0};
        for (int y = 0; y < SIZE; ++y) {
            if (!getline(cin, line)) return 0;
            if (y == 0) t0 = chrono::steady_clock::now();
            for (int x = 0; x < SIZE; ++x) {
                char ch = x < (int)line.size() ? line[x] : '#';
                U64 bit = 1ULL << (y * SIZE + x);
                if (ch == '#' || ch == '\r') s.walls |= bit;
                else if (ch == me) s.mine |= bit;
                else if (ch == '0' || ch == '1') s.theirs |= bit;
            }
        }
        if (!getline(cin, line) || !getline(cin, line)) return 0;
        int n = atoi(line.c_str());
        vector<Move> legal;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            size_t a = line.find_first_not_of(" \t");
            if (a == string::npos) continue;
            size_t b = line.find_first_of(" \t\r", a);
            Move m;
            if (parseAction(line.substr(a, b == string::npos ? string::npos : b - a), m)) legal.push_back(m);
        }
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = first ? FIRST_TURN_MS : TURN_MS;
        first = false;
        if (legal.empty()) { cout << "random" << endl; continue; }
        Move best = search(s, legal);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << actionText(best) << endl;
    }
}
