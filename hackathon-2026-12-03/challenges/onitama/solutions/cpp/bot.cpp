// Onitama 5x5 — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Cells: index = (row - 1) * 5 + x, row 1 = last input line; cards stored in
// player 0's frame (player 1 negates). CodinGame compiles without -O2: pragma.
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int SIZE = 5, CELLS = SIZE * SIZE;
constexpr long long FIRST_TURN_NODES = 30000, TURN_NODES = 30000;
constexpr int TURN_MS = 35;
constexpr int STUDENT_VALUE = 100, SHRINE_DISTANCE_WEIGHT = 4;
constexpr int SHRINE[2] = {2 + 4 * SIZE, 2};  // player 0 targets C5, player 1 targets C1
constexpr int EMPTY = 0, MASTER = 1, STUDENT = 2;
constexpr int MAX_MOVES = 2 * CELLS * 4;

struct Move { int card, src, dst; };  // src = -1 for PASS
struct State {
    int kind[CELLS], owner[CELLS];
    int hands[2][2], center, side;
};

map<int, vector<pair<int, int>>> CARDS;  // card id -> (dx, dy) in player 0's frame

inline int cellIndex(int x, int row) { return (row - 1) * SIZE + x; }

int genMoves(const State &s, Move *out) {
    int n = 0, sign = s.side == 0 ? 1 : -1;
    for (int h = 0; h < 2; ++h) {
        int card = s.hands[s.side][h];
        const auto &moves = CARDS[card];
        for (int src = 0; src < CELLS; ++src) {
            if (s.kind[src] == EMPTY || s.owner[src] != s.side) continue;
            int x = src % SIZE, row = src / SIZE + 1;
            for (const auto &d : moves) {
                int nx = x + sign * d.first, nrow = row + sign * d.second;
                if (nx < 0 || nx >= SIZE || nrow < 1 || nrow > SIZE) continue;
                int dst = cellIndex(nx, nrow);
                if (s.kind[dst] != EMPTY && s.owner[dst] == s.side) continue;
                out[n++] = {card, src, dst};
            }
        }
    }
    if (!n) for (int h = 0; h < 2; ++h) out[n++] = {s.hands[s.side][h], -1, -1};
    return n;
}

State applyMove(const State &s, const Move &m) {
    State t = s;
    if (m.src >= 0) {
        t.kind[m.dst] = s.kind[m.src];
        t.owner[m.dst] = s.owner[m.src];
        t.kind[m.src] = EMPTY;
    }
    for (int h = 0; h < 2; ++h)
        if (t.hands[s.side][h] == m.card) { t.hands[s.side][h] = s.center; break; }
    t.center = m.card;
    t.side = 1 - s.side;
    return t;
}

int masterOf(const State &s, int p) {
    for (int i = 0; i < CELLS; ++i) if (s.kind[i] == MASTER && s.owner[i] == p) return i;
    return -1;
}

int winner(const State &s) {
    for (int p = 0; p < 2; ++p) {
        int m = masterOf(s, p);
        if (m < 0) return 1 - p;
        if (m == SHRINE[p]) return p;
    }
    return -1;
}

inline int distance(int a, int b) { return max(abs(a % SIZE - b % SIZE), abs(a / SIZE - b / SIZE)); }

int sideScore(const State &s, int p) {
    int students = 0;
    for (int i = 0; i < CELLS; ++i) students += s.kind[i] == STUDENT && s.owner[i] == p;
    return STUDENT_VALUE * students - SHRINE_DISTANCE_WEIGHT * distance(masterOf(s, p), SHRINE[p]);
}

inline int evaluate(const State &s) { return sideScore(s, s.side) - sideScore(s, 1 - s.side); }

inline int orderKey(const State &s, const Move &m) { return (m.src >= 0 && s.kind[m.dst] != EMPTY) ? 1 : 0; }

void orderMoves(const State &s, Move *mv, int n) {  // stable insertion sort, captures first
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
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 1023) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TURN_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int w = winner(s);
    if (w >= 0) return w == s.side ? WIN - ply : -(WIN - ply);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
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
    if (t.size() < at + 2) return -1;
    char c = t[at], r = t[at + 1];
    if (c < 'A' || c > 'E' || r < '1' || r > '5') return -1;
    return cellIndex(c - 'A', r - '0');
}

string nameOf(int i) { return string(1, char('A' + i % SIZE)) + char('1' + i / SIZE); }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int me = atoi(line.c_str());
    bool first = true;
    while (true) {
        State s;
        for (int li = 0; li < SIZE; ++li) {
            if (!getline(cin, line)) return 0;
            if (li == 0) t0 = chrono::steady_clock::now();
            int row = SIZE - li;
            for (int x = 0; x < SIZE; ++x) {
                char ch = x < (int)line.size() ? line[x] : '.';
                int i = cellIndex(x, row);
                s.kind[i] = EMPTY; s.owner[i] = 0;
                if (ch == 'W' || ch == 'w') { s.kind[i] = ch == 'W' ? MASTER : STUDENT; s.owner[i] = 0; }
                else if (ch == 'B' || ch == 'b') { s.kind[i] = ch == 'B' ? MASTER : STUDENT; s.owner[i] = 1; }
            }
        }
        int handCount[2] = {0, 0};
        s.center = -1;
        for (int k = 0; k < 5; ++k) {
            if (!getline(cin, line)) return 0;
            istringstream in(line);
            int cardOwner, cardId, v[8] = {0};
            in >> cardOwner >> cardId;
            for (int j = 0; j < 8; ++j) in >> v[j];
            int user = cardOwner == -1 ? me : cardOwner;  // the centre card's next user is us
            int sign = user == 0 ? 1 : -1;
            vector<pair<int, int>> moves;
            for (int j = 0; j < 8; j += 2)
                if (v[j] || v[j + 1]) moves.push_back({sign * v[j], sign * v[j + 1]});
            CARDS[cardId] = moves;
            if (cardOwner == -1) s.center = cardId;
            else if (cardOwner == 0 || cardOwner == 1) s.hands[cardOwner][handCount[cardOwner]++] = cardId;
        }
        if (!getline(cin, line)) return 0;
        int n = atoi(line.c_str());
        vector<Move> legal;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            istringstream in(line);
            string cardTok, act;
            in >> cardTok >> act;
            if (cardTok.empty() || act.empty()) continue;
            int card = atoi(cardTok.c_str());
            if (act == "PASS") { legal.push_back({card, -1, -1}); continue; }
            int a = square(act, 0), b = square(act, 2);
            if (a >= 0 && b >= 0) legal.push_back({card, a, b});
        }
        s.side = me;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        first = false;
        if (legal.empty()) { cout << s.hands[me][0] << " PASS" << endl; continue; }
        Move m = search(s, legal);
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        if (m.src < 0) cout << m.card << " PASS" << endl;
        else cout << m.card << ' ' << nameOf(m.src) << nameOf(m.dst) << endl;
    }
}
