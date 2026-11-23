// Domain Expansion 7x7 (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Rules from the referee (Mortis66666/CG-Domain-Expansion, Board.kt): move up to Manhattan 3
// through a BFS path whose nodes are expanded up to depth 3 (paths of length 4 allowed), then
// wall one free side of the token's cell; tokens separated by walls = game over, domains compared.
// v1: negamax alpha-beta over actions, iterative deepening, node budget (env CG_NODES);
// eval = Voronoi territory difference.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

constexpr int SIZE = 7, CELLS = SIZE * SIZE;
constexpr int MAX_MOVE = 3;              // Manhattan reach of a move
constexpr int PATH_EXPANSION_DEPTH = 3;  // referee BFS expands nodes up to this depth
constexpr char LETTERS[4] = {'U', 'R', 'D', 'L'};
constexpr int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
constexpr int OPPOSITE[4] = {2, 3, 0, 1};
constexpr int WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64;
constexpr int DEFAULT_NODES = 10000;     // CodinGame budget (100 ms per turn): ~15 ms local
constexpr int FIRST_TURN_NODES = 60000;  // first turn has 1000 ms
constexpr int TIME_GUARD_MS = 40, FIRST_TURN_GUARD_MS = 400;  // 40 % of the limits
constexpr int MAX_ACTIONS = CELLS * 4;
constexpr int NOT_FINAL = INF;           // final_score sentinel: tokens still connected

int NEIGHBOUR[CELLS][4];

void buildNeighbours() {
    for (int cell = 0; cell < CELLS; ++cell)
        for (int d = 0; d < 4; ++d) {
            int nx = cell % SIZE + DX[d], ny = cell / SIZE + DY[d];
            NEIGHBOUR[cell][d] = (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) ? ny * SIZE + nx : -1;
        }
}

int manhattan(int a, int b) { return abs(a % SIZE - b % SIZE) + abs(a / SIZE - b / SIZE); }

struct Action { int cell, dir; };

struct State {
    int walls[CELLS];  // bitmask over U R D L of sides already walled
    int tokens[2];     // 0 = us, 1 = them

    void initBorders() {
        for (int c = 0; c < CELLS; ++c) {
            walls[c] = 0;
            for (int d = 0; d < 4; ++d) if (NEIGHBOUR[c][d] < 0) walls[c] |= 1 << d;
        }
    }

    void apply(int player, int cell, int d) {
        tokens[player] = cell;
        walls[cell] |= 1 << d;
        walls[NEIGHBOUR[cell][d]] |= 1 << OPPOSITE[d];
    }
};

// Cells the token of `player` may move to, ascending (referee findPath).
int destinations(const State &s, int player, int *out) {
    int start = s.tokens[player], blocker = s.tokens[1 - player];
    int depth[CELLS], queue[CELLS], head = 0, tail = 0;
    for (int c = 0; c < CELLS; ++c) depth[c] = -1;
    depth[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int cell = queue[head++];
        if (depth[cell] > PATH_EXPANSION_DEPTH) continue;
        for (int d = 0; d < 4; ++d) {
            int next = NEIGHBOUR[cell][d];
            if (((s.walls[cell] >> d) & 1) || next == blocker || depth[next] >= 0) continue;
            depth[next] = depth[cell] + 1;
            queue[tail++] = next;
        }
    }
    int n = 0;
    for (int c = 0; c < CELLS; ++c)
        if (depth[c] >= 0 && manhattan(start, c) <= MAX_MOVE) out[n++] = c;
    return n;
}

int generateActions(const State &s, int player, Action *out) {
    int cells[CELLS], nc = destinations(s, player, cells), n = 0;
    for (int i = 0; i < nc; ++i)
        for (int d = 0; d < 4; ++d)
            if (!((s.walls[cells[i]] >> d) & 1)) out[n++] = {cells[i], d};
    return n;
}

// BFS steps through walls only (tokens do not block territory); -1 = unreachable.
void distances(const State &s, int start, int *dist) {
    int queue[CELLS], head = 0, tail = 0;
    for (int c = 0; c < CELLS; ++c) dist[c] = -1;
    dist[start] = 0;
    queue[tail++] = start;
    while (head < tail) {
        int cell = queue[head++];
        for (int d = 0; d < 4; ++d) {
            if ((s.walls[cell] >> d) & 1) continue;
            int next = NEIGHBOUR[cell][d];
            if (dist[next] < 0) { dist[next] = dist[cell] + 1; queue[tail++] = next; }
        }
    }
}

int reachableCount(const int *dist) {
    int n = 0;
    for (int c = 0; c < CELLS; ++c) n += dist[c] >= 0;
    return n;
}

// NOT_FINAL while the tokens are connected, else the outcome for `player`.
int finalScore(const State &s, int player) {
    int mine[CELLS], theirs[CELLS];
    distances(s, s.tokens[player], mine);
    if (mine[s.tokens[1 - player]] >= 0) return NOT_FINAL;
    distances(s, s.tokens[1 - player], theirs);
    int diff = reachableCount(mine) - reachableCount(theirs);
    if (diff > 0) return WIN + diff;
    if (diff < 0) return -WIN + diff;
    return 0;
}

// Voronoi territory of `player` minus the opponent's.
int evaluate(const State &s, int player) {
    int mine[CELLS], theirs[CELLS], score = 0;
    distances(s, s.tokens[player], mine);
    distances(s, s.tokens[1 - player], theirs);
    for (int c = 0; c < CELLS; ++c) {
        int a = mine[c], b = theirs[c];
        if (a < 0 && b < 0) continue;
        if (b < 0 || (a >= 0 && a < b)) ++score;
        else if (a < 0 || b < a) --score;
    }
    return score;
}

// ---------------------------------------------------------------- search
long long nodeLimit = DEFAULT_NODES, nodes = 0;
int timeGuardMs = TIME_GUARD_MS, lastDepth = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

bool outOfTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > timeGuardMs;
}

int negamax(const State &s, int player, int depth, int alpha, int beta);

// Value of `a` for `player`, searching depth - 1 below it.
int actionValue(const State &s, int player, const Action &a, int depth, int alpha, int beta) {
    State child = s;
    child.apply(player, a.cell, a.dir);
    int final = finalScore(child, player);
    if (final != NOT_FINAL) return final;
    return -negamax(child, 1 - player, depth - 1, -beta, -alpha);
}

int negamax(const State &s, int player, int depth, int alpha, int beta) {
    if (++nodes > nodeLimit) { aborted = true; return 0; }
    if ((nodes & 63) == 0 && outOfTime()) { cerr << "TIMEGUARD" << endl; aborted = true; return 0; }
    if (depth == 0) return evaluate(s, player);
    Action acts[MAX_ACTIONS];
    int n = generateActions(s, player, acts), best = -INF;
    for (int i = 0; i < n; ++i) {
        int value = actionValue(s, player, acts[i], depth, alpha, beta);
        if (aborted) return 0;
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

// Root actions sorted by 1-ply value (stable insertion sort).
int orderedRoot(const State &s, int player, Action *acts) {
    int n = generateActions(s, player, acts), keys[MAX_ACTIONS];
    for (int i = 0; i < n; ++i) keys[i] = actionValue(s, player, acts[i], 1, -INF, INF);
    for (int i = 1; i < n; ++i) {
        Action item = acts[i];
        int key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { acts[j + 1] = acts[j]; keys[j + 1] = keys[j]; --j; }
        acts[j + 1] = item; keys[j + 1] = key;
    }
    return n;
}

Action search(const State &s, int player) {
    nodes = 0; aborted = false; lastDepth = 0;
    Action acts[MAX_ACTIONS];
    int n = orderedRoot(s, player, acts);
    Action best = acts[0];
    for (int depth = 2; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestValue = -INF, bestIndex = 0;
        for (int i = 0; i < n; ++i) {
            int value = actionValue(s, player, acts[i], depth, alpha, INF);
            if (aborted) break;
            if (value > bestValue) { bestValue = value; bestIndex = i; }
            if (value > alpha) alpha = value;
        }
        if (aborted) break;
        best = acts[bestIndex];
        lastDepth = depth;
        for (int i = bestIndex; i > 0; --i) acts[i] = acts[i - 1];
        acts[0] = best;
        if (bestValue >= WIN || bestValue <= -WIN) break;
    }
    return best;
}

// ---------------------------------------------------------------- I/O
int letterIndex(const string &t) {
    for (int d = 0; d < 4; ++d) if (t.size() == 1 && t[0] == LETTERS[d]) return d;
    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    buildNeighbours();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int mx, my, ox, oy;
    cin >> mx >> my >> ox >> oy;
    getline(cin, line);
    State s;
    s.initBorders();
    s.tokens[0] = my * SIZE + mx;
    s.tokens[1] = oy * SIZE + ox;
    bool firstTurn = true;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        istringstream in(line);
        int ax = -1, ay = -1;
        string dir;
        in >> ax >> ay >> dir;
        int d = letterIndex(dir);
        if (d >= 0) s.apply(1, ay * SIZE + ax, d);
        nodeLimit = cliNodes > 0 ? cliNodes : (firstTurn ? FIRST_TURN_NODES : DEFAULT_NODES);
        timeGuardMs = firstTurn ? FIRST_TURN_GUARD_MS : TIME_GUARD_MS;
        Action a = search(s, 0);
        s.apply(0, a.cell, a.dir);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << a.cell % SIZE << " " << a.cell / SIZE << " " << LETTERS[a.dir] << endl;
        firstTurn = false;
    }
    return 0;
}
