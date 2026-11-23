// Bandas 8x8 (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Rules ported from the referee (Oli8/CG-bandas, Referee.java), quirks included: pawns of the
// mover step in the referee's scan order, recursively pushing pawns in front; leaving the grid
// or entering a hole removes a pawn; then empty border rows, then columns, become holes.
// v1: negamax alpha-beta, iterative deepening, node budget (env CG_NODES), branching 4.
// Eval = pawn difference + edge safety.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

constexpr int MAX_SIDE = 8, MAX_CELLS = MAX_SIDE * MAX_SIDE;
constexpr int EMPTY = 0, PAWN0 = 1, PAWN1 = 2, HOLE = 3;
const char *DIRECTIONS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};
constexpr int STEP_Y[4] = {-1, 0, 1, 0}, STEP_X[4] = {0, 1, 0, -1};
constexpr int WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64;
constexpr int PAWN_WEIGHT = 100;     // material dominates
constexpr int SAFETY_WEIGHT = 3;     // per step between a pawn and the nearest drop
constexpr int SAFETY_CAP = 3;        // beyond this distance a pawn is considered safe
constexpr int DEFAULT_NODES = 10000; // CodinGame budget: ~1.8 us/node local, ~40 ms on CG
constexpr int TIME_GUARD_MS = 40;    // 40 % of the limit

int H = MAX_SIDE, W = MAX_SIDE;

inline bool isPawn(int v) { return v == PAWN0 || v == PAWN1; }

struct Grid {
    int cells[MAX_CELLS];  // row major, H x W

    int at(int y, int x) const { return cells[y * W + x]; }
    void put(int y, int x, int v) { cells[y * W + x] = v; }

    // referee singleMove*: push what is in front, then move or fall.
    void stepPawn(int pawn, int y, int x, int dy, int dx) {
        int ny = y + dy, nx = x + dx;
        if (ny < 0 || ny >= H || nx < 0 || nx >= W) { put(y, x, EMPTY); return; }
        int front = at(ny, nx);
        if (isPawn(front)) {
            stepPawn(front, ny, nx, dy, dx);
            stepPawn(pawn, y, x, dy, dx);
        } else if (front == HOLE) {
            put(y, x, EMPTY);
        } else {
            put(ny, nx, pawn);
            put(y, x, EMPTY);
        }
    }

    void stepIfOwn(int pawn, int y, int x, int dir) {
        if (at(y, x) == pawn) stepPawn(pawn, y, x, STEP_Y[dir], STEP_X[dir]);
    }

    // Scan order of the referee for each direction.
    void move(int player, int dir) {
        int pawn = PAWN0 + player;
        if (dir == 0) { for (int x = 0; x < W; ++x) for (int y = 0; y < H; ++y) stepIfOwn(pawn, y, x, dir); }
        else if (dir == 2) { for (int x = 0; x < W; ++x) for (int y = H - 1; y >= 0; --y) stepIfOwn(pawn, y, x, dir); }
        else if (dir == 1) { for (int y = 0; y < H; ++y) for (int x = W - 1; x >= 0; --x) stepIfOwn(pawn, y, x, dir); }
        else { for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) stepIfOwn(pawn, y, x, dir); }
    }

    // A row (column = false) or a column: true when it still holds a pawn.
    bool lineHasPawn(int index, bool column) const {
        int n = column ? H : W;
        for (int k = 0; k < n; ++k) if (isPawn(column ? at(k, index) : at(index, k))) return true;
        return false;
    }

    void fillLine(int index, bool column) {
        int n = column ? H : W;
        for (int k = 0; k < n; ++k) { if (column) put(k, index, HOLE); else put(index, k, HOLE); }
    }

    // Pawn-free lines become holes from one side until a line with a pawn.
    void trim(bool column, bool reverse) {
        int n = column ? W : H;
        for (int i = 0; i < n; ++i) {
            int index = reverse ? n - 1 - i : i;
            if (lineHasPawn(index, column)) return;
            fillLine(index, column);
        }
    }

    void removeEmptyBorders() {
        trim(false, false);
        trim(false, true);
        trim(true, false);
        trim(true, true);
    }

    void play(int player, int dir) {
        move(player, dir);
        removeEmptyBorders();
    }

    int count(int player) const {
        int n = 0;
        for (int i = 0; i < H * W; ++i) n += cells[i] == PAWN0 + player;
        return n;
    }

    // -1 while both sides have pawns (referee checkWinner order).
    int winner() const {
        if (count(0) == 0) return 1;
        if (count(1) == 0) return 0;
        return -1;
    }

    // Steps before this pawn would fall, in its most exposed direction.
    int dropDistance(int y, int x) const {
        int best = SAFETY_CAP;
        for (int d = 0; d < 4; ++d) {
            int steps = 0, ny = y + STEP_Y[d], nx = x + STEP_X[d];
            while (steps < best && ny >= 0 && ny < H && nx >= 0 && nx < W && at(ny, nx) != HOLE) {
                ++steps; ny += STEP_Y[d]; nx += STEP_X[d];
            }
            if (steps < best) best = steps;
        }
        return best;
    }
};

int evaluate(const Grid &g, int player) {
    int score = 0;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            int v = g.at(y, x);
            if (!isPawn(v)) continue;
            int sign = v == PAWN0 + player ? 1 : -1;
            score += sign * (PAWN_WEIGHT + SAFETY_WEIGHT * g.dropDistance(y, x));
        }
    return score;
}

// ---------------------------------------------------------------- search
long long nodeLimit = DEFAULT_NODES, nodes = 0;
int lastDepth = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

bool outOfTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_GUARD_MS;
}

int negamax(const Grid &g, int player, int depth, int alpha, int beta, int ply);

int moveValue(const Grid &g, int player, int dir, int depth, int alpha, int beta, int ply) {
    Grid child = g;
    child.play(player, dir);
    int w = child.winner();
    if (w >= 0) return w == player ? WIN - ply : -(WIN - ply);
    return -negamax(child, 1 - player, depth - 1, -beta, -alpha, ply + 1);
}

int negamax(const Grid &g, int player, int depth, int alpha, int beta, int ply) {
    if (++nodes > nodeLimit) { aborted = true; return 0; }
    if ((nodes & 255) == 0 && outOfTime()) { cerr << "TIMEGUARD" << endl; aborted = true; return 0; }
    if (depth == 0) return evaluate(g, player);
    int best = -INF;
    for (int dir = 0; dir < 4; ++dir) {
        int value = moveValue(g, player, dir, depth, alpha, beta, ply);
        if (aborted) return 0;
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

int search(const Grid &g, int player) {
    nodes = 0; aborted = false; lastDepth = 0;
    int order[4] = {0, 1, 2, 3}, best = order[0];
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestValue = -INF, bestIndex = 0;
        for (int i = 0; i < 4; ++i) {
            int value = moveValue(g, player, order[i], depth, alpha, INF, 1);
            if (aborted) break;
            if (value > bestValue) { bestValue = value; bestIndex = i; }
            if (value > alpha) alpha = value;
        }
        if (aborted) break;
        best = order[bestIndex];
        lastDepth = depth;
        for (int i = bestIndex; i > 0; --i) order[i] = order[i - 1];
        order[0] = best;
        if (bestValue >= WIN - MAX_DEPTH || bestValue <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// ---------------------------------------------------------------- I/O
int cellCode(const string &tok) {
    if (tok == "-") return EMPTY;
    if (tok == "0") return PAWN0;
    if (tok == "1") return PAWN1;
    return HOLE;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long budget = DEFAULT_NODES;
    if (const char *e = getenv("CG_NODES")) budget = atoll(e);
    int me;
    if (!(cin >> me >> H >> W)) return 0;
    string line;
    getline(cin, line);
    while (true) {
        Grid g;
        for (int y = 0; y < H; ++y) {
            if (!getline(cin, line)) return 0;
            if (y == 0) t0 = chrono::steady_clock::now();
            istringstream in(line);
            string tok;
            for (int x = 0; x < W; ++x) g.put(y, x, (in >> tok) ? cellCode(tok) : HOLE);
        }
        nodeLimit = budget;
        int dir = search(g, me);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << DIRECTIONS[dir] << endl;
    }
}
