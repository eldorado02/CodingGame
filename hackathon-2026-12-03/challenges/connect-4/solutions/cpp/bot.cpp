#include <bits/stdc++.h>
using namespace std;

static const int R = 7, C = 9;
static const int WIN = 1000000;
struct Board {
    array<unsigned char, R * C> cell{};
    array<unsigned char, C> h{};
    bool stolen = false;
};

static inline int at(const Board& b, int r, int c) { return b.cell[r * C + c]; }

static bool winsAt(const Board& b, int r, int c, int p) {
    static const int DR[4] = {0, 1, 1, 1};
    static const int DC[4] = {1, 0, 1, -1};
    for (int d = 0; d < 4; ++d) {
        int cnt = 1;
        for (int s = 1; s < 4; ++s) {
            int nr = r + DR[d] * s, nc = c + DC[d] * s;
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || at(b, nr, nc) != p) break;
            ++cnt;
        }
        for (int s = 1; s < 4; ++s) {
            int nr = r - DR[d] * s, nc = c - DC[d] * s;
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || at(b, nr, nc) != p) break;
            ++cnt;
        }
        if (cnt >= 4) return true;
    }
    return false;
}

static bool play(Board& b, int col, int p) {
    int r = R - 1 - b.h[col];
    b.cell[r * C + col] = p;
    b.h[col]++;
    return winsAt(b, r, col, p);
}

static int evaluate(const Board& b, int p) {
    int opp = 3 - p;
    int score = 0;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            static const int DR[4] = {0, 1, 1, 1};
            static const int DC[4] = {1, 0, 1, -1};
            for (int d = 0; d < 4; ++d) {
                int ar = r - DR[d] * 3, ac = c - DC[d] * 3;
                if (ar < 0 || ac < 0 || ac >= C) continue;
                int mine = 0, theirs = 0;
                bool ok = true;
                for (int s = 0; s < 4; ++s) {
                    int rr = ar + DR[d] * s, cc = ac + DC[d] * s;
                    if (rr >= R || cc >= C) { ok = false; break; }
                    int v = at(b, rr, cc);
                    if (v == p) ++mine;
                    else if (v == opp) ++theirs;
                }
                if (!ok) continue;
                if (mine && !theirs) score += 1 << (mine - 1);
                if (theirs && !mine) score -= 1 << (theirs - 1);
            }
        }
    }
    score += 2 * (at(b, 0, 4) == p) - 2 * (at(b, 0, 4) == opp);
    return score;
}

struct TTEntry { int depth, val, flag, move; };
static unordered_map<uint64_t, TTEntry> tt;
static uint64_t key(const Board& b, int p) {
    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < R * C; ++i) { h ^= b.cell[i] + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2); }
    h ^= p;
    return h;
}

static chrono::steady_clock::time_point deadline;
static int nodes = 0;

static int search(Board& b, int p, int depth, int alpha, int beta, int ply) {
    (void)nodes;
    if (depth <= 0) return evaluate(b, p);
    int best = -2 * WIN;
    int bestMove = -1;
    uint64_t k = key(b, p);
    auto it = tt.find(k);
    int ttMove = -1;
    if (it != tt.end() && it->second.depth >= depth) {
        int v = it->second.val;
        if (it->second.flag == 0) return v;
        if (it->second.flag == 1 && v >= beta) return v;
        if (it->second.flag == 2 && v <= alpha) return v;
        ttMove = it->second.move;
    }
    vector<int> moves;
    moves.reserve(C);
    if (ttMove >= 0 && b.h[ttMove] < R) moves.push_back(ttMove);
    for (int c = 4, k2 = 0; k2 < C; ++k2, c = (c + (k2 % 2 ? k2 : -k2) + C) % C) {
        if (b.h[c] < R && find(moves.begin(), moves.end(), c) == moves.end()) moves.push_back(c);
    }
    int origAlpha = alpha;
    bool full = true;
    for (int c : moves) {
        Board nb = b;
        bool win = play(nb, c, p);
        int val;
        if (win) val = WIN - ply;
        else {
            bool draw = true;
            for (int j = 0; j < C; ++j) if (nb.h[j] < R) { draw = false; break; }
            val = draw ? 0 : -search(nb, 3 - p, depth - 1, -beta, -alpha, ply + 1);
        }
        if (val > best) { best = val; bestMove = c; }
        if (best > alpha) alpha = best;
        if (alpha >= beta) { full = false; break; }
    }
    tt[k] = {depth, best, full ? 0 : (best >= beta ? 1 : 2), bestMove};
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int myId, oppId;
    if (!(cin >> myId >> oppId)) return 0;
    int me = myId + 1;
    while (true) {
        int turn;
        if (!(cin >> turn)) break;
        Board b;
        for (int r = 0; r < R; ++r) {
            string row;
            cin >> row;
            for (int c = 0; c < C; ++c) {
                b.cell[r * C + c] = row[c] == '.' ? 0 : row[c] - '0' + 1;
            }
        }
        for (int c = 0; c < C; ++c) {
            int cnt = 0;
            for (int r = 0; r < R; ++r) if (b.cell[r * C + c] != 0) ++cnt;
            b.h[c] = cnt;
        }
        int n;
        cin >> n;
        vector<int> valid(n);
        bool canSteal = false;
        for (int i = 0; i < n; ++i) { cin >> valid[i]; if (valid[i] == -2) canSteal = true; }
        int oppPrev;
        cin >> oppPrev;
        if (canSteal) { cout << "STEAL\n" << flush; continue; }
        deadline = chrono::steady_clock::now() + chrono::milliseconds(85);
        nodes = 0;
        int bestCol = valid.empty() ? 4 : valid[0], bestVal = -2 * WIN;
        int p = me;
        for (int depth = 1; depth <= 6; ++depth) {
            int alpha = -2 * WIN, beta = 2 * WIN;
            int localBest = -1, localVal = -2 * WIN;
            bool aborted = false;
            for (int c : valid) {
                
                Board nb = b;
                bool win = play(nb, c, p);
                int val = win ? WIN : (depth == 1 ? evaluate(nb, p) : -search(nb, 3 - p, depth - 1, -2 * WIN, -alpha, 1));
                if (val > localVal) { localVal = val; localBest = c; }
                if (val > alpha) alpha = val;
            }
            if (!aborted && localBest >= 0) { bestCol = localBest; bestVal = localVal; }
            if (bestVal >= WIN / 2) break;
        }
        cout << bestCol << '\n' << flush;
    }
    return 0;
}
