// Elemental Wars (Wood) — BFS multi-source : poursuivre la proie, fuir le prédateur.
#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr int DX[4] = {-1, 0, 1, 0};
constexpr int DY[4] = {0, 1, 0, -1};

char prey_of(char t) { return t == 'W' ? 'F' : (t == 'F' ? 'P' : 'W'); }
char pred_of(char t) { return t == 'W' ? 'P' : (t == 'F' ? 'W' : 'F'); }

using Dist = std::vector<std::vector<int>>;

Dist distances(const std::vector<std::pair<int, int>> &sources,
               const std::vector<std::string> &grid, int size) {
    Dist dist(size, std::vector<int>(size, -1));
    std::queue<std::pair<int, int>> q;
    for (const auto &s : sources) {
        if (s.first >= 0 && s.first < size && s.second >= 0 && s.second < size
            && dist[s.second][s.first] < 0) {
            dist[s.second][s.first] = 0;
            q.push(s);
        }
    }
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx >= 0 && nx < size && ny >= 0 && ny < size
                && grid[ny][nx] != '#' && dist[ny][nx] < 0) {
                dist[ny][nx] = dist[y][x] + 1;
                q.push({nx, ny});
            }
        }
    }
    return dist;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string tribe;
    if (!(std::cin >> tribe)) return 0;
    int size;
    if (!(std::cin >> size)) return 0;
    std::vector<std::string> grid(size);
    for (auto &row : grid) std::cin >> row;

    while (true) {
        int sw, sf, sp, count;
        if (!(std::cin >> sw >> sf >> sp)) return 0;  // scores (non utilisés)
        if (!(std::cin >> count)) return 0;
        struct Unit { int id, x, y; };
        std::vector<Unit> mine;
        std::vector<std::pair<int, int>> prey, predator;
        for (int i = 0; i < count; ++i) {
            int id, x, y, prisoner;
            char t;
            if (!(std::cin >> id >> t >> x >> y >> prisoner)) return 0;
            if (prisoner) continue;
            if (t == tribe[0]) mine.push_back({id, x, y});
            else if (t == prey_of(tribe[0])) prey.push_back({x, y});
            else if (t == pred_of(tribe[0])) predator.push_back({x, y});
        }
        Dist dp = distances(prey, grid, size);
        Dist dd = distances(predator, grid, size);
        std::vector<std::string> actions;
        for (const auto &u : mine) {
            using Key = std::tuple<int, int, int, int>;
            Key bestKey = std::make_tuple(-1000000000, 0, 0, 0);
            int bo = 0, bx = u.x, by = u.y;
            for (int order = 0; order < 5; ++order) {
                int dx = order == 0 ? 0 : DX[order - 1];
                int dy = order == 0 ? 0 : DY[order - 1];
                int nx = u.x + dx, ny = u.y + dy;
                if (nx < 0 || nx >= size || ny < 0 || ny >= size) continue;
                if (order != 0 && grid[ny][nx] == '#') continue;
                int score = 0;
                for (const auto &p : prey)
                    if (p.first == nx && p.second == ny) score += 1000;
                if (dp[ny][nx] >= 0) score -= 2 * dp[ny][nx];
                if (dd[ny][nx] >= 0) {
                    score += 3 * dd[ny][nx];
                    if (dd[ny][nx] <= 1) score -= 50;
                }
                Key key = std::make_tuple(score, -order, -ny, -nx);
                if (key > bestKey) {
                    bestKey = key; bo = order; bx = nx; by = ny;
                }
            }
            if (bo == 0) {
                actions.push_back(std::to_string(u.id) + " WAIT");
            } else {
                actions.push_back(std::to_string(u.id) + " MOVE " +
                                  std::to_string(bx) + " " + std::to_string(by));
            }
        }
        std::string out;
        for (size_t i = 0; i < actions.size(); ++i) {
            if (i) out += ';';
            out += actions[i];
        }
        std::cout << (out.empty() ? "WAIT" : out) << '\n' << std::flush;
    }
    return 0;
}
