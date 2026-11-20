// Fantastic Bits Wood 2 — attaquant + défenseur, THROW au but après grab.
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int W = 16000, H = 7500, GOAL_Y = 3750;

struct Entity {
    int id, x, y, vx, vy, state;
};

long long d2(int ax, int ay, int bx, int by) {
    const long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}
int clampi(int v, int lo, int hi) { return std::clamp(v, lo, hi); }
}  // namespace

int main() {
    int team;
    if (!(std::cin >> team)) return 0;
    const int goal_x = team == 0 ? W : 0;
    const int own_x = team == 0 ? 0 : W;

    for (;;) {
        int my_score, my_magic, opp_score, opp_magic, n;
        if (!(std::cin >> my_score >> my_magic >> opp_score >> opp_magic >> n)) return 0;
        std::vector<Entity> wizards, snaffles;
        for (int i = 0; i < n; ++i) {
            Entity e{};
            std::string kind;
            if (!(std::cin >> e.id >> kind >> e.x >> e.y >> e.vx >> e.vy >> e.state)) return 0;
            if (kind == "WIZARD") wizards.push_back(e);
            else if (kind == "SNAFFLE") snaffles.push_back(e);
        }

        std::vector<bool> chosen(snaffles.size(), false);
        for (std::size_t i = 0; i < wizards.size(); ++i) {
            const Entity& w = wizards[i];
            if (w.state == 1) {
                std::cout << "THROW " << goal_x << ' ' << GOAL_Y << " 500\n";
                continue;
            }

            int best = -1;
            if (i == 0) {
                for (std::size_t s = 0; s < snaffles.size(); ++s) {
                    if (chosen[s]) continue;
                    if (best < 0 || d2(w.x, w.y, snaffles[s].x, snaffles[s].y) <
                                    d2(w.x, w.y, snaffles[best].x, snaffles[best].y)) {
                        best = static_cast<int>(s);
                    }
                }
            } else {
                for (std::size_t s = 0; s < snaffles.size(); ++s) {
                    if (chosen[s]) continue;
                    const bool in_own = (snaffles[s].x < W / 2) == (team == 0);
                    if (!in_own) continue;
                    if (best < 0 || d2(own_x, GOAL_Y, snaffles[s].x, snaffles[s].y) <
                                    d2(own_x, GOAL_Y, snaffles[best].x, snaffles[best].y)) {
                        best = static_cast<int>(s);
                    }
                }
                if (best < 0) {
                    for (std::size_t s = 0; s < snaffles.size(); ++s) {
                        if (chosen[s]) continue;
                        if (best < 0 || d2(w.x, w.y, snaffles[s].x, snaffles[s].y) <
                                        d2(w.x, w.y, snaffles[best].x, snaffles[best].y)) {
                            best = static_cast<int>(s);
                        }
                    }
                }
            }

            if (best < 0) {
                std::cout << "MOVE " << goal_x << ' ' << GOAL_Y << " 150\n";
                continue;
            }
            chosen[best] = true;
            const int tx = clampi(snaffles[best].x + 3 * snaffles[best].vx, 0, W);
            const int ty = clampi(snaffles[best].y + 3 * snaffles[best].vy, 0, H);
            const int thrust = d2(w.x, w.y, tx, ty) > 1200LL * 1200 ? 150 : 60;
            std::cout << "MOVE " << tx << ' ' << ty << ' ' << thrust << '\n';
        }
        std::cout << std::flush;
    }
}
