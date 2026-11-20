// Spring Challenge 2022 (Wood 4) — défense de base par 3 héros.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr int MAP_W = 17630, MAP_H = 9000;

struct Monster {
    int x, y, nx, ny, hp, threat;
};
struct Hero {
    int id, x, y;
};

double dist(int ax, int ay, int bx, int by) {
    return std::hypot(ax - bx, ay - by);
}

std::vector<std::string> tokens(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string t;
    while (iss >> t) out.push_back(t);
    return out;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    auto head = tokens(line);
    int bx = std::stoi(head[0]), by = std::stoi(head[1]);
    int nh = std::stoi([] { std::string l; std::getline(std::cin, l); return l; }());

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        auto f = tokens(line);
        if (f.size() < 2) break;
        std::getline(std::cin, line);  // adversaire
        int n = std::stoi([] { std::string l; std::getline(std::cin, l); return l; }());
        std::vector<Monster> monsters;
        std::vector<Hero> heroes;
        for (int i = 0; i < n; ++i) {
            std::getline(std::cin, line);
            auto e = tokens(line);
            if (e.size() < 11) continue;
            int type = std::stoi(e[1]);
            int x = std::stoi(e[2]), y = std::stoi(e[3]);
            int hp = std::stoi(e[6]), vx = std::stoi(e[7]), vy = std::stoi(e[8]);
            int near = std::stoi(e[9]), threat = std::stoi(e[10]);
            if (type == 0) {
                Monster m{x, y, 0, 0, hp, threat};
                if (near) {
                    int px = threat == 1 ? bx : MAP_W - bx;
                    int py = threat == 1 ? by : MAP_H - by;
                    double dx = px - x, dy = py - y;
                    double d = std::max(1.0, dist(x, y, px, py));
                    m.nx = x + static_cast<int>(400.0 * dx / d);
                    m.ny = y + static_cast<int>(400.0 * dy / d);
                } else {
                    m.nx = x + vx; m.ny = y + vy;
                }
                if (threat == 1) monsters.push_back(m);
            } else if (type == 1) {
                heroes.push_back({std::stoi(e[0]), x, y});
            }
        }

        std::sort(monsters.begin(), monsters.end(), [&](const Monster &a, const Monster &b) {
            return std::make_tuple(dist(a.nx, a.ny, bx, by), a.hp, a.x, a.y) <
                   std::make_tuple(dist(b.nx, b.ny, bx, by), b.hp, b.x, b.y);
        });

        std::vector<bool> used(monsters.size(), false);
        for (const auto &h : heroes) {
            int best = -1;
            double bd = 0;
            for (size_t i = 0; i < monsters.size(); ++i) {
                if (used[i]) continue;
                double d = dist(h.x, h.y, monsters[i].nx, monsters[i].ny);
                if (best < 0 || d < bd) { best = static_cast<int>(i); bd = d; }
            }
            if (best < 0) { std::cout << "WAIT\n"; continue; }
            used[best] = true;
            int tx = monsters[best].nx, ty = monsters[best].ny;
            if (bd <= 800) { tx = h.x; ty = h.y; }
            std::cout << "MOVE " << tx << ' ' << ty << '\n';
        }
        for (size_t i = heroes.size(); i < static_cast<size_t>(nh); ++i) std::cout << "WAIT\n";
        std::cout << std::flush;
    }
    return 0;
}
