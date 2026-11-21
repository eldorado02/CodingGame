// Ataxx bot — deterministic 1-ply greedy (port of solutions/python/bot.py).
// Build: g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o bot_cpp
// The referee provides the legal actions; we only simulate and score them.
#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int N = 49;
constexpr int WIN_SCORE = 1'000'000;
constexpr int PIECE_WEIGHT = 20;

// Center-control weights (doubly symmetric matrix, row 0 = bottom).
constexpr std::array<int, N> WEIGHTS{{
    1, 2, 2, 3, 2, 2, 1,
    2, 3, 4, 4, 4, 3, 2,
    2, 4, 5, 5, 5, 4, 2,
    3, 4, 5, 6, 5, 4, 3,
    2, 4, 5, 5, 5, 4, 2,
    2, 3, 4, 4, 4, 3, 2,
    1, 2, 2, 3, 2, 2, 1,
}};

using Neighbors = std::array<std::vector<int>, N>;

Neighbors build_neighbors() {
    Neighbors nb;
    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    const int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < 7 && ny >= 0 && ny < 7) {
                        nb[y * 7 + x].push_back(ny * 7 + nx);
                    }
                }
            }
        }
    }
    return nb;
}

int clamp7(int v) { return std::clamp(v, 0, 6); }

int char_to_cell(char ch) {
    if (ch == '1') return 2;
    if (ch == '0') return 1;
    if (ch == '#') return 3;
    return 0;
}

struct Parsed {
    bool jump;
    int x0, y0, x1, y1;
};

Parsed parse_action(const std::string& s) {
    if (s.size() < 2) return {false, 0, 0, 0, 0};
    const int x0 = clamp7(s[0] - 'a');
    const int y0 = clamp7(s[1] - '1');
    if (s.size() >= 4) {
        return {true, x0, y0, clamp7(s[2] - 'a'), clamp7(s[3] - '1')};
    }
    return {false, x0, y0, x0, y0};
}

using Board = std::array<int, N>;

Board simulate(const Board& cells, const std::string& action, int me, int opp,
               const Neighbors& nb) {
    Board board = cells;
    const Parsed p = parse_action(action);
    const int dst = p.y1 * 7 + p.x1;
    if (p.jump) board[p.y0 * 7 + p.x0] = 0;  // source vacated by a jump
    board[dst] = me;
    for (int idx : nb[dst]) {
        if (board[idx] == opp) board[idx] = me;
    }
    return board;
}

int evaluate(const Board& board, int me, int opp, const Neighbors& nb) {
    int my_count = 0, opp_count = 0, score = 0;
    for (int i = 0; i < N; ++i) {
        const int cell = board[i];
        if (cell == me) {
            ++my_count;
            int s = WEIGHTS[i];
            for (int idx : nb[i]) if (board[idx] == opp) ++s;
            score += s;
        } else if (cell == opp) {
            ++opp_count;
            int s = WEIGHTS[i];
            for (int idx : nb[i]) if (board[idx] == me) ++s;
            score -= s;
        }
    }
    if (my_count == 0) return -WIN_SCORE;
    if (opp_count == 0) return WIN_SCORE;
    return score + PIECE_WEIGHT * (my_count - opp_count);
}

std::string choose(const Board& cells, int me, const std::vector<std::string>& actions,
                   const Neighbors& nb) {
    const int opp = 3 - me;
    std::string best;
    int best_eval = 0;
    bool have = false;
    for (const std::string& action : actions) {
        if (action.empty()) continue;
        const Board next = simulate(cells, action, me, opp, nb);
        const int val = evaluate(next, me, opp, nb);
        if (!have || val > best_eval || (val == best_eval && action < best)) {
            best_eval = val;
            best = action;
            have = true;
        }
    }
    return have ? best : "random";
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int id;
    if (!(std::cin >> id)) return 0;
    int me = id + 1;
    if (me != 1 && me != 2) me = 1;
    const Neighbors nb = build_neighbors();

    std::string row, last;
    while (true) {
        Board cells{};
        bool complete = true;
        for (int y = 0; y < 7; ++y) {
            if (!(std::cin >> row)) { complete = false; break; }
            for (int x = 0; x < 7; ++x) {
                const char ch = (x < static_cast<int>(row.size())) ? row[x] : '.';
                cells[y * 7 + x] = char_to_cell(ch);
            }
        }
        if (!complete) break;
        if (!(std::cin >> last)) break;  // lastAction
        int count;
        if (!(std::cin >> count)) break;
        if (count < 0) count = 0;
        std::vector<std::string> actions;
        actions.reserve(count);
        std::string action;
        for (int i = 0; i < count; ++i) {
            if (!(std::cin >> action)) break;
            actions.push_back(action);
        }
        std::cout << choose(cells, me, actions, nb) << std::endl;  // flush each turn
    }
    return 0;
}
