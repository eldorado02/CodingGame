// TwixT-PP 12x12 (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// "D6" = column D (x = 3), row 6 (y = 5); cell = y * 12 + x.
// First player links top/bottom (never columns A/L), second links left/right.
// v1: eval = opponent knight-path distance - ours (0-1 BFS, segments crossing
// an opponent link are cut); 1-ply ranking then 2-ply on the TOP_K best.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

constexpr int SIZE = 12, CELLS = SIZE * SIZE;
constexpr int EMPTY = 0, ME = 1, OPP = 2;
constexpr int TOP_BOTTOM = 0, LEFT_RIGHT = 1;
constexpr int KNIGHT_DX[8] = {1, 2, 2, 1, -1, -2, -2, -1};
constexpr int KNIGHT_DY[8] = {2, 1, -1, -2, -2, -1, 1, 2};
constexpr int MAX_SEGMENTS = 440, MAX_CROSSES = 16;
constexpr int UNREACHABLE = 99;
constexpr int WIN_SCORE = 1000000;
constexpr int DISTANCE_WEIGHT = 100;  // one peg of path length outweighs every link count
constexpr int LINK_WEIGHT = 3;        // more links = more alternative routes
constexpr int DEFAULT_TOP_K = 16;     // 2-ply candidates (300 ms per turn on CodinGame)
constexpr int SWAP_LOW = 3, SWAP_HIGH = 8;      // swap an opening peg inside this square
constexpr int OPENING_CELL = 3 * SIZE + 5;      // "F4"

// ---------------------------------------------------------------- geometry
struct Segment { int a, b; };
Segment SEGMENTS[MAX_SEGMENTS];
int SEGMENT_COUNT = 0;
int SEG_ID[CELLS][CELLS];
int NEIGHBOUR[CELLS][8], NEIGHBOUR_SEG[CELLS][8], NEIGHBOUR_COUNT[CELLS];
int CROSSES[MAX_SEGMENTS][MAX_CROSSES], CROSS_COUNT[MAX_SEGMENTS];

inline int cellOf(int x, int y) { return y * SIZE + x; }
inline int xOf(int cell) { return cell % SIZE; }
inline int yOf(int cell) { return cell / SIZE; }

string nameOf(int cell) { return string(1, char('A' + xOf(cell))) + to_string(yOf(cell) + 1); }
int parseCell(const string &t) { return cellOf(t[0] - 'A', atoi(t.c_str() + 1) - 1); }

int orientation(int a, int b, int c) {
    long v = (long)(xOf(b) - xOf(a)) * (yOf(c) - yOf(a)) - (long)(yOf(b) - yOf(a)) * (xOf(c) - xOf(a));
    return (v > 0) - (v < 0);
}

bool boundingBoxesOverlap(const Segment &s, const Segment &t) {
    return min(xOf(s.a), xOf(s.b)) <= max(xOf(t.a), xOf(t.b)) && min(xOf(t.a), xOf(t.b)) <= max(xOf(s.a), xOf(s.b))
        && min(yOf(s.a), yOf(s.b)) <= max(yOf(t.a), yOf(t.b)) && min(yOf(t.a), yOf(t.b)) <= max(yOf(s.a), yOf(s.b));
}

// Proper crossing of two segments that share no endpoint.
bool segmentsCross(const Segment &s, const Segment &t) {
    if (s.a == t.a || s.a == t.b || s.b == t.a || s.b == t.b) return false;
    return orientation(s.a, s.b, t.a) * orientation(s.a, s.b, t.b) < 0
        && orientation(t.a, t.b, s.a) * orientation(t.a, t.b, s.b) < 0;
}

void buildGeometry() {
    memset(SEG_ID, -1, sizeof SEG_ID);
    for (int cell = 0; cell < CELLS; ++cell) {
        for (int k = 0; k < 8; ++k) {
            int nx = xOf(cell) + KNIGHT_DX[k], ny = yOf(cell) + KNIGHT_DY[k];
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
            int other = cellOf(nx, ny), lo = min(cell, other), hi = max(cell, other);
            if (SEG_ID[lo][hi] < 0) {
                SEG_ID[lo][hi] = SEG_ID[hi][lo] = SEGMENT_COUNT;
                SEGMENTS[SEGMENT_COUNT++] = {lo, hi};
            }
            int &n = NEIGHBOUR_COUNT[cell];
            NEIGHBOUR[cell][n] = other;
            NEIGHBOUR_SEG[cell][n] = SEG_ID[lo][hi];
            ++n;
        }
    }
    for (int i = 0; i < SEGMENT_COUNT; ++i)
        for (int j = i + 1; j < SEGMENT_COUNT; ++j)
            if (boundingBoxesOverlap(SEGMENTS[i], SEGMENTS[j]) && segmentsCross(SEGMENTS[i], SEGMENTS[j])) {
                CROSSES[i][CROSS_COUNT[i]++] = j;
                CROSSES[j][CROSS_COUNT[j]++] = i;
            }
}

// ---------------------------------------------------------------- state
// board[cell] in {EMPTY, ME, OPP}; linked[p][seg] = p owns that link;
// cut[p][seg] = number of opponent links crossing seg (unusable when > 0).
struct State {
    int side[3];
    int8_t board[CELLS];
    int8_t linked[3][MAX_SEGMENTS];
    int16_t cut[3][MAX_SEGMENTS];
    int linkCount[3];

    explicit State(int mySide = TOP_BOTTOM) {
        side[ME] = mySide; side[OPP] = 1 - mySide;
        memset(board, 0, sizeof board);
        memset(linked, 0, sizeof linked);
        memset(cut, 0, sizeof cut);
        linkCount[ME] = linkCount[OPP] = 0;
    }

    void addLink(int player, int seg) {
        if (linked[player][seg]) return;
        linked[player][seg] = 1;
        ++linkCount[player];
        int other = player == ME ? OPP : ME;
        for (int i = 0; i < CROSS_COUNT[seg]; ++i) ++cut[other][CROSSES[seg][i]];
    }

    // Drop a peg and create every legal link, as the referee does.
    void place(int player, int cell) {
        board[cell] = (int8_t)player;
        for (int i = 0; i < NEIGHBOUR_COUNT[cell]; ++i) {
            int seg = NEIGHBOUR_SEG[cell][i];
            if (board[NEIGHBOUR[cell][i]] == player && cut[player][seg] == 0) addLink(player, seg);
        }
    }

    bool canPlay(int player, int cell) const {
        if (board[cell] != EMPTY) return false;
        int x = xOf(cell), y = yOf(cell);
        bool xEdge = x == 0 || x == SIZE - 1, yEdge = y == 0 || y == SIZE - 1;
        if (xEdge && yEdge) return false;  // corners are not holes
        return side[player] == TOP_BOTTOM ? !xEdge : !yEdge;
    }

    int legalMoves(int player, int *out) const {
        int n = 0;
        for (int c = 0; c < CELLS; ++c) if (canPlay(player, c)) out[n++] = c;
        return n;
    }
};

// ---------------------------------------------------------------- evaluation
inline bool onStartBorder(int side, int cell) { return side == TOP_BOTTOM ? yOf(cell) == 0 : xOf(cell) == 0; }
inline bool onGoalBorder(int side, int cell) { return side == TOP_BOTTOM ? yOf(cell) == SIZE - 1 : xOf(cell) == SIZE - 1; }

// Fewest pegs `player` still needs to connect his borders (0-1 BFS, ring deque).
int pathDistance(const State &s, int player) {
    constexpr int RING = 4 * CELLS * 8;
    static int ring[RING];
    int head = RING / 2, tail = RING / 2;
    int dist[CELLS];
    int side = s.side[player];
    for (int c = 0; c < CELLS; ++c) dist[c] = UNREACHABLE;
    for (int c = 0; c < CELLS; ++c) {
        if (!onStartBorder(side, c)) continue;
        if (s.board[c] == player) { dist[c] = 0; ring[--head] = c; }
        else if (s.canPlay(player, c)) { dist[c] = 1; ring[tail++] = c; }
    }
    while (head < tail) {
        int cell = ring[head++], d = dist[cell];
        if (onGoalBorder(side, cell)) return d;
        for (int i = 0; i < NEIGHBOUR_COUNT[cell]; ++i) {
            int seg = NEIGHBOUR_SEG[cell][i], other = NEIGHBOUR[cell][i];
            if (s.cut[player][seg]) continue;
            int owner = s.board[other];
            if (owner == player) {
                // two own pegs not linked: an older opponent link separates them
                if (s.board[cell] == player && !s.linked[player][seg]) continue;
                if (d < dist[other]) { dist[other] = d; ring[--head] = other; }
            } else if (owner == EMPTY && s.canPlay(player, other) && d + 1 < dist[other]) {
                dist[other] = d + 1;
                ring[tail++] = other;
            }
        }
    }
    return UNREACHABLE;
}

int evaluate(const State &s) {
    int mine = pathDistance(s, ME);
    if (mine == 0) return WIN_SCORE;
    int theirs = pathDistance(s, OPP);
    if (theirs == 0) return -WIN_SCORE;
    int links = s.linkCount[ME] - s.linkCount[OPP];
    return DISTANCE_WEIGHT * (theirs - mine) + LINK_WEIGHT * links;
}

// ---------------------------------------------------------------- search
// Insertion sort on integer keys, descending, stable (same in 4 languages).
void stableSortDesc(int *items, int *keys, int n) {
    for (int i = 1; i < n; ++i) {
        int item = items[i], key = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < key) { items[j + 1] = items[j]; keys[j + 1] = keys[j]; --j; }
        items[j + 1] = item; keys[j + 1] = key;
    }
}

// Lowest score the opponent can force; stops once it is <= floor.
int worstReply(const State &s, int floor) {
    int moves[CELLS], n = s.legalMoves(OPP, moves), worst = WIN_SCORE;
    for (int i = 0; i < n; ++i) {
        State child = s;
        child.place(OPP, moves[i]);
        int score = evaluate(child);
        if (score < worst) {
            worst = score;
            if (worst <= floor) break;
        }
    }
    return worst;
}

int chooseMove(const State &s, int topK, string &info) {
    int moves[CELLS], scores[CELLS], n = s.legalMoves(ME, moves);
    for (int i = 0; i < n; ++i) {
        State child = s;
        child.place(ME, moves[i]);
        scores[i] = evaluate(child);
    }
    stableSortDesc(moves, scores, n);
    if (scores[0] == WIN_SCORE) { info = "win"; return moves[0]; }
    int bestCell = moves[0], bestScore = -WIN_SCORE - 1;
    for (int i = 0; i < n && i < topK; ++i) {
        State child = s;
        child.place(ME, moves[i]);
        int score = worstReply(child, bestScore);
        if (score > bestScore) { bestScore = score; bestCell = moves[i]; }
    }
    info = "score=" + to_string(bestScore);
    return bestCell;
}

// ---------------------------------------------------------------- I/O
bool shouldSwap(int cell) {
    int x = xOf(cell), y = yOf(cell);
    return x >= SWAP_LOW && x <= SWAP_HIGH && y >= SWAP_LOW && y <= SWAP_HIGH;
}

string readLine() {
    string line;
    getline(cin, line);
    while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
    return line;
}

string firstWord(const string &line) { return line.substr(0, line.find(' ')); }

// Rebuild the state from one turn of input (pegs, then links, for each player).
void readState(State &s) {
    for (int player : {ME, OPP}) {
        int pegs = atoi(readLine().c_str());
        for (int i = 0; i < pegs; ++i) s.board[parseCell(firstWord(readLine()))] = (int8_t)player;
        int links = atoi(readLine().c_str());
        for (int i = 0; i < links; ++i) {
            string line = readLine();
            size_t sp = line.find(' ');
            int a = parseCell(line.substr(0, sp)), b = parseCell(line.substr(sp + 1));
            s.addLink(player, SEG_ID[a][b]);
        }
    }
}

string decide(const State &s, const string &last, bool firstTurn, int topK, string &info) {
    int myPegs = 0, theirPegs = 0, theirCell = -1;
    for (int c = 0; c < CELLS; ++c) {
        if (s.board[c] == ME) ++myPegs;
        if (s.board[c] == OPP) { ++theirPegs; if (theirCell < 0) theirCell = c; }
    }
    if (last == "FIRST") { info = "opening"; return nameOf(OPENING_CELL); }
    if (firstTurn && myPegs == 0 && theirPegs == 1 && shouldSwap(theirCell)) { info = "swap"; return "SWAP"; }
    int moves[CELLS];
    if (s.legalMoves(ME, moves) == 0) { info = "no move"; return nameOf(0); }
    return nameOf(chooseMove(s, topK, info));
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    buildGeometry();
    int topK = DEFAULT_TOP_K;
    if (const char *e = getenv("CG_NODES")) topK = atoi(e);
    int mySide = -1;
    while (true) {
        string last = readLine();
        if (last.empty()) break;
        auto t0 = chrono::steady_clock::now();
        bool firstTurn = mySide < 0;
        if (firstTurn) mySide = last == "FIRST" ? TOP_BOTTOM : LEFT_RIGHT;
        State s(mySide);
        readState(s);
        string info, move = decide(s, last, firstTurn, topK, info);
        cerr << info << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << move << endl;
    }
    return 0;
}
