// Oware Abapa — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Rules from github.com/MaximeCheramy/cg-oware (see the Python header).
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

constexpr int HOUSES = 12, SIDE = 6, WIN_SCORE = 24, MAX_TURNS = 200, TOTAL_SEEDS = 48;
constexpr int SCORE_WEIGHT = 4;  // captured seed vs seed still on our side
constexpr long long FIRST_TURN_NODES = 600000, TURN_NODES = 60000;
constexpr int FIRST_TURN_MS = 700, TURN_MS = 35;

struct State {
    array<int, HOUSES> h;  // first 6 houses = side to move
    int mine, theirs, turn;
};

int legalMoves(const State &s, int *out) {
    bool opponentHasSeeds = false;
    for (int i = SIDE; i < HOUSES; ++i) if (s.h[i]) { opponentHasSeeds = true; break; }
    int n = 0;
    for (int i = 0; i < SIDE; ++i)
        if (s.h[i] > 0 && (opponentHasSeeds || s.h[i] >= SIDE - i)) out[n++] = i;
    return n;
}

// Next state from the opponent's point of view.
State applyMove(const State &s, int i) {
    array<int, HOUSES> h = s.h;
    int seeds = h[i], pos = i;
    h[i] = 0;
    while (seeds) {
        pos = (pos + 1) % HOUSES;
        if (pos == i) continue;
        ++h[pos];
        --seeds;
    }
    bool captured[HOUSES] = {false};
    for (int cur = pos; cur >= SIDE && (h[cur] == 2 || h[cur] == 3); --cur) captured[cur] = true;
    bool grandSlam = true;
    for (int j = SIDE; j < HOUSES; ++j) if (h[j] && !captured[j]) { grandSlam = false; break; }
    int gain = 0;
    if (!grandSlam)
        for (int j = SIDE; j < HOUSES; ++j) if (captured[j]) { gain += h[j]; h[j] = 0; }
    State t;
    for (int j = 0; j < HOUSES; ++j) t.h[j] = h[(j + SIDE) % HOUSES];
    t.mine = s.theirs;
    t.theirs = s.mine + gain;
    t.turn = s.turn + 1;
    return t;
}

int sideSeeds(const State &s, int from) {
    int n = 0;
    for (int j = from; j < from + SIDE; ++j) n += s.h[j];
    return n;
}

int evaluate(const State &s) { return SCORE_WEIGHT * (s.mine - s.theirs) + sideSeeds(s, 0) - sideSeeds(s, SIDE); }

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

inline int finalScore(int mine, int theirs, int ply) {
    return mine > theirs ? WIN - ply : mine < theirs ? -(WIN - ply) : 0;
}

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    if (s.theirs > WIN_SCORE) return -(WIN - ply);
    if (s.mine > WIN_SCORE) return WIN - ply;
    int mv[SIDE];
    int n = legalMoves(s, mv);
    if (!n) return finalScore(s.mine + sideSeeds(s, 0) + sideSeeds(s, SIDE), s.theirs, ply);
    if (s.turn >= MAX_TURNS) return finalScore(s.mine, s.theirs, ply);
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

int search(const State &root) {
    int mv[SIDE];
    int n = legalMoves(root, mv), best = mv[0];
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

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    int meId = -1, myScore = 0, theirScore = 0, seedsAfterMyMove = TOTAL_SEEDS, myTurns = 0;
    while (true) {
        State s;
        for (int j = 0; j < HOUSES; ++j) if (!(cin >> s.h[j])) return 0;
        t0 = chrono::steady_clock::now();
        if (meId < 0) {
            meId = 0;
            for (int j = 0; j < HOUSES; ++j) if (s.h[j] != 4) { meId = 1; break; }
        }
        // the opponent's captures are the seeds that vanished since our move
        theirScore += seedsAfterMyMove - (sideSeeds(s, 0) + sideSeeds(s, SIDE));
        s.mine = myScore;
        s.theirs = theirScore;
        s.turn = 2 * myTurns + meId;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (myTurns == 0 ? FIRST_TURN_NODES : TURN_NODES);
        TIME_MS = myTurns == 0 ? FIRST_TURN_MS : TURN_MS;
        int tmp[SIDE];
        if (!legalMoves(s, tmp)) { cout << meId * SIDE << endl; continue; }
        int m = search(s);
        State next = applyMove(s, m);
        myScore = next.theirs;
        seedsAfterMyMove = sideSeeds(next, 0) + sideSeeds(next, SIDE);
        ++myTurns;
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << m + meId * SIDE << endl;
    }
}
