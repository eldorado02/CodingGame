#include <bits/stdc++.h>
using namespace std;

static const int SIZE = 7;
static const int EMPTY = 0, ATT = 1, DEF = 2, KING = 3;
static const int WIN = 1000000000;
static const int DX[4] = {1, -1, 0, 0};
static const int DY[4] = {0, 0, 1, -1};
using Board = array<int, 49>;

static inline int idx(int x, int y) { return y * SIZE + x; }
static inline bool corner(int x, int y) {
    return (x == 0 || x == 6) && (y == 0 || y == 6);
}
static inline bool throne(int x, int y) { return x == 3 && y == 3; }

struct Move { int sx, sy, ex, ey; };

static bool hostile(const Board& g, int x, int y, int capturing, bool capturingKing) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return false;
    if (corner(x, y)) return true;
    bool targetAttacker = capturing != ATT;
    if (throne(x, y)) {
        int occ = g[idx(x, y)];
        if (occ == EMPTY) return !capturingKing;
        return targetAttacker;
    }
    int p = g[idx(x, y)];
    if (p == EMPTY) return false;
    return targetAttacker ? (p != ATT) : (p == ATT);
}

static bool applyMove(const Board& g, const Move& m, Board& out) {
    out = g;
    int piece = out[idx(m.sx, m.sy)];
    out[idx(m.sx, m.sy)] = EMPTY;
    out[idx(m.ex, m.ey)] = piece;
    for (int d = 0; d < 4; ++d) {
        int ax = m.ex + DX[d], ay = m.ey + DY[d];
        if (ax < 0 || ax >= SIZE || ay < 0 || ay >= SIZE) continue;
        int adj = out[idx(ax, ay)];
        if (adj == EMPTY || (piece == ATT && adj == ATT) || (piece != ATT && adj != ATT)) continue;
        if (adj == KING && throne(ax, ay)) {
            bool all = true;
            for (int k = 0; k < 4; ++k)
                if (!hostile(out, ax + DX[k], ay + DY[k], ATT, true)) { all = false; break; }
            if (all) return true;
            continue;
        }
        if (hostile(out, ax + DX[d], ay + DY[d], piece, adj == KING))
            out[idx(ax, ay)] = EMPTY;
    }
    return false;
}

static vector<Move> genMoves(const Board& g, int player) {
    vector<Move> out;
    for (int sy = 0; sy < SIZE; ++sy) for (int sx = 0; sx < SIZE; ++sx) {
        int p = g[idx(sx, sy)];
        if (p == EMPTY) continue;
        if (p == KING) { if (player != 1) continue; }
        else if ((p == ATT) != (player == 0)) continue;
        for (int d = 0; d < 4; ++d) {
            int x = sx, y = sy;
            while (true) {
                x += DX[d]; y += DY[d];
                if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) break;
                if (g[idx(x, y)] != EMPTY) break;
                if ((throne(x, y) || corner(x, y)) && p != KING) continue;
                out.push_back({sx, sy, x, y});
            }
        }
    }
    return out;
}

static int kingPos(const Board& g) {
    for (int i = 0; i < 49; ++i) if (g[i] == KING) return i;
    return -1;
}

static int escapeDist(const Board& g, int kp) {
    int kx = kp % SIZE, ky = kp / SIZE;
    bool seen[49] = {};
    queue<array<int,3>> q;
    seen[kp] = true;
    q.push({kx, ky, 0});
    while (!q.empty()) {
        auto cur = q.front(); q.pop();
        int x = cur[0], y = cur[1], dist = cur[2];
        if (corner(x, y)) return dist;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
            int ni = idx(nx, ny);
            if (seen[ni] || g[ni] == ATT) continue;
            seen[ni] = true;
            q.push({nx, ny, dist + 1});
        }
    }
    return 14;
}

static bool encircled(const Board& g) {
    bool seen[49] = {};
    queue<int> q;
    for (int x = 0; x < SIZE; ++x) for (int y = 0; y < SIZE; ++y) {
        if (x != 0 && x != 6 && y != 0 && y != 6) continue;
        int p = g[idx(x, y)];
        if (p == KING || p == DEF) return false;
        if (p == EMPTY) { seen[idx(x, y)] = true; q.push(idx(x, y)); }
    }
    while (!q.empty()) {
        int i = q.front(); q.pop();
        int x = i % SIZE, y = i / SIZE;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) continue;
            int ni = idx(nx, ny);
            if (seen[ni]) continue;
            int p = g[ni];
            if (p == KING || p == DEF) return false;
            if (p != ATT) { seen[ni] = true; q.push(ni); }
        }
    }
    return true;
}

static int evaluate(const Board& g, int player) {
    int kp = kingPos(g);
    if (kp < 0) return player == 0 ? WIN : -WIN;
    int kx = kp % SIZE, ky = kp / SIZE;
    if (corner(kx, ky)) return player == 0 ? -WIN : WIN;
    if (encircled(g)) return player == 0 ? WIN : -WIN;
    int atts = 0, defs = 0;
    for (int v : g) { if (v == ATT) ++atts; else if (v == DEF) ++defs; }
    int dist = escapeDist(g, kp);
    int mob = 0;
    for (int d = 0; d < 4; ++d) {
        int x = kx, y = ky;
        while (true) {
            x += DX[d]; y += DY[d];
            if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || g[idx(x, y)] != EMPTY) break;
            ++mob;
            if (corner(x, y)) break;
        }
    }
    int adj = 0;
    for (int d = 0; d < 4; ++d) {
        int x = kx + DX[d], y = ky + DY[d];
        if (x >= 0 && x < SIZE && y >= 0 && y < SIZE && g[idx(x, y)] == ATT) ++adj;
    }
    if (player == 0) return -120 * dist - 8 * mob + 50 * adj + 300 * (atts - defs);
    return 120 * dist + 8 * mob - 50 * adj + 300 * (defs - atts);
}

static chrono::steady_clock::time_point deadline;
static bool outOfTime() { return chrono::steady_clock::now() >= deadline; }

static int search(const Board& g, int player, int depth, int alpha, int beta) {
    if (outOfTime()) return evaluate(g, player);
    auto moves = genMoves(g, player);
    if (moves.empty()) return player == 0 ? -WIN : WIN;
    if (depth == 0) return evaluate(g, player);
    int best = player == 0 ? -2 * WIN : 2 * WIN;
    for (const auto& m : moves) {
        Board ng;
        bool dead = applyMove(g, m, ng);
        int val = dead ? (player == 0 ? WIN : -WIN)
                       : search(ng, 1 - player, depth - 1, alpha, beta);
        if (player == 0) { best = max(best, val); alpha = max(alpha, best); }
        else { best = min(best, val); beta = min(beta, best); }
        if (beta <= alpha) break;
    }
    return best;
}

static Move choose(const Board& g, int role) {
    auto moves = genMoves(g, role);
    if (moves.empty()) return {-1, -1, -1, -1};
    Move best = moves[0];
    int bestVal = -2 * WIN;
    for (int depth = 1; depth <= 4; ++depth) {
        int alpha = -2 * WIN, beta = 2 * WIN;
        Move localBest = best;
        int localVal = -2 * WIN;
        bool aborted = false;
        for (const auto& m : moves) {
            if (outOfTime()) { aborted = true; break; }
            Board ng;
            bool dead = applyMove(g, m, ng);
            int val = dead ? WIN : search(ng, 1 - role, depth - 1, alpha, beta);
            if (val > localVal) { localVal = val; localBest = m; }
            alpha = max(alpha, localVal);
        }
        if (!aborted) { best = localBest; bestVal = localVal; }
        if (outOfTime() || bestVal >= WIN / 2) break;
    }
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        int role = stoi(line);
        getline(cin, line);
        Board g{};
        for (int y = 0; y < SIZE; ++y) {
            getline(cin, line);
            for (int x = 0; x < SIZE; ++x) {
                char c = line[x];
                g[idx(x, y)] = c == 'A' ? ATT : c == 'D' ? DEF : c == 'K' ? KING : EMPTY;
            }
        }
        deadline = chrono::steady_clock::now() + chrono::milliseconds(42);
        Move m = choose(g, role);
        if (m.sx < 0) { cout << "a1a1\n" << flush; continue; }
        cout << char('a' + m.sx) << char('7' - m.sy)
             << char('a' + m.ex) << char('7' - m.ey) << '\n' << flush;
    }
    return 0;
}
