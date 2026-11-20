// Hypersonic Wood — BFS vers les boîtes, bombes sur cibles rentables.
#include <array>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

namespace {
constexpr int W = 13, H = 11, RANGE = 3;
constexpr int DX[4] = {1, -1, 0, 0};
constexpr int DY[4] = {0, 0, 1, -1};
using Grid = std::array<std::string, H>;
using Mask = std::array<std::array<bool, W>, H>;

int blast_boxes(const Grid& grid, int x, int y) {
    int total = 0;
    for (int d = 0; d < 4; ++d) {
        for (int s = 1; s <= RANGE; ++s) {
            const int nx = x + DX[d] * s, ny = y + DY[d] * s;
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) break;
            if (grid[ny][nx] == '0') { ++total; break; }
        }
    }
    return total;
}

// mode 0=boîte, 1=centre, 2=fuite hors portée 3.
bool bfs_next(const Grid& grid, int sx, int sy, const Mask& blocked, int mode, int& out_x, int& out_y) {
    std::array<int, H * W> parent;
    parent.fill(-1);
    std::queue<std::pair<int, int>> q;
    q.push({sx, sy});
    std::array<std::array<bool, W>, H> seen{};
    seen[sy][sx] = true;
    while (!q.empty()) {
        const auto [x, y] = q.front();
        q.pop();
        for (int d = 0; d < 4; ++d) {
            const int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (seen[ny][nx] || blocked[ny][nx] || grid[ny][nx] == '0') continue;
            seen[ny][nx] = true;
            parent[ny * W + nx] = y * W + x;
            bool done = false;
            if (mode == 0) {
                for (int k = 0; k < 4; ++k) {
                    const int ax = nx + DX[k], ay = ny + DY[k];
                    if (ax >= 0 && ax < W && ay >= 0 && ay < H && grid[ay][ax] == '0') { done = true; break; }
                }
            } else if (mode == 1) {
                done = nx == W / 2 && ny == H / 2;
            } else {
                done = std::abs(nx - sx) + std::abs(ny - sy) > 3;
            }
            if (done) {
                int idx = ny * W + nx;
                while (parent[idx] != sy * W + sx && parent[idx] != -1) idx = parent[idx];
                out_x = idx % W;
                out_y = idx / W;
                return true;
            }
            q.push({nx, ny});
        }
    }
    return false;
}
}  // namespace

int main() {
    int width, height, my_id;
    if (!(std::cin >> width >> height >> my_id)) return 0;
    for (;;) {
        Grid grid{};
        for (int r = 0; r < H; ++r) std::cin >> grid[r];
        int n;
        if (!(std::cin >> n)) return 0;
        int mx = -1, my = -1, bombs = 0;
        Mask blocked{};
        for (int i = 0; i < n; ++i) {
            int kind, owner, x, y, p1, p2;
            std::cin >> kind >> owner >> x >> y >> p1 >> p2;
            if (kind == 0 && owner == my_id) { mx = x; my = y; bombs = p1; }
            else if (kind == 1 && x >= 0 && x < W && y >= 0 && y < H) blocked[y][x] = true;
        }
        if (mx < 0) {
            std::cout << "MOVE " << W / 2 << ' ' << H / 2 << '\n' << std::flush;
            continue;
        }
        int nx = mx, ny = my;
        if (bombs > 0 && blast_boxes(grid, mx, my) > 0) {
            auto b2 = blocked;
            b2[my][mx] = true;
            if (bfs_next(grid, mx, my, b2, 2, nx, ny)) std::cout << "BOMB " << nx << ' ' << ny << '\n';
            else std::cout << "BOMB " << mx << ' ' << my << '\n';
        } else {
            if (!bfs_next(grid, mx, my, blocked, 0, nx, ny) &&
                !bfs_next(grid, mx, my, blocked, 1, nx, ny)) { nx = W / 2; ny = H / 2; }
            std::cout << "MOVE " << nx << ' ' << ny << '\n';
        }
        std::cout << std::flush;
    }
}
