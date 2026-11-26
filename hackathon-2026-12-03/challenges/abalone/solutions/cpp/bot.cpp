// Abalone — C++ (submitted), v1 one-ply greedy on legal moves. Port of solutions/python/bot.py.
// Rows of 5..9..5 cells; axial r = y - 4, q = x + max(-4, -r - 4); directions
// 0 E, 1 NE, 2 NW, 3 W, 4 SW, 5 SE. Each legal move is played (in-line pushes
// with Sumito and marbles falling off, or side-steps) and scored: captures,
// our distance to the centre (-), theirs (+), our cohesion (+). Ties: first.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int RADIUS = 4, ROWS = 2 * RADIUS + 1;
constexpr int DIRECTIONS[6][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}};
constexpr long long CAPTURE_WEIGHT = 10000, CENTER_WEIGHT = 10, EDGE_WEIGHT = 5, COHESION_WEIGHT = 2;
constexpr char EMPTY = '0';

using Board = array<array<char, ROWS>, ROWS>;  // [q + RADIUS][r + RADIUS]
using Move = array<int, 5>;
struct Hex { int q, r; };

int qMin(int r) { return max(-RADIUS, -r - RADIUS); }
int qMax(int r) { return min(RADIUS, -r + RADIUS); }
Hex toAxial(int x, int y) { int r = y - RADIUS; return {x + qMin(r), r}; }
bool onBoard(Hex h) { return h.r >= -RADIUS && h.r <= RADIUS && h.q >= qMin(h.r) && h.q <= qMax(h.r); }
int distance(int q, int r) { return (abs(q) + abs(r) + abs(q + r)) / 2; }
int sign(int v) { return (v > 0) - (v < 0); }
char &cell(Board &b, Hex h) { return b[h.q + RADIUS][h.r + RADIUS]; }
char cellOf(const Board &b, Hex h) { return b[h.q + RADIUS][h.r + RADIUS]; }

// Plays our move on `b`; returns the enemy marbles pushed off.
int play(Board &b, char me, const Move &m) {
    Hex first = toAxial(m[0], m[1]), last = toAxial(m[2], m[3]);
    int steps = distance(last.q - first.q, last.r - first.r);
    Hex axis{sign(last.q - first.q), sign(last.r - first.r)};
    vector<Hex> line;
    for (int k = 0; k <= steps; ++k) {
        Hex h{first.q + k * axis.q, first.r + k * axis.r};
        if (onBoard(h)) line.push_back(h);
    }
    if (line.empty()) return 0;
    int d = ((m[4] % 6) + 6) % 6, dq = DIRECTIONS[d][0], dr = DIRECTIONS[d][1];
    bool inline_ = steps == 0 || (dq == axis.q && dr == axis.r) || (dq == -axis.q && dr == -axis.r);
    Board before = b;
    int captured = 0;
    if (inline_) {
        Hex head = line[0];
        for (const Hex &h : line)
            if (h.q * dq + h.r * dr > head.q * dq + head.r * dr) head = h;
        vector<Hex> chain;
        Hex cursor{head.q + dq, head.r + dr};
        while (onBoard(cursor) && cellOf(before, cursor) != EMPTY && cellOf(before, cursor) != me) {
            chain.push_back(cursor);
            cursor = {cursor.q + dq, cursor.r + dr};
        }
        for (int k = (int)chain.size() - 1; k >= 0; --k) {
            Hex target{chain[k].q + dq, chain[k].r + dr};
            if (onBoard(target)) cell(b, target) = cellOf(before, chain[k]);
            else ++captured;
            cell(b, chain[k]) = EMPTY;
        }
    }
    for (const Hex &h : line) cell(b, h) = EMPTY;
    for (const Hex &h : line) {
        Hex target{h.q + dq, h.r + dr};
        if (onBoard(target)) cell(b, target) = me;
    }
    return captured;
}

long long evaluate(const Board &b, char me) {
    long long score = 0;
    for (int r = -RADIUS; r <= RADIUS; ++r)
        for (int q = qMin(r); q <= qMax(r); ++q) {
            char v = cellOf(b, {q, r});
            if (v == me) {
                score -= CENTER_WEIGHT * distance(q, r);
                for (const auto &d : DIRECTIONS) {
                    Hex n{q + d[0], r + d[1]};
                    if (onBoard(n) && cellOf(b, n) == me) score += COHESION_WEIGHT;
                }
            } else if (v != EMPTY) {
                score += EDGE_WEIGHT * distance(q, r);
            }
        }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    string me;
    if (!(cin >> me)) return 0;
    int myScore, theirScore;
    while (cin >> myScore >> theirScore) {
        Board board;
        for (auto &column : board) column.fill(EMPTY);
        for (int y = 0; y < ROWS; ++y) {
            string row;
            cin >> row;
            int r = y - RADIUS, width = qMax(r) - qMin(r) + 1;
            for (int x = 0; x < width; ++x) cell(board, toAxial(x, y)) = x < (int)row.size() ? row[x] : EMPTY;
        }
        Move last;
        for (int &v : last) cin >> v;
        int count;
        cin >> count;
        vector<Move> moves(max(0, count));
        for (Move &m : moves)
            for (int &v : m) cin >> v;
        if (!cin) return 0;
        int best = -1;
        long long bestScore = 0;
        for (int i = 0; i < (int)moves.size(); ++i) {
            Board after = board;
            long long score = CAPTURE_WEIGHT * play(after, me[0], moves[i]) + evaluate(after, me[0]);
            if (best < 0 || score > bestScore) { best = i; bestScore = score; }
        }
        if (best < 0) { cout << "0 0 0 0 0" << endl; continue; }
        const Move &m = moves[best];
        cout << m[0] << ' ' << m[1] << ' ' << m[2] << ' ' << m[3] << ' ' << m[4] << endl;
    }
}
