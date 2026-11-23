// Clobber — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Bit index = y * 8 + x, y = 0 is the first input line (top row).
// Eval = mobility difference. CodinGame compiles without -O2: pragma needed.
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using U64 = uint64_t;

constexpr int STRIDE = 8;
constexpr long long FIRST_TURN_NODES = 400000, TURN_NODES = 60000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 100;
constexpr int DX[4] = {0, 0, -1, 1}, DY[4] = {-1, 1, 0, 0};  // same order as Python DIRS
constexpr int MAX_MOVES = 64 * 4;

struct Move { int src, dst; };
struct State { U64 mine, theirs; };  // mine = side to move

int boardSize = 8;
U64 boardMask = 0;
constexpr U64 COL0 = 0x0101010101010101ULL, COL7 = 0x8080808080808080ULL;
vector<int> cells;
int NB[64][4], NBN[64];

void initBoard(int n) {
    boardSize = n;
    cells.clear();
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x) {
            int i = y * STRIDE + x, k = 0;
            cells.push_back(i);
            boardMask |= 1ULL << i;
            for (int d = 0; d < 4; ++d) {
                int nx = x + DX[d], ny = y + DY[d];
                if (nx >= 0 && nx < n && ny >= 0 && ny < n) NB[i][k++] = ny * STRIDE + nx;
            }
            NBN[i] = k;
        }
}

int genMoves(const State &s, Move *out) {
    int n = 0;
    for (int i : cells) {
        if (!((s.mine >> i) & 1)) continue;
        for (int k = 0; k < NBN[i]; ++k)
            if ((s.theirs >> NB[i][k]) & 1) out[n++] = {i, NB[i][k]};
    }
    return n;
}

// Each shift maps every stone to its neighbour in one direction, so the
// popcount of (shifted & other) is the number of moves in that direction.
// Wrap-around between rows is masked out by the column masks.
int countMoves(U64 own, U64 other) {
    U64 up = own >> STRIDE, down = (own << STRIDE) & boardMask;
    U64 left = (own >> 1) & ~COL7 & boardMask, right = (own << 1) & ~COL0 & boardMask;
    return __builtin_popcountll(up & other) + __builtin_popcountll(down & other)
         + __builtin_popcountll(left & other) + __builtin_popcountll(right & other);
}

inline State applyMove(const State &s, const Move &m) {
    U64 mine = (s.mine & ~(1ULL << m.src)) | (1ULL << m.dst);
    return {s.theirs & ~(1ULL << m.dst), mine};  // side to move switches
}

inline int evaluate(const State &s) { return countMoves(s.mine, s.theirs) - countMoves(s.theirs, s.mine); }

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
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
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

string squareName(int i) { return string(1, char('a' + i % STRIDE)) + to_string(boardSize - i / STRIDE); }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int n = atoi(line.c_str());
    if (n > STRIDE) n = STRIDE;
    if (n < 1) n = 8;
    initBoard(n);
    if (!getline(cin, line)) return 0;
    char me = 'w';
    for (char ch : line) if (ch != ' ') { me = ch; break; }
    bool first = true;
    while (true) {
        State s = {0, 0};
        for (int y = 0; y < n; ++y) {
            if (!getline(cin, line)) return 0;
            if (y == 0) t0 = chrono::steady_clock::now();
            for (int x = 0; x < n && x < (int)line.size(); ++x) {
                U64 bit = 1ULL << (y * STRIDE + x);
                if (line[x] == me) s.mine |= bit;
                else if (line[x] == 'w' || line[x] == 'b') s.theirs |= bit;
            }
        }
        if (!getline(cin, line) || !getline(cin, line)) return 0;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = first ? FIRST_TURN_MS : TURN_MS;
        first = false;
        Move tmp[MAX_MOVES];
        if (!genMoves(s, tmp)) { cout << "random" << endl; continue; }
        Move m = search(s);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << squareName(m.src) << squareName(m.dst) << endl;
    }
}
