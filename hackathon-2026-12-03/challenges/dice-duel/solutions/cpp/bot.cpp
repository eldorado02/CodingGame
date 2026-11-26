// Dice Duel — C++ (submitted), v1 alpha-beta on material. Port of solutions/python/bot.py.
// A die rolls exactly `top` steps (U = y+1, D = y-1, R = x+1, L = x-1), never
// revisits a cell nor crosses a die; the last step may capture an enemy die
// (Wood 1: only when the tops sum to 7, SUM_RULE). Moves deduplicated by (die,
// end cell, end state); negamax alpha-beta, iterative deepening, node budget.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

constexpr int SIZE = 8;
constexpr bool SUM_RULE = true;  // Wood 1 rule (Wood 2: false): capture only when tops sum to 7
constexpr int CAPTURE_SUM = 7;
constexpr int DIE_VALUE = 100;
constexpr long long TURN_NODES = 4000;  // node cost varies x4 with the dice count
constexpr int TURN_MS = 30;
constexpr int NONE = -1;
struct Step { char name; int dx, dy; int roll[6]; };
constexpr Step STEPS[4] = {{'U', 0, 1, {1, 2, 3, 0, 4, 5}},
                           {'D', 0, -1, {3, 0, 1, 2, 4, 5}},
                           {'R', 1, 0, {4, 1, 5, 3, 2, 0}},
                           {'L', -1, 0, {5, 1, 4, 3, 0, 2}}};

using Faces = array<int, 6>;  // top, front, bottom, back, left, right
struct Die { int owner, x, y; Faces faces; };
struct Move { int die; string path; int x, y; Faces faces; int captured; };
using Dice = vector<Die>;

Faces rolled(const Faces &f, const Step &s) {
    Faces out;
    for (int k = 0; k < 6; ++k) out[k] = f[s.roll[k]];
    return out;
}

// Self-avoiding rolls of every die of one side (DFS order U, D, R, L).
struct Generator {
    const Dice &dice;
    int side;
    int at[SIZE][SIZE];
    bool visited[SIZE][SIZE] = {};
    vector<Move> moves;
    set<tuple<int, int, int, Faces>> seen;

    Generator(const Dice &d, int s) : dice(d), side(s) {
        for (auto &row : at)
            for (int &v : row) v = NONE;
        for (int i = 0; i < (int)dice.size(); ++i) at[dice[i].x][dice[i].y] = i;
    }

    vector<Move> run() {
        for (int i = 0; i < (int)dice.size(); ++i) {
            if (dice[i].owner != side) continue;
            visited[dice[i].x][dice[i].y] = true;
            string path;
            walk(i, dice[i].x, dice[i].y, dice[i].faces, path);
            visited[dice[i].x][dice[i].y] = false;
        }
        vector<Move> ordered;  // captures first, generation order kept
        for (const Move &m : moves)
            if (m.captured >= 0) ordered.push_back(m);
        for (const Move &m : moves)
            if (m.captured < 0) ordered.push_back(m);
        return ordered;
    }

    void walk(int i, int x, int y, const Faces &faces, string &path) {
        int top = dice[i].faces[0];
        if ((int)path.size() == top) { record(i, x, y, faces, path); return; }
        for (const Step &s : STEPS) {
            int nx = x + s.dx, ny = y + s.dy;
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE || visited[nx][ny]) continue;
            int j = at[nx][ny];
            if (j >= 0 && (dice[j].owner == side || (int)path.size() + 1 < top)) continue;  // only the last step captures
            visited[nx][ny] = true;
            path.push_back(s.name);
            walk(i, nx, ny, rolled(faces, s), path);
            path.pop_back();
            visited[nx][ny] = false;
        }
    }

    void record(int i, int x, int y, const Faces &faces, const string &path) {
        int j = at[x][y];
        if (j >= 0 && SUM_RULE && faces[0] + dice[j].faces[0] != CAPTURE_SUM) return;
        if (!seen.insert({i, x, y, faces}).second) return;
        moves.push_back({i, path, x, y, faces, j});
    }
};

vector<Move> genMoves(const Dice &dice, int side) { return Generator(dice, side).run(); }

Dice applyMove(const Dice &dice, const Move &m) {
    Dice out = dice;
    out[m.die].x = m.x;
    out[m.die].y = m.y;
    out[m.die].faces = m.faces;
    if (m.captured >= 0) out.erase(out.begin() + m.captured);
    return out;
}

int evaluate(const Dice &dice, int side) {
    int mine = 0;
    for (const Die &d : dice) mine += d.owner == side;
    return DIE_VALUE * (mine - ((int)dice.size() - mine));
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64;
long long nodeLimit = TURN_NODES, nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

long long elapsedMs() { return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count(); }

int negamax(const Dice &dice, int side, int depth, int alpha, int beta, int ply) {
    if (++nodes > nodeLimit) { aborted = true; return 0; }
    // checked at every node: one move generation can cost 0.2 ms
    if (elapsedMs() > TURN_MS) { cerr << "TIMEGUARD" << endl; aborted = true; return 0; }
    if (depth == 0) return evaluate(dice, side);
    vector<Move> moves = genMoves(dice, side);
    if (moves.empty()) return -(WIN - ply);
    int best = -INF;
    for (const Move &m : moves) {
        int v = -negamax(applyMove(dice, m), 1 - side, depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

Move search(const Dice &dice, vector<Move> moves) {
    Move best = moves[0];
    nodes = 0;
    aborted = false;
    lastDepth = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, cur = 0;
        for (int k = 0; k < (int)moves.size(); ++k) {
            int v = -negamax(applyMove(dice, moves[k]), 1, depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = k; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = moves[cur];
        lastDepth = depth;
        for (int k = cur; k > 0; --k) moves[k] = moves[k - 1];  // PV first
        moves[0] = best;
        if (abs(bestV) >= WIN - MAX_DEPTH) break;
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    int count;
    while (cin >> count) {
        t0 = chrono::steady_clock::now();
        Dice dice(count);
        for (Die &d : dice) {
            string cell;
            cin >> d.owner >> cell;
            for (int &f : d.faces) cin >> f;
            d.x = cell[0] - 'A';
            d.y = atoi(cell.c_str() + 1) - 1;
        }
        if (!cin) return 0;
        vector<Move> moves = genMoves(dice, 0);
        if (moves.empty()) { cout << "A1 U" << endl; continue; }
        nodeLimit = cliNodes > 0 ? cliNodes : TURN_NODES;
        Move m = search(dice, moves);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms=" << elapsedMs() << endl;
        const Die &d = dice[m.die];
        cout << char('A' + d.x) << d.y + 1 << ' ' << m.path << endl;
    }
}
