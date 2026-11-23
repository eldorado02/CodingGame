// Paper Soccer 8x10 (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Geometry from the referee (jdermont/CodinGame-paper-soccer, Pitch.java): points
// x 0..8, y 0..10, goal points x 3..5 at y -1 (player 0 scores) and y 11 (player 1).
// v1: negamax alpha-beta over complete turns, iterative deepening, node budget
// (env CG_NODES). A turn = bounce sequence (DFS, first per end point+outcome).
// Eval = rows to goal + unused-edge BFS goal distance difference + centring.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

constexpr int WIDTH = 8, HEIGHT = 10, COLS = WIDTH + 1, ROWS = HEIGHT + 1;
constexpr int FIELD_POINTS = COLS * ROWS;
constexpr int TOP_GOAL = FIELD_POINTS, BOTTOM_GOAL = FIELD_POINTS + 3, POINTS = FIELD_POINTS + 6;
constexpr int GOAL_X0 = 3, GOAL_WIDTH = 3;
constexpr int CENTER_X = WIDTH / 2;
constexpr int START_BALL = (HEIGHT / 2) * COLS + CENTER_X;
constexpr int DX[8] = {0, 1, 1, 1, 0, -1, -1, -1}, DY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
constexpr int PLAYER_TOP = 0, PLAYER_BOTTOM = 1;
constexpr int NORMAL = 0, WON = 1, LOST = 2, OUTCOMES = 3;

constexpr int WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64;
constexpr int ROW_WEIGHT = 80;             // one row of vertical progress
constexpr int GOAL_DISTANCE_WEIGHT = 120;  // unused-edge path to goal (Lecorbio evaluator)
constexpr int NO_PATH = 40;                // goal distance when every route is drawn
constexpr int CENTRE_WEIGHT = 15;          // per column away from the goal mouth
constexpr int CENTRE_ZONE_ROWS = 3;        // centring matters this close to a goal
constexpr int MAX_DFS_STEPS = 20000;       // bounce-sequence enumeration cap per node
constexpr int DEFAULT_NODES = 3000;        // CodinGame budget (200 ms per turn)
constexpr int FIRST_TURN_NODES = 15000;    // first turn has 1000 ms
constexpr int TIME_GUARD_MS = 80, FIRST_TURN_GUARD_MS = 400;  // 40 % of 200 / 1000 ms: late nodes are slow
constexpr int MAX_TURN_LEN = 448;          // more than the number of edges
constexpr int MAX_TURNS = POINTS * OUTCOMES;
constexpr int MAX_PLY = MAX_DEPTH + 2;

// ---------------------------------------------------------------- geometry
int NEIGHBOUR[POINTS][8], DEGREE[POINTS];

int pointIndex(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) return y * COLS + x;
    bool inMouth = x >= GOAL_X0 && x < GOAL_X0 + GOAL_WIDTH;
    if (inMouth && y == -1) return TOP_GOAL + x - GOAL_X0;
    if (inMouth && y == ROWS) return BOTTOM_GOAL + x - GOAL_X0;
    return -1;
}

void pointXY(int p, int &x, int &y) {
    if (p < FIELD_POINTS) { x = p % COLS; y = p / COLS; }
    else if (p < BOTTOM_GOAL) { x = GOAL_X0 + p - TOP_GOAL; y = -1; }
    else { x = GOAL_X0 + p - BOTTOM_GOAL; y = ROWS; }
}

bool isRemovedPair(int p, int q) {
    // goal posts: the ball cannot enter the goal diagonally from outside the mouth
    static const int PAIRS[4][4] = {{2, 0, 3, -1}, {6, 0, 5, -1}, {2, 10, 3, 11}, {6, 10, 5, 11}};
    for (const auto &r : PAIRS) {
        int a = pointIndex(r[0], r[1]), b = pointIndex(r[2], r[3]);
        if ((p == a && q == b) || (p == b && q == a)) return true;
    }
    return false;
}

void buildNeighbours() {
    for (int p = 0; p < POINTS; ++p) {
        int x, y;
        pointXY(p, x, y);
        DEGREE[p] = 0;
        for (int d = 0; d < 8; ++d) {
            int q = pointIndex(x + DX[d], y + DY[d]);
            NEIGHBOUR[p][d] = (q < 0 || isRemovedPair(p, q)) ? -1 : q;
            if (NEIGHBOUR[p][d] >= 0) ++DEGREE[p];
        }
    }
}

int directionBetween(int p, int q) {
    for (int d = 0; d < 8; ++d) if (NEIGHBOUR[p][d] == q) return d;
    return -1;
}

int goalOwner(int p) {
    if (p >= TOP_GOAL && p < BOTTOM_GOAL) return PLAYER_TOP;
    if (p >= BOTTOM_GOAL) return PLAYER_BOTTOM;
    return -1;
}

// ---------------------------------------------------------------- pitch
struct Pitch {
    int drawn[POINTS];  // bitmask of directions already drawn from each point
    int freeEdges[POINTS];
    int ball = START_BALL;

    int draw(int p, int d) {
        int q = NEIGHBOUR[p][d];
        drawn[p] |= 1 << d;
        drawn[q] |= 1 << ((d + 4) % 8);
        --freeEdges[p];
        --freeEdges[q];
        return q;
    }

    void undraw(int p, int d) {
        int q = NEIGHBOUR[p][d];
        drawn[p] &= ~(1 << d);
        drawn[q] &= ~(1 << ((d + 4) % 8));
        ++freeEdges[p];
        ++freeEdges[q];
    }

    bool canMove(int d) const { return NEIGHBOUR[ball][d] >= 0 && !((drawn[ball] >> d) & 1); }

    void drawPair(int x1, int y1, int x2, int y2) {
        int a = pointIndex(x1, y1), b = pointIndex(x2, y2);
        draw(a, directionBetween(a, b));
    }

    // Border lines and goal frames, as Pitch() draws them.
    void reset() {
        memset(drawn, 0, sizeof drawn);
        for (int p = 0; p < POINTS; ++p) freeEdges[p] = DEGREE[p];
        ball = START_BALL;
        for (int y = 0; y < HEIGHT; ++y) { drawPair(0, y, 0, y + 1); drawPair(WIDTH, y, WIDTH, y + 1); }
        for (int x = 0; x < WIDTH; ++x)
            if (x < GOAL_X0 || x >= GOAL_X0 + GOAL_WIDTH - 1) { drawPair(x, 0, x + 1, 0); drawPair(x, HEIGHT, x + 1, HEIGHT); }
        const int frames[2][2] = {{-1, 0}, {ROWS, HEIGHT}};
        for (const auto &f : frames) {
            drawPair(3, f[0], 4, f[0]);
            drawPair(4, f[0], 5, f[0]);
            drawPair(3, f[0], 3, f[1]);
            drawPair(5, f[0], 5, f[1]);
        }
    }

    void apply(const char *digits, int len) {
        for (int i = 0; i < len; ++i) {
            int d = digits[i] - '0';
            if (d >= 0 && d < 8 && canMove(d)) ball = draw(ball, d);
        }
    }
};

// ---------------------------------------------------------------- turn generation
struct Turn { char digits[MAX_TURN_LEN]; int len, end, outcome; };

struct TurnGenerator {
    Pitch *pitch;
    int mover, steps, count, pathLen;
    char path[MAX_TURN_LEN];
    bool seen[POINTS][OUTCOMES];
    Turn *out;

    void emit(int q, int outcome) {
        if (seen[q][outcome]) return;
        seen[q][outcome] = true;
        Turn &t = out[count++];
        memcpy(t.digits, path, pathLen);
        t.len = pathLen; t.end = q; t.outcome = outcome;
    }

    void dfs() {
        int p = pitch->ball;
        for (int d = 0; d < 8; ++d) {
            if (steps >= MAX_DFS_STEPS) return;
            if (!pitch->canMove(d)) continue;
            ++steps;
            int q = pitch->draw(p, d);
            pitch->ball = q;
            path[pathLen++] = char('0' + d);
            int owner = goalOwner(q);
            bool bounce = DEGREE[q] - pitch->freeEdges[q] > 1;
            if (owner >= 0) emit(q, owner == mover ? WON : LOST);
            else if (pitch->freeEdges[q] == 0) emit(q, LOST);
            else if (bounce) dfs();
            else emit(q, NORMAL);
            --pathLen;
            pitch->ball = p;
            pitch->undraw(p, d);
        }
    }
};

// Complete turns from pitch.ball, first sequence per (end point, outcome).
int generateTurns(Pitch &pitch, int mover, Turn *out) {
    static TurnGenerator g;
    g.pitch = &pitch; g.mover = mover; g.steps = 0; g.count = 0; g.pathLen = 0; g.out = out;
    memset(g.seen, 0, sizeof g.seen);
    g.dfs();
    return g.count;
}

// ---------------------------------------------------------------- evaluation
int rowsToGoal(int player, int p) {
    int x, y;
    pointXY(p, x, y);
    return player == PLAYER_TOP ? y + 1 : ROWS - y;
}

// Edges the ball needs, over unused edges only, to reach the goal `player` attacks.
int goalDistance(const Pitch &pitch, int player) {
    int targetLo = player == PLAYER_TOP ? TOP_GOAL : BOTTOM_GOAL;
    int dist[POINTS], queue[POINTS], head = 0, tail = 0;
    for (int p = 0; p < POINTS; ++p) dist[p] = NO_PATH;
    dist[pitch.ball] = 0;
    queue[tail++] = pitch.ball;
    while (head < tail) {
        int p = queue[head++];
        if (p >= targetLo && p < targetLo + GOAL_WIDTH) return dist[p];
        if (goalOwner(p) >= 0) continue;  // the ball stops in any goal
        for (int d = 0; d < 8; ++d) {
            int q = NEIGHBOUR[p][d];
            if (q >= 0 && !((pitch.drawn[p] >> d) & 1) && dist[q] == NO_PATH) {
                dist[q] = dist[p] + 1;
                queue[tail++] = q;
            }
        }
    }
    return NO_PATH;
}

// Score for `player`, who is about to move with the ball at pitch.ball.
int evaluate(const Pitch &pitch, int player) {
    int x, y;
    pointXY(pitch.ball, x, y);
    int mine = rowsToGoal(player, pitch.ball), theirs = rowsToGoal(1 - player, pitch.ball);
    int score = ROW_WEIGHT * (theirs - mine);
    score += GOAL_DISTANCE_WEIGHT * (goalDistance(pitch, 1 - player) - goalDistance(pitch, player));
    int offCentre = abs(x - CENTER_X);
    if (mine <= CENTRE_ZONE_ROWS) score -= CENTRE_WEIGHT * offCentre;
    if (theirs <= CENTRE_ZONE_ROWS) score += CENTRE_WEIGHT * offCentre;
    return score;
}

int orderKey(const Turn &t, int mover) {
    if (t.outcome == WON) return WIN;
    if (t.outcome == LOST) return -WIN;
    return -rowsToGoal(mover, t.end);
}

// Stable insertion sort, best first; sorts indexes to avoid moving big structs.
void sortTurns(const Turn *turns, int *order, int n, int mover) {
    int keys[MAX_TURNS];
    for (int i = 0; i < n; ++i) { order[i] = i; keys[i] = orderKey(turns[i], mover); }
    for (int i = 1; i < n; ++i) {
        int item = order[i], key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { order[j + 1] = order[j]; keys[j + 1] = keys[j]; --j; }
        order[j + 1] = item; keys[j + 1] = key;
    }
}

// ---------------------------------------------------------------- search
static Turn TURNS[MAX_PLY][MAX_TURNS];
long long nodeLimit = DEFAULT_NODES, nodes = 0;
int timeGuardMs = TIME_GUARD_MS, lastDepth = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

bool outOfTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > timeGuardMs;
}

Pitch childOf(const Pitch &pitch, const Turn &t) {
    Pitch next = pitch;
    next.apply(t.digits, t.len);
    return next;
}

int negamax(Pitch &pitch, int player, int depth, int alpha, int beta, int ply) {
    if (++nodes > nodeLimit) { aborted = true; return 0; }
    if (outOfTime()) { cerr << "TIMEGUARD" << endl; aborted = true; return 0; }
    if (depth == 0) return evaluate(pitch, player);
    Turn *turns = TURNS[ply];
    int n = generateTurns(pitch, player, turns);
    if (n == 0) return -(WIN - ply);
    int order[MAX_TURNS];
    sortTurns(turns, order, n, player);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        const Turn &t = turns[order[i]];
        if (t.outcome == WON) return WIN - ply;
        int value;
        if (t.outcome == LOST) value = -(WIN - ply);
        else {
            Pitch next = childOf(pitch, t);
            value = -negamax(next, 1 - player, depth - 1, -beta, -alpha, ply + 1);
            if (aborted) return 0;
        }
        if (value > best) best = value;
        if (value > alpha) alpha = value;
        if (alpha >= beta) break;
    }
    return best;
}

string searchTurn(Pitch &pitch, int player) {
    nodes = 0; aborted = false; lastDepth = 0;
    Turn *turns = TURNS[0];
    int n = generateTurns(pitch, player, turns);
    int order[MAX_TURNS];
    sortTurns(turns, order, n, player);
    int best = order[0];
    if (turns[best].outcome != WON) {
        for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
            int alpha = -INF, bestValue = -INF, bestIndex = 0;
            for (int i = 0; i < n; ++i) {
                const Turn &t = turns[order[i]];
                int value;
                if (t.outcome == LOST) value = -WIN;
                else {
                    Pitch next = childOf(pitch, t);
                    value = -negamax(next, 1 - player, depth - 1, -INF, -alpha, 1);
                }
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
    }
    return string(turns[best].digits, turns[best].len);
}

// ---------------------------------------------------------------- I/O
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    buildNeighbours();
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int me = atoi(line.c_str());
    static Pitch pitch;
    pitch.reset();
    bool firstTurn = true;
    while (getline(cin, line)) {
        string opponentMove;
        if (!getline(cin, opponentMove)) break;
        t0 = chrono::steady_clock::now();
        while (!opponentMove.empty() && (opponentMove.back() == '\r' || opponentMove.back() == ' ')) opponentMove.pop_back();
        if (opponentMove != "-") pitch.apply(opponentMove.c_str(), (int)opponentMove.size());
        nodeLimit = cliNodes > 0 ? cliNodes : (firstTurn ? FIRST_TURN_NODES : DEFAULT_NODES);
        timeGuardMs = firstTurn ? FIRST_TURN_GUARD_MS : TIME_GUARD_MS;
        string digits = searchTurn(pitch, me);
        pitch.apply(digits.c_str(), (int)digits.size());
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << digits << endl;
        firstTurn = false;
    }
    return 0;
}
