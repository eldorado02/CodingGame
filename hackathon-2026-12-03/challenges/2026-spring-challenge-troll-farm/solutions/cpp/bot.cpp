// Troll Farm (Wood) — BFS depuis le troll, cible fruitière puis retour au shack.
#include <array>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace {
constexpr int INF = 1'000'000'000;
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int width, height;
    if (!(std::cin >> width >> height)) return 0;
    std::vector<std::string> grid(height);
    int shack_x = -1, shack_y = -1;
    for (int y = 0; y < height; ++y) {
        std::cin >> grid[y];
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] == '0') {
                shack_x = x;
                shack_y = y;
            }
        }
    }
    auto walkable = [&](int x, int y) {
        // Cell.isWalkable() : seule la case GRASS est praticable.
        return x >= 0 && x < width && y >= 0 && y < height && grid[y][x] == '.';
    };

    while (true) {
        for (int i = 0; i < 12; ++i) {
            int v;
            if (!(std::cin >> v)) return 0;
        }
        int tree_count;
        if (!(std::cin >> tree_count)) return 0;
        std::vector<std::array<int, 3>> trees;  // x, y, fruits
        for (int i = 0; i < tree_count; ++i) {
            std::string type;
            int x, y, size, health, fruits, cooldown;
            std::cin >> type >> x >> y >> size >> health >> fruits >> cooldown;
            trees.push_back({x, y, fruits});
        }
        int troll_count;
        if (!(std::cin >> troll_count)) return 0;
        int id = -1, px = -1, py = -1, carrying = 0;
        for (int i = 0; i < troll_count; ++i) {
            int tid, owner, x, y, speed, capacity, power, chop;
            std::array<int, 6> carry{};
            std::cin >> tid >> owner >> x >> y >> speed >> capacity >> power >> chop;
            for (int &v : carry) std::cin >> v;
            if (owner == 0) {
                id = tid;
                px = x;
                py = y;
                for (int k = 0; k < 4; ++k) carrying += carry[k];
            }
        }
        if (id < 0) {
            std::cout << "WAIT\n" << std::flush;
            continue;
        }

        std::vector<std::vector<int>> dist(height, std::vector<int>(width, -1));
        std::vector<std::vector<std::pair<int, int>>> first(
            height, std::vector<std::pair<int, int>>(width, {-1, -1}));
        std::queue<std::pair<int, int>> q;
        dist[py][px] = 0;
        first[py][px] = {px, py};
        q.push({px, py});
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            for (int k = 0; k < 4; ++k) {
                int nx = x + DX[k], ny = y + DY[k];
                if (!walkable(nx, ny) || dist[ny][nx] >= 0) continue;
                dist[ny][nx] = dist[y][x] + 1;
                first[ny][nx] = dist[y][x] == 0
                                    ? std::make_pair(nx, ny)
                                    : first[y][x];
                q.push({nx, ny});
            }
        }

        auto at = [&](int x, int y) {
            return x >= 0 && x < width && y >= 0 && y < height;
        };
        // Clé commune : (distance, -fruits, y, x). Pour un retour au shack,
        // le nombre de fruits ne joue pas (toujours 0).
        std::tuple<int, int, int, int> best{INF, INF, INF, INF};
        int bx = -1, by = -1;
        bool harvest = false;
        if (carrying > 0) {
            if (std::abs(px - shack_x) + std::abs(py - shack_y) == 1) {
                std::cout << "DROP " << id << '\n' << std::flush;
                continue;
            }
            for (int k = 0; k < 4; ++k) {
                int nx = shack_x + DX[k], ny = shack_y + DY[k];
                if (!at(nx, ny) || dist[ny][nx] < 0) continue;
                auto key = std::make_tuple(dist[ny][nx], 0, ny, nx);
                if (key < best) {
                    best = key;
                    bx = nx;
                    by = ny;
                }
            }
        } else {
            for (const auto &t : trees) {
                if (t[2] <= 0) continue;
                if (t[0] == px && t[1] == py) {
                    harvest = true;
                    break;
                }
                int d = at(t[0], t[1]) ? dist[t[1]][t[0]] : INF;
                if (d < 0) d = INF;
                auto key = std::make_tuple(d, -t[2], t[1], t[0]);
                if (key < best) {
                    best = key;
                    bx = t[0];
                    by = t[1];
                }
            }
        }
        if (harvest) {
            std::cout << "HARVEST " << id << '\n';
        } else if (at(bx, by) && dist[by][bx] >= 0) {
            auto [fx, fy] = first[by][bx];
            std::cout << "MOVE " << id << ' ' << fx << ' ' << fy << '\n';
        } else {
            std::cout << "WAIT\n";
        }
        std::cout.flush();
    }
    return 0;
}
