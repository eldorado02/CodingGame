// Brandubh 7x7 — C++ (submitted version), v9. Port of solutions/python/bot.py.
// Rules from the referee (github.com/n3l-ctrl/brandubh-codingame Referee.java),
// see the Python header. Index = y * 7 + x, y = 0 is row "7", x = 0 is "a".
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

constexpr int EMPTY = 0, ATT = 1, DEF = 2, KING = 3;
constexpr int SIDE_A = 0, SIDE_D = 1;
constexpr int THRONE = 24;
constexpr int CORNER_DIAG[4] = {8, 12, 36, 40};  // b6 f6 b2 f2
constexpr int DX[4] = {1, -1, 0, 0}, DY[4] = {0, 0, 1, -1};
constexpr int KDIST[5] = {0, -600, -250, -80, 0};
constexpr int MAX_MOVES = 128;

struct Move { int from = 0, to = 0; };
struct State { int cell[49]; int side; };

inline bool isCorner(int i) { return i == 0 || i == 6 || i == 42 || i == 48; }
inline int sideOf(int v) { return v == ATT ? SIDE_A : SIDE_D; }

int genMoves(const State &s, Move *out) {
    int n = 0;
    for (int i = 0; i < 49; ++i) {
        int v = s.cell[i];
        if (v == EMPTY || sideOf(v) != s.side) continue;
        int x0 = i % 7, y0 = i / 7;
        for (int d = 0; d < 4; ++d) {
            int x = x0 + DX[d], y = y0 + DY[d];
            while (x >= 0 && x < 7 && y >= 0 && y < 7) {
                int j = y * 7 + x;
                if (s.cell[j] != EMPTY) break;
                if (v == KING || !(j == THRONE || isCorner(j))) out[n++] = {i, j};
                x += DX[d]; y += DY[d];
            }
        }
    }
    return n;
}

inline bool hostile(const int *c, int x, int y, int capType, bool capKing) {
    if (x < 0 || x > 6 || y < 0 || y > 6) return false;
    int j = y * 7 + x;
    if (isCorner(j)) return true;
    bool targetAtt = capType != ATT;
    if (j == THRONE) {
        if (c[THRONE] == EMPTY) return !capKing;
        return targetAtt;
    }
    int p = c[j];
    if (p != EMPTY) return targetAtt ? p != ATT : p == ATT;
    return false;
}

inline bool isEnemy(int a, int b) { return a == ATT ? b != ATT : b == ATT; }

State applyMove(const State &s, const Move &m) {
    State t = s;
    int *c = t.cell;
    int v = c[m.from];
    c[m.from] = EMPTY;
    c[m.to] = v;
    int ex = m.to % 7, ey = m.to / 7;
    for (int d = 0; d < 4; ++d) {
        int ax = ex + DX[d], ay = ey + DY[d];
        if (ax < 0 || ax > 6 || ay < 0 || ay > 6) continue;
        int a = ay * 7 + ax;
        if (c[a] == EMPTY || !isEnemy(v, c[a])) continue;
        if (c[a] == KING && a == THRONE) {
            bool all = true;
            for (int e = 0; e < 4; ++e)
                if (!hostile(c, 3 + DX[e], 3 + DY[e], ATT, true)) { all = false; break; }
            if (all) c[a] = EMPTY;
        } else if (hostile(c, ax + DX[d], ay + DY[d], v, c[a] == KING)) {
            c[a] = EMPTY;
        }
    }
    t.side = 1 - s.side;
    return t;
}

bool encircled(const int *c) {
    bool seen[49] = {false};
    int queue[49], qt = 0, h = 0;
    for (int i = 0; i < 49; ++i) {
        int x = i % 7, y = i / 7;
        if (x == 0 || x == 6 || y == 0 || y == 6) {
            if (c[i] == KING || c[i] == DEF) return false;
            if (c[i] != ATT) { seen[i] = true; queue[qt++] = i; }
        }
    }
    while (h < qt) {
        int i = queue[h++];
        if (c[i] == KING || c[i] == DEF) return false;
        int x = i % 7, y = i / 7;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx >= 0 && nx < 7 && ny >= 0 && ny < 7) {
                int j = ny * 7 + nx;
                if (!seen[j] && c[j] != ATT) { seen[j] = true; queue[qt++] = j; }
            }
        }
    }
    return true;
}

inline int kingPos(const int *c) {
    for (int i = 0; i < 49; ++i) if (c[i] == KING) return i;
    return -1;
}

int winner(const State &s) {
    int k = kingPos(s.cell);
    if (k < 0) return SIDE_A;
    if (isCorner(k)) return SIDE_D;
    if (encircled(s.cell)) return SIDE_A;
    return -1;
}

int kingLines(const int *c, int k) {
    int n = 0, x0 = k % 7, y0 = k / 7;
    for (int d = 0; d < 4; ++d) {
        int x = x0 + DX[d], y = y0 + DY[d];
        while (x >= 0 && x < 7 && y >= 0 && y < 7) {
            int j = y * 7 + x;
            if (c[j] != EMPTY) break;
            if (isCorner(j)) ++n;
            x += DX[d]; y += DY[d];
        }
    }
    return n;
}

int kingDist(const int *c, int k) {
    int dist[49];
    for (int i = 0; i < 49; ++i) dist[i] = 9;
    int queue[49], qt = 0, h = 0;
    dist[k] = 0; queue[qt++] = k;
    while (h < qt) {
        int i = queue[h++];
        if (isCorner(i)) return dist[i] < 4 ? dist[i] : 4;
        if (dist[i] >= 3) continue;
        int x0 = i % 7, y0 = i / 7;
        for (int d = 0; d < 4; ++d) {
            int x = x0 + DX[d], y = y0 + DY[d];
            while (x >= 0 && x < 7 && y >= 0 && y < 7) {
                int j = y * 7 + x;
                if (c[j] != EMPTY) break;
                if (dist[j] == 9) { dist[j] = dist[i] + 1; queue[qt++] = j; }
                x += DX[d]; y += DY[d];
            }
        }
    }
    return 4;
}

int evaluate(const State &s) {
    const int *c = s.cell;
    int k = kingPos(c), a = 0, d = 0;
    for (int i = 0; i < 49; ++i) { if (c[i] == ATT) ++a; else if (c[i] == DEF) ++d; }
    int sc = 100 * a - 150 * d;
    int kx = k % 7, ky = k / 7;
    for (int e = 0; e < 4; ++e) {
        int x = kx + DX[e], y = ky + DY[e];
        if (x >= 0 && x < 7 && y >= 0 && y < 7 && c[y * 7 + x] == ATT) sc += 60;
    }
    for (int i : CORNER_DIAG) if (c[i] == ATT) sc += 60;
    sc += KDIST[kingDist(c, k)];
    if (s.side == SIDE_A && kingLines(c, k) >= 2) sc -= 3000;
    return s.side == SIDE_A ? sc : -sc;
}

int orderKey(const State &s, const Move &m) {
    int v = s.cell[m.from];
    if (v == KING) return isCorner(m.to) ? 10 : 1;
    if (v == ATT) {
        int k = kingPos(s.cell);
        int dx = abs(m.to % 7 - k % 7), dy = abs(m.to / 7 - k / 7);
        return dx + dy == 1 ? 2 : 0;
    }
    return 0;
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = 30000;
int TIME_MS = 35;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

void orderMoves(const State &s, Move *mv, int n) {
    int key[MAX_MOVES];
    for (int i = 0; i < n; ++i) key[i] = orderKey(s, mv[i]);
    for (int i = 1; i < n; ++i) {
        Move x = mv[i]; int k = key[i], j = i - 1;
        while (j >= 0 && key[j] < k) { mv[j + 1] = mv[j]; key[j + 1] = key[j]; --j; }
        mv[j + 1] = x; key[j + 1] = k;
    }
}

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    int w = winner(s);
    if (w >= 0) return (w == s.side) ? WIN - ply : -(WIN - ply);
    if (s.side == SIDE_D && kingLines(s.cell, kingPos(s.cell))) return WIN - (ply + 1);
    if (depth == 0) return evaluate(s);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (n == 0) return -(WIN - ply);
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
    lastDepth = 0;
    int n = (int)mv.size();
    orderMoves(root, mv.data(), n);
    Move best = mv[0];
    nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        Move cur = mv[0];
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; cur = mv[i]; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = cur;
        lastDepth = depth;
        Move pv = mv[curIdx];
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// ---------------------------------------------------------------- GAME: I/O
string keyOf(const State &s) {
    string k(50, '.');
    for (int i = 0; i < 49; ++i) k[i] = ".ADK"[s.cell[i]];
    k[49] = char('0' + s.side);
    return k;
}

string nameOf(const Move &m) {
    string r;
    r += char('a' + m.from % 7); r += char('7' - m.from / 7);
    r += char('a' + m.to % 7);   r += char('7' - m.to / 7);
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    map<string, int> history;
    bool first = true;
    string line;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        int role = atoi(line.c_str());
        if (line.find_first_of("0123456789") == string::npos) break;
        string last;
        if (!getline(cin, last)) break;
        State s;
        for (int y = 0; y < 7; ++y) {
            string row;
            if (!getline(cin, row)) row = "";
            for (int x = 0; x < 7; ++x) {
                char ch = x < (int)row.size() ? row[x] : '.';
                s.cell[y * 7 + x] = ch == 'A' ? ATT : ch == 'D' ? DEF : ch == 'K' ? KING : EMPTY;
            }
        }
        s.side = role == 0 ? SIDE_A : SIDE_D;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? 200000 : 12000);
        TIME_MS = first ? 700 : 35;
        first = false;
        history[keyOf(s)]++;
        Move all[MAX_MOVES];
        int na = genMoves(s, all);
        vector<Move> safe, pool;
        for (int i = 0; i < na; ++i) {
            auto it = history.find(keyOf(applyMove(s, all[i])));
            if (it == history.end() || it->second < 2) safe.push_back(all[i]);
        }
        if (!safe.empty()) pool = safe;
        else pool.assign(all, all + na);
        if (pool.empty()) { cout << "a1a1" << endl; continue; }
        Move m = search(s, pool);
        history[keyOf(applyMove(s, m))]++;
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << nameOf(m) << endl;
    }
    return 0;
}
