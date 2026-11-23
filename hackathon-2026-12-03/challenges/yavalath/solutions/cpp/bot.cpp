// Yavalath (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Board rows 5,6,7,8,9,8,7,6,5 (referee MultiStruct/Yavalath); output "x y".
// Axial Q = x - min(y,4) + 4 ; dirs E W DR UL DL UR. Line >= 4 wins, exactly 3 loses.
// v1: negamax alpha-beta + ID, node budget (env CG_NODES); forced blocks
// searched without depth cost; eval = 2-stone 4-windows + safe-move diff.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

constexpr int NC = 61, MAXW = 120;
constexpr int WIN = 1000000, INF = 1 << 30, MAX_DEPTH = 64;
constexpr int W_TWO = 10, W_THREE = 40, W_SAFE = 3;
constexpr int OPENING = 7;
int ROWS[9], CX[NC], CY[NC], IDX[9][9];
int RAY[NC][6][3], RAYN[NC][6];
int WINDOWS[MAXW][4], NW = 0;
int WIN_OF[NC][12], WIN_OFN[NC];

void initGeo() {
    int n = 0;
    memset(IDX, -1, sizeof IDX);
    for (int y = 0; y < 9; ++y) {
        ROWS[y] = 9 - abs(4 - y);
        for (int x = 0; x < ROWS[y]; ++x) {
            CX[n] = x; CY[n] = y;
            IDX[x - min(y, 4) + 4][y] = n;
            ++n;
        }
    }
    const int dq[6] = {1, -1, 0, 0, -1, 1}, dr[6] = {0, 0, 1, -1, 1, -1};
    for (int i = 0; i < NC; ++i) {
        int q = CX[i] - min(CY[i], 4) + 4, r = CY[i];
        for (int d = 0; d < 6; ++d) {
            int k = 0;
            for (int s = 1; s < 4; ++s) {
                int a = q + dq[d] * s, b = r + dr[d] * s;
                if (a < 0 || a > 8 || b < 0 || b > 8 || IDX[a][b] < 0) break;
                RAY[i][d][k++] = IDX[a][b];
            }
            RAYN[i][d] = k;
        }
    }
    for (int i = 0; i < NC; ++i)
        for (int d = 0; d < 6; d += 2)
            if (RAYN[i][d] == 3) {
                WINDOWS[NW][0] = i;
                for (int k = 0; k < 3; ++k) WINDOWS[NW][k + 1] = RAY[i][d][k];
                ++NW;
            }
    for (int w = 0; w < NW; ++w)
        for (int k = 0; k < 4; ++k) { int c = WINDOWS[w][k]; WIN_OF[c][WIN_OFN[c]++] = w; }
}

inline int placeResult(const int8_t *b, int c, int p) {
    int best = 1;
    for (int a = 0; a < 6; a += 2) {
        int n = 1;
        for (int k = 0; k < RAYN[c][a]; ++k) { if (b[RAY[c][a][k]] != p) break; ++n; }
        for (int k = 0; k < RAYN[c][a + 1]; ++k) { if (b[RAY[c][a + 1][k]] != p) break; ++n; }
        if (n > best) best = n;
    }
    if (best >= 4) return 2;
    return best == 3 ? 1 : 0;
}

int evaluate(const int8_t *b, int p, int safeP, int safeO) {
    int o = 3 - p, s = 0;
    for (int w = 0; w < NW; ++w) {
        int a = 0, n = 0;
        for (int k = 0; k < 4; ++k) { int v = b[WINDOWS[w][k]]; if (v == p) ++a; else if (v == o) ++n; }
        if (n == 0) { if (a == 2) s += W_TWO; else if (a == 3) s += W_THREE; }
        else if (a == 0) { if (n == 2) s -= W_TWO; else if (n == 3) s -= W_THREE; }
    }
    return s + W_SAFE * (safeP - safeO);
}

int orderKey(const int8_t *b, int c, int p) {
    static const int OWN[4] = {0, 1, 4, 8}, OPP[4] = {0, 0, 3, 6};
    int o = 3 - p, k = 0;
    for (int j = 0; j < WIN_OFN[c]; ++j) {
        int w = WIN_OF[c][j], a = 0, n = 0;
        for (int t = 0; t < 4; ++t) { int v = b[WINDOWS[w][t]]; if (v == p) ++a; else if (v == o) ++n; }
        if (n == 0) k += OWN[a];
        else if (a == 0) k += OPP[n];
    }
    return k;
}

void sortMoves(const int8_t *b, int *mv, int n, int p) {
    int keys[NC];
    for (int i = 0; i < n; ++i) keys[i] = orderKey(b, mv[i], p);
    for (int i = 1; i < n; ++i) {
        int x = mv[i], k = keys[i], j = i - 1;
        while (j >= 0 && keys[j] < k) { mv[j + 1] = mv[j]; keys[j + 1] = keys[j]; --j; }
        mv[j + 1] = x; keys[j + 1] = k;
    }
}

enum Kind { K_WIN, K_DRAW, K_FORCED, K_FREE, K_LOST };
struct Analysis { int kind, n, safeP, safeO; int mv[NC]; bool loses[NC]; };

void analyse(const int8_t *b, int p, Analysis &A) {
    int o = 3 - p, nf = 0, nfree = 0, safeO = 0, empty = 0;
    int forced[NC]; bool fl[NC]; int freeM[NC];
    for (int c = 0; c < NC; ++c) {
        if (b[c]) continue;
        ++empty;
        int rp = placeResult(b, c, p);
        if (rp == 2) { A.kind = K_WIN; A.n = 1; A.mv[0] = c; return; }
        int ro = placeResult(b, c, o);
        if (ro == 2) { forced[nf] = c; fl[nf] = rp == 1; ++nf; }
        else if (ro == 0) ++safeO;
        if (rp == 0) freeM[nfree++] = c;
    }
    A.safeP = nfree; A.safeO = safeO;
    if (empty == 0) { A.kind = K_DRAW; A.n = 0; return; }
    if (nf) { A.kind = K_FORCED; A.n = nf; for (int i = 0; i < nf; ++i) { A.mv[i] = forced[i]; A.loses[i] = fl[i]; } return; }
    if (!nfree) { A.kind = K_LOST; A.n = 0; return; }
    A.kind = K_FREE; A.n = nfree;
    for (int i = 0; i < nfree; ++i) A.mv[i] = freeM[i];
}

long long NODE_LIMIT = 20000, nodes = 0;
int TIME_MS = 70, lastDepth = 0;
bool aborted = false;
chrono::steady_clock::time_point t0;

int negamax(int8_t *b, int p, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TIME_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    Analysis A;
    analyse(b, p, A);
    if (A.kind == K_WIN) return WIN - ply;
    if (A.kind == K_DRAW) return 0;
    if (A.kind == K_LOST) return -(WIN - ply - 1);
    int best = -INF;
    if (A.kind == K_FORCED) {
        for (int i = 0; i < A.n; ++i) {
            int c = A.mv[i], v;
            if (A.loses[i]) v = -(WIN - ply - 1);
            else {
                b[c] = (int8_t)p;
                v = -negamax(b, 3 - p, depth, -beta, -alpha, ply + 1);
                b[c] = 0;
                if (aborted) return 0;
            }
            if (v > best) best = v;
            if (v > alpha) alpha = v;
            if (alpha >= beta) break;
        }
        return best;
    }
    if (depth <= 0) return evaluate(b, p, A.safeP, A.safeO);
    sortMoves(b, A.mv, A.n, p);
    for (int i = 0; i < A.n; ++i) {
        int c = A.mv[i];
        b[c] = (int8_t)p;
        int v = -negamax(b, 3 - p, depth - 1, -beta, -alpha, ply + 1);
        b[c] = 0;
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

int search(int8_t *b, int p, int *mv, int n) {
    lastDepth = 0; nodes = 0; aborted = false;
    sortMoves(b, mv, n, p);
    int best = mv[0];
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curI = 0;
        for (int i = 0; i < n; ++i) {
            b[mv[i]] = (int8_t)p;
            int v = -negamax(b, 3 - p, depth - 1, -INF, -alpha, 1);
            b[mv[i]] = 0;
            if (aborted) break;
            if (v > bestV) { bestV = v; curI = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[curI];
        lastDepth = depth;
        int pv = mv[curI];
        for (int i = curI; i > 0; --i) mv[i] = mv[i - 1];
        mv[0] = pv;
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// returns cell, or -1 = steal the opponent's first stone
int choose(int8_t *b, int myId, bool firstTurn, int last, string &info) {
    int stones = 0;
    for (int c = 0; c < NC; ++c) stones += b[c] != 0;
    if (stones == 0) { info = "opening"; return OPENING; }
    if (myId == 2 && firstTurn && stones == 1 && last >= 0) {
        int q = CX[last] - min(CY[last], 4) + 4, r = CY[last];
        int dist = max(abs(q - 4), max(abs(r - 4), abs(q + r - 8)));
        if (dist <= 2) { info = "steal"; return -1; }
    }
    Analysis A;
    analyse(b, 1, A);
    if (A.kind == K_WIN) { info = "win"; return A.mv[0]; }
    int cand[NC], nc = 0;
    if (A.kind == K_FORCED) {
        for (int i = 0; i < A.n; ++i) if (!A.loses[i]) cand[nc++] = A.mv[i];
        if (nc <= 1) { info = "forced"; return nc ? cand[0] : A.mv[0]; }
    } else if (A.kind == K_FREE) {
        for (int i = 0; i < A.n; ++i) cand[nc++] = A.mv[i];
    } else {
        int c = 0;
        while (b[c]) ++c;
        info = "lost";
        return c;
    }
    int c = search(b, 1, cand, nc);
    info = "d=" + to_string(lastDepth) + " n=" + to_string(nodes);
    return c;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initGeo();
    long long cli = -1;
    if (const char *e = getenv("CG_NODES")) cli = atoll(e);
    int myId = 0, turn = 0;
    bool firstLine = true;
    string line;
    while (getline(cin, line)) {
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;
        t0 = chrono::steady_clock::now();
        if (firstLine) {
            myId = atoi(line.c_str());
            firstLine = false;
            if (!getline(cin, line)) break;
        }
        int count = atoi(line.c_str());
        int8_t b[NC];
        int k = 0;
        for (int i = 0; i < count; ++i) {
            string row;
            getline(cin, row);
            for (char ch : row) {
                if (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n') continue;
                if (k < NC) b[k] = (int8_t)(ch == '1' ? 1 : ch == '2' ? 2 : 0);
                ++k;
            }
        }
        for (; k < NC; ++k) b[k] = 0;
        int ox = -1, oy = -1;
        cin >> ox >> oy;
        getline(cin, line);
        int last = -1;
        if (oy >= 0 && oy < 9 && ox >= 0 && ox < ROWS[oy]) last = IDX[ox - min(oy, 4) + 4][oy];
        NODE_LIMIT = cli > 0 ? cli : (turn == 0 ? 140000 : 14000);
        TIME_MS = turn == 0 ? 700 : 70;
        string info;
        int c = choose(b, myId, turn == 0, last, info);
        if (c < 0) c = last;
        cerr << info << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << CX[c] << " " << CY[c] << endl;
        ++turn;
    }
    return 0;
}
