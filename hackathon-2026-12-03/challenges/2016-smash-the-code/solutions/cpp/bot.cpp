// Smash The Code — solution C++20 (Wood league).
#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr int W = 6;
constexpr int H = 12;
using Grid = std::array<std::array<char, W>, H>;

constexpr int dr[4] = {1, -1, 0, 0};
constexpr int dc[4] = {0, 0, 1, -1};

struct Outcome {
    int cleared = 0;
    int chains = 0;
};

bool drop(Grid& g, int col, char top, char bottom) {
    int row = 0;
    while (row < H && g[row][col] == '.') ++row;
    if (row < 2) return false;
    g[row - 2][col] = top;
    g[row - 1][col] = bottom;
    return true;
}

void gravity(Grid& g) {
    for (int col = 0; col < W; ++col) {
        std::vector<char> cells;
        for (int row = H - 1; row >= 0; --row) {
            if (g[row][col] != '.') cells.push_back(g[row][col]);
        }
        int k = 0;
        for (int row = H - 1; row >= 0 && k < static_cast<int>(cells.size()); --row) {
            g[row][col] = cells[k++];
        }
        for (int row = H - 1 - static_cast<int>(cells.size()); row >= 0; --row) {
            g[row][col] = '.';
        }
    }
}

Outcome resolve(Grid& g) {
    Outcome out;
    for (;;) {
        std::array<std::array<bool, W>, H> visited{};
        std::array<std::array<bool, W>, H> to_clear{};
        bool found = false;

        for (int row = 0; row < H; ++row) {
            for (int col = 0; col < W; ++col) {
                const char colour = g[row][col];
                if (colour == '.' || colour == '0' || visited[row][col]) continue;

                std::queue<std::pair<int, int>> q;
                std::vector<std::pair<int, int>> group;
                q.emplace(row, col);
                visited[row][col] = true;
                while (!q.empty()) {
                    auto [r, c] = q.front();
                    q.pop();
                    group.emplace_back(r, c);
                    for (int d = 0; d < 4; ++d) {
                        const int nr = r + dr[d], nc = c + dc[d];
                        if (nr >= 0 && nr < H && nc >= 0 && nc < W &&
                            !visited[nr][nc] && g[nr][nc] == colour) {
                            visited[nr][nc] = true;
                            q.emplace(nr, nc);
                        }
                    }
                }
                if (group.size() >= 4) {
                    found = true;
                    for (auto [r, c] : group) to_clear[r][c] = true;
                }
            }
        }
        if (!found) break;

        for (int row = 0; row < H; ++row) {
            for (int col = 0; col < W; ++col) {
                if (g[row][col] != '0') continue;
                for (int d = 0; d < 4; ++d) {
                    const int nr = row + dr[d], nc = col + dc[d];
                    if (nr >= 0 && nr < H && nc >= 0 && nc < W && to_clear[nr][nc]) {
                        to_clear[row][col] = true;
                        break;
                    }
                }
            }
        }
        for (int row = 0; row < H; ++row) {
            for (int col = 0; col < W; ++col) {
                if (to_clear[row][col]) {
                    g[row][col] = '.';
                    ++out.cleared;
                }
            }
        }
        ++out.chains;
        gravity(g);
    }
    return out;
}

int height(const Grid& g) {
    int best = 0;
    for (int col = 0; col < W; ++col) {
        int h = 0;
        for (int row = 0; row < H; ++row) if (g[row][col] != '.') ++h;
        best = std::max(best, h);
    }
    return best;
}

int evaluate(const Grid& g, int col, char a, char b) {
    Grid trial = g;
    if (!drop(trial, col, a, b)) return std::numeric_limits<int>::min();
    const Outcome out = resolve(trial);
    return out.cleared * 100 + out.chains * 50 - height(trial) * 2;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    for (;;) { // une entree complete = un tour, puis on rejoue
        // Nombre de paires variable : on lit jusqu'a la ligne de score (1 entier).
        char first_a = 0, first_b = 0;
        while (std::getline(std::cin, line)) {
            std::istringstream iss(line);
            int a = 0, b = 0;
            if (iss >> a >> b) {
                if (first_a == 0) {
                    first_a = static_cast<char>('0' + a);
                    first_b = static_cast<char>('0' + b);
                }
                continue;
            }
            break; // ligne de score
        }
        if (first_a == 0) return 0;

        int score = 0;
        Grid grid{};
        for (int row = 0; row < H; ++row) {
            std::cin >> line;
            for (int col = 0; col < W; ++col) grid[row][col] = line[col];
        }
        std::cin >> score;
        for (int row = 0; row < H; ++row) {
            std::cin >> line;  // grille adverse, inutile en Wood
        }

        int best_col = 0;
        int best_score = std::numeric_limits<int>::min();
        for (int col = 0; col < W; ++col) {
            const int s = evaluate(grid, col, first_a, first_b);
            if (s > best_score) {
                best_score = s;
                best_col = col;
            }
        }
        std::cout << best_col << '\n' << std::flush;
    }
}
