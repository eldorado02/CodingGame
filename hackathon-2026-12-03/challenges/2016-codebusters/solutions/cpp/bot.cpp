// CodeBusters — stratégie Wood : un fantôme par buster, bande de capture.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace {
constexpr int W = 16000, H = 9000, STEP = 800;
constexpr long long BUST_MIN2 = 900LL * 900LL;
constexpr long long BUST_MAX2 = 1760LL * 1760LL;

struct Entity {
    int id, x, y, type, state;
};

long long dist2(int ax, int ay, int bx, int by) {
    const long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

std::pair<int, int> step_toward(int x, int y, int tx, int ty) {
    const long long dx = tx - x, dy = ty - y;
    const long long d2 = dx * dx + dy * dy;
    if (d2 == 0) return {x, y};
    const double d = std::sqrt(static_cast<double>(d2));
    const int nx = static_cast<int>(std::floor(x + STEP * dx / d + 0.5));
    const int ny = static_cast<int>(std::floor(y + STEP * dy / d + 0.5));
    return {std::clamp(nx, 0, W), std::clamp(ny, 0, H)};
}
}  // namespace

int main() {
    int per_player, ghost_count, team;
    if (!(std::cin >> per_player >> ghost_count >> team)) return 0;
    const int home_x = team == 0 ? 0 : W;
    const int home_y = team == 0 ? 0 : H;

    for (;;) {
        int n;
        if (!(std::cin >> n)) return 0;
        std::vector<Entity> mine, ghosts;
        for (int i = 0; i < n; ++i) {
            Entity e{};
            int value;
            if (!(std::cin >> e.id >> e.x >> e.y >> e.type >> e.state >> value)) return 0;
            if (e.type == team) mine.push_back(e);
            else if (e.type == -1) ghosts.push_back(e);
        }

        std::vector<bool> reserved(ghosts.size(), false);
        for (const Entity& b : mine) {
            if (b.state == 1) {
                if (dist2(b.x, b.y, home_x, home_y) <= static_cast<long long>(STEP) * STEP) {
                    std::cout << "RELEASE\n";
                } else {
                    const auto [nx, ny] = step_toward(b.x, b.y, home_x, home_y);
                    std::cout << "MOVE " << nx << ' ' << ny << '\n';
                }
                continue;
            }

            int best = -1;
            long long best_d2 = 0;
            for (std::size_t g = 0; g < ghosts.size(); ++g) {
                if (reserved[g]) continue;
                const long long d2 = dist2(b.x, b.y, ghosts[g].x, ghosts[g].y);
                if (best < 0 || d2 < best_d2) { best = static_cast<int>(g); best_d2 = d2; }
            }
            if (best < 0) {
                const auto [nx, ny] = step_toward(b.x, b.y, W / 2, H / 2);
                std::cout << "MOVE " << nx << ' ' << ny << '\n';
                continue;
            }

            const Entity& g = ghosts[best];
            if (best_d2 >= BUST_MIN2 && best_d2 <= BUST_MAX2) {
                reserved[best] = true;
                std::cout << "BUST " << g.id << '\n';
                continue;
            }
            const int tx = best_d2 < BUST_MIN2 ? 2 * b.x - g.x : g.x;
            const int ty = best_d2 < BUST_MIN2 ? 2 * b.y - g.y : g.y;
            const auto [nx, ny] = step_toward(b.x, b.y, tx, ty);
            std::cout << "MOVE " << nx << ' ' << ny << '\n';
        }
        std::cout << std::flush;
    }
}
