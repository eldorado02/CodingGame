// Wondev Woman Wood 3 — meilleure action légale : victoire, déni, montée.
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace {
constexpr int MAXS = 7;

std::pair<int, int> delta_of(const std::string& d) {
    if (d == "N") return {0, -1};
    if (d == "NE") return {1, -1};
    if (d == "E") return {1, 0};
    if (d == "SE") return {1, 1};
    if (d == "S") return {0, 1};
    if (d == "SW") return {-1, 1};
    if (d == "W") return {-1, 0};
    return {-1, -1};  // NW
}

int height(const std::array<std::array<int, MAXS>, MAXS>& g,
           int x, int y, int size) {
    if (x < 0 || y < 0 || x >= size || y >= size) return -1;
    return g[y][x];
}
}  // namespace

int main() {
    int size, up;
    if (!(std::cin >> size >> up)) return 0;
    for (;;) {
        std::array<std::array<int, MAXS>, MAXS> grid{};
        for (int y = 0; y < size; ++y) {
            std::string row;
            if (!(std::cin >> row)) return 0;
            for (int x = 0; x < size; ++x)
                grid[y][x] = row[x] == '.' ? -1 : row[x] - '0';
        }
        int ux, uy, ox, oy, n;
        if (!(std::cin >> ux >> uy >> ox >> oy >> n)) return 0;

        long long best_score = -1'000'000'000LL;
        std::string best1, best2;
        for (int i = 0; i < n; ++i) {
            std::string atype, d1s, d2s;
            int index;
            if (!(std::cin >> atype >> index >> d1s >> d2s)) return 0;
            const auto [dx, dy] = delta_of(d1s);
            const auto [bx, by] = delta_of(d2s);
            const int mx = ux + dx, my = uy + dy;
            const int h_dest = height(grid, mx, my, size);
            const int cx = mx + bx, cy = my + by;
            const int h_build = height(grid, cx, cy, size);

            long long score = 0;
            if (h_dest == 3) score += 1'000'000;
            if (h_dest < 0) score -= 100'000;
            else score += 1000LL * h_dest;
            if (h_build == 3) {
                if (ox >= 0 && std::abs(cx - ox) <= 1 && std::abs(cy - oy) <= 1)
                    score += 50'000;
                else
                    score -= 50;
            } else if (h_build == 2) score += 300;
            else if (h_build == 1) score += 100;

            if (score > best_score) {
                best_score = score;
                best1 = d1s;
                best2 = d2s;
            }
        }
        if (best1.empty()) std::cout << "WAIT\n";
        else std::cout << "MOVE&BUILD 0 " << best1 << ' ' << best2 << '\n';
        std::cout << std::flush;
    }
}
