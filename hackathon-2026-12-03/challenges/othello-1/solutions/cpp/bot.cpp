// Othello 8x8 - determinist bitboard engine (fast fixed depth, no timeouts).
// Compile: g++ -O2 -std=c++20 -o othello_bb othello_bb.cpp
//
// Bit layout: bit 0 = a1, bit 7 = h1, bit 8 = a2, bit 63 = h8.
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using U64 = uint64_t;

constexpr U64 FA = 0x0101010101010101ULL; // file a
constexpr U64 FH = 0x8080808080808080ULL; // file h

inline U64 shE (U64 x) { return (x << 1) & ~FA; }
inline U64 shO (U64 x) { return (x >> 1) & ~FH; }
inline U64 shN (U64 x) { return  x << 8;        }
inline U64 shS (U64 x) { return  x >> 8;        }
inline U64 shNE(U64 x) { return (x << 9) & ~FA; }
inline U64 shNO(U64 x) { return (x << 7) & ~FH; }
inline U64 shSE(U64 x) { return (x >> 7) & ~FA; }
inline U64 shSO(U64 x) { return (x >> 9) & ~FH; }

// legal moves: from each own stone, walk over opponent stones in direction d
// (6 dilation steps, max opponent run length on an 8x8 board) and land on empty.
#define DILATE(SH) do {                 \
    U64 t = SH(P) & O;                  \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    m |= SH(t) & empty;                 \
} while (0)

inline U64 moves(U64 P, U64 O) {
    U64 m = 0, empty = ~(P | O);
    DILATE(shE);  DILATE(shO);  DILATE(shN);  DILATE(shS);
    DILATE(shNE); DILATE(shNO); DILATE(shSE); DILATE(shSO);
    return m;
}
#undef DILATE

// all discs flipped by putting a stone at mv (exactly one bit set)
#define WALK(SH) do {                   \
    U64 s = SH(mv), t = 0;              \
    while (s & O) { t |= s; s = SH(s); }\
    if (s & P) f |= t;                  \
} while (0)

inline U64 flips(U64 P, U64 O, U64 mv) {
    U64 f = 0;
    WALK(shE);  WALK(shO);  WALK(shN);  WALK(shS);
    WALK(shNE); WALK(shNO); WALK(shSE); WALK(shSO);
    return f;
}
#undef WALK

inline void apply(U64 P, U64 O, U64 mv, U64 &nP, U64 &nO) {
    U64 f = flips(P, O, mv);
    nP = P | mv | f;
    nO = O & ~f;
}

inline U64 neighbors(U64 x) {
    return shE(x) | shO(x) | shN(x) | shS(x)
         | shNE(x) | shNO(x) | shSE(x) | shSO(x);
}

// identical to POS in solutions/cpp/bot.cpp, indexed by bit index
constexpr int POS[64] = {
     120,-20, 20,  5,  5, 20,-20, 120,
     -20,-40, -5, -5, -5, -5,-40, -20,
      20, -5, 15,  3,  3, 15, -5,  20,
       5, -5,  3,  3,  3,  3, -5,   5,
       5, -5,  3,  3,  3,  3, -5,   5,
      20, -5, 15,  3,  3, 15, -5,  20,
     -20,-40, -5, -5, -5, -5,-40, -20,
     120,-20, 20,  5,  5, 20,-20, 120
};

inline int evaluate(U64 P, U64 O) {
    int pos = 0;
    U64 x = P;
    while (x) { int b = __builtin_ctzll(x); pos += POS[b]; x &= x - 1; }
    x = O;
    while (x) { int b = __builtin_ctzll(x); pos -= POS[b]; x &= x - 1; }
    U64 empty = ~(P | O);
    U64 nb = neighbors(empty);
    int score = pos
              + 20 * (__builtin_popcountll(moves(P, O)) - __builtin_popcountll(moves(O, P)))
              + 8  * (__builtin_popcountll(O & nb) - __builtin_popcountll(P & nb));
    if (__builtin_popcountll(empty) <= 12)
        score += 8 * ((int)__builtin_popcountll(P) - (int)__builtin_popcountll(O));
    return score;
}

constexpr int WIN = 1000000000;
constexpr int INF = 2000000000;

// exact terminal score from material
inline int terminal(U64 P, U64 O) {
    int d = __builtin_popcountll(P) - __builtin_popcountll(O);
    return d > 0 ? WIN : (d < 0 ? -WIN : 0);
}

// move ordering: POS[bit] descending, ties keep ascending bit index
inline int ordered_bits(U64 m, array<int, 32> &out) {
    int n = 0;
    while (m) {
        int b = __builtin_ctzll(m);
        out[n++] = b;
        m &= m - 1;
    }
    for (int i = 1; i < n; ++i) {
        int v = out[i], j = i - 1;
        while (j >= 0 && POS[out[j]] < POS[v]) { out[j + 1] = out[j]; --j; }
        out[j + 1] = v;
    }
    return n;
}

int search(U64 P, U64 O, int depth, int alpha, int beta, bool passed) {
    U64 m = moves(P, O);
    if (!m) {
        if (passed) return terminal(P, O);
        return -search(O, P, depth, -beta, -alpha, true);
    }
    if (depth <= 0) return evaluate(P, O);
    array<int, 32> ord;
    int n = ordered_bits(m, ord);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        U64 nP, nO;
        apply(P, O, 1ULL << ord[i], nP, nO);
        int val = -search(nO, nP, depth - 1, -beta, -alpha, false);
        if (val > best) best = val;
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }
    return best;
}

#ifndef OTHELLO_DEPTH
#define OTHELLO_DEPTH 6
#endif

int choose(U64 P, U64 O) {
    U64 m = moves(P, O);
    if (!m) return -1;
    array<int, 32> ord;
    int n = ordered_bits(m, ord);
    int best = ord[0], best_val = -INF;
    for (int depth = 1; depth <= OTHELLO_DEPTH; ++depth) {
        int local_best = -INF, local_move = best, alpha = -INF;
        for (int i = 0; i < n; ++i) {
            U64 nP, nO;
            apply(P, O, 1ULL << ord[i], nP, nO);
            int val = -search(nO, nP, depth - 1, -INF, -alpha, false);
            if (val > local_best) { local_best = val; local_move = ord[i]; }
            if (val > alpha) alpha = val;
        }
        best = local_move;
        best_val = local_best;
        if (best_val >= WIN / 2) break;
    }
    return best;
}

// true if s is a single non-negative integer (with optional surrounding blanks)
inline bool parse_int(const string &s, int &v) {
    size_t i = 0;
    while (i < s.size() && s[i] == ' ') ++i;
    if (i == s.size() || s[i] < '0' || s[i] > '9') return false;
    long long x = 0;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        x = x * 10 + (s[i] - '0');
        if (x > 1000000) return false;
        ++i;
    }
    while (i < s.size() && s[i] == ' ') ++i;
    if (i != s.size()) return false;
    v = (int)x;
    return true;
}

#ifndef OTHELLO_BB_DRIVER
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    auto is_board_row = [](const string &s) {
        if (s.size() < 8) return false;
        for (int c = 0; c < 8; ++c)
            if (s[c] != '.' && s[c] != '0' && s[c] != '1') return false;
        return true;
    };
    string line;
    int id = 0, size = 8;
    // id / boardSize are only sent on the first three turns by the referee.
    if (getline(cin, line)) parse_int(line, id);
    if (getline(cin, line)) parse_int(line, size);
    while (true) {
        U64 P = 0, O = 0;
        int rows = 0;
        // Skip id/size lines (when present) and any other non-board line.
        while (rows < 8) {
            if (!getline(cin, line)) return 0;
            if (!is_board_row(line)) continue;
            for (int c = 0; c < 8; ++c) {
                U64 bit = 1ULL << (rows * 8 + c);
                if (line[c] == '0') P |= bit;
                else if (line[c] == '1') O |= bit;
            }
            ++rows;
        }
        if (id == 1) swap(P, O);
        // Optional "EXPERT" extra line (only if WE output EXPERT).
        int n = -1;
        while (getline(cin, line)) {
            if (parse_int(line, n)) break;
        }
        if (n < 0) break;
        vector<int> actions;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            size_t k = 0;
            while (k < line.size() && line[k] == ' ') ++k;
            if (k + 1 >= line.size()) continue;
            actions.push_back((line[k] - 'a') + (line[k + 1] - '1') * 8);
        }
#ifdef OTHELLO_BENCH
        auto t0 = chrono::steady_clock::now();
#endif
        int mv = choose(P, O);
#ifdef OTHELLO_BENCH
        auto t1 = chrono::steady_clock::now();
        cerr << "BENCH_US " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << '\n';
#endif
        bool found = false;
        for (int a : actions) if (a == mv) { found = true; break; }
        if (!found && !actions.empty()) mv = actions[0];
        if (mv < 0) { cout << "PASS\n" << flush; continue; }
        cout << char('a' + mv % 8) << char('1' + mv / 8) << '\n' << flush;
    }
    return 0;
}
#endif // OTHELLO_BB_DRIVER
