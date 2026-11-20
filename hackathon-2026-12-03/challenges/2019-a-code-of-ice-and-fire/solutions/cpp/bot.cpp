// A Code of Ice and Fire (Wood 3) — rush du QG adverse.
#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr int W = 12, H = 12, INF = 1000000;
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};

struct Unit { int id, x, y; };
struct Candidate { int d, x, y; };
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    const int mines = std::stoi(line);
    for (int i = 0; i < mines; ++i) std::getline(std::cin, line);

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        const int gold = std::stoi(line);
        std::getline(std::cin, line);  // income
        std::getline(std::cin, line);  // opponent gold
        std::getline(std::cin, line);  // opponent income

        std::vector<std::string> grid(H);
        for (int y = 0; y < H; ++y) std::getline(std::cin, grid[y]);

        int my_hx = -1, my_hy = -1, enemy_hx = -1, enemy_hy = -1;
        int nb;
        std::getline(std::cin, line);
        std::stringstream(line) >> nb;
        for (int i = 0; i < nb; ++i) {
            std::getline(std::cin, line);
            int owner, kind, x, y;
            std::stringstream(line) >> owner >> kind >> x >> y;
            if (kind == 0) {
                if (owner == 0) { my_hx = x; my_hy = y; }
                else { enemy_hx = x; enemy_hy = y; }
            }
        }

        int nu;
        std::getline(std::cin, line);
        std::stringstream(line) >> nu;
        std::vector<Unit> units;
        for (int i = 0; i < nu; ++i) {
            std::getline(std::cin, line);
            int owner, id, level, x, y;
            std::stringstream(line) >> owner >> id >> level >> x >> y;
            if (owner == 0) units.push_back({id, x, y});
        }

        if (enemy_hx < 0) {
            std::cout << "WAIT\n" << std::flush;
            continue;
        }

        std::array<std::array<int, W>, H> dist;
        for (auto &row : dist) row.fill(INF);
        std::queue<std::pair<int, int>> q;
        dist[enemy_hy][enemy_hx] = 0;
        q.push({enemy_hx, enemy_hy});
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            const int nd = dist[y][x] + 1;
            for (int k = 0; k < 4; ++k) {
                const int nx = x + DX[k], ny = y + DY[k];
                if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                if (grid[ny][nx] == '#') continue;
                if (dist[ny][nx] != INF) continue;
                dist[ny][nx] = nd;
                q.push({nx, ny});
            }
        }

        bool first = true;
        for (const auto &u : units) {
            if (!first) std::cout << ';';
            std::cout << "MOVE " << u.id << ' ' << enemy_hx << ' ' << enemy_hy;
            first = false;
        }

        std::vector<Candidate> cand;
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                if (grid[y][x] == '#') continue;
                bool playable = grid[y][x] == 'O';
                for (int k = 0; k < 4 && !playable; ++k) {
                    const int nx = x + DX[k], ny = y + DY[k];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (grid[ny][nx] == 'O') playable = true;
                }
                if (!playable) continue;
                if (my_hx >= 0 && x == my_hx && y == my_hy) continue;
                if (dist[y][x] == INF) continue;
                cand.push_back({dist[y][x], x, y});
            }
        }
        std::sort(cand.begin(), cand.end(), [](const Candidate &a, const Candidate &b) {
            return std::tie(a.d, a.x, a.y) < std::tie(b.d, b.x, b.y);
        });

        const int ntrain = gold >= 10 ? gold / 10 : 0;
        for (int i = 0; i < ntrain && i < static_cast<int>(cand.size()); ++i) {
            if (!first) std::cout << ';';
            std::cout << "TRAIN 1 " << cand[i].x << ' ' << cand[i].y;
            first = false;
        }
        if (first) std::cout << "WAIT";
        std::cout << '\n' << std::flush;
    }
    return 0;
}
