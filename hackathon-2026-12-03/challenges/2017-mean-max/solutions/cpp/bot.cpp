// Mean Max Wood 3 — simulation 1 pas des actions, reste dans les épaves.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace {
struct Wreck { double x, y; int r, water; };
struct Candidate { bool has_target; double tx, ty; int power; };

int round_away(double z) {
    return z >= 0 ? static_cast<int>(std::floor(z + 0.5))
                  : -static_cast<int>(std::floor(-z + 0.5));
}

void simulate(double x, double y, double vx, double vy, const Candidate& c,
              int& nx, int& ny, int& nvx, int& nvy) {
    double ax = 0.0, ay = 0.0;
    if (c.has_target && c.power > 0) {
        const double dx = c.tx - x, dy = c.ty - y;
        const double dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 1e-9) {
            const double coef = 2.0 * c.power / dist;
            ax = dx * coef;
            ay = dy * coef;
        }
    }
    const double sfx = vx + ax, sfy = vy + ay;
    nx = round_away(x + sfx);
    ny = round_away(y + sfy);
    nvx = round_away(sfx * 0.8);
    nvy = round_away(sfy * 0.8);
}

double evaluate(int nx, int ny, int nvx, int nvy, const std::vector<Wreck>& w) {
    int inside = 0, water = 0;
    double closest = 1e18;
    for (const auto& k : w) {
        const double dx = nx - k.x, dy = ny - k.y;
        const double d = std::sqrt(dx * dx + dy * dy);
        if (d <= k.r) { ++inside; water += k.water; }
        closest = std::min(closest, d);
    }
    const double speed = std::sqrt(1.0 * nvx * nvx + 1.0 * nvy * nvy);
    return inside * 1'000'000.0 + water * 10.0 - closest - speed;
}
}  // namespace

int main() {
    for (;;) {
        double ignored;
        bool ok = true;
        for (int i = 0; i < 6; ++i) ok &= static_cast<bool>(std::cin >> ignored);  // scores + rages
        int n;
        if (!ok || !(std::cin >> n)) return 0;

        double rx = 0, ry = 0, rvx = 0, rvy = 0;
        bool have = false;
        std::vector<Wreck> wrecks;
        for (int i = 0; i < n; ++i) {
            int id, type, player, radius, x, y, vx, vy, extra, extra2;
            double mass;
            if (!(std::cin >> id >> type >> player >> mass >> radius >> x >> y
                          >> vx >> vy >> extra >> extra2)) return 0;
            if (type == 0 && player == 0) {
                rx = x; ry = y; rvx = vx; rvy = vy; have = true;
            } else if (type == 4) {
                wrecks.push_back({static_cast<double>(x), static_cast<double>(y), radius, extra});
            }
        }
        if (!have) {
            std::cout << "WAIT\nWAIT\nWAIT\n" << std::flush;
            continue;
        }

        std::vector<Candidate> cands{{false, 0, 0, 0}};  // WAIT
        static const int powers[] = {0, 100, 200, 300};
        for (const auto& k : wrecks)
            for (int p : powers)
                cands.push_back({true, k.x, k.y, p});
        if (rvx != 0 || rvy != 0) {
            cands.push_back({true, rx - rvx, ry - rvy, 150});
            cands.push_back({true, rx - rvx, ry - rvy, 300});
        }

        double best = -1e18;
        bool best_target = false;
        int best_tx = 0, best_ty = 0, best_power = 0;
        for (const auto& c : cands) {
            int nx, ny, nvx, nvy;
            simulate(rx, ry, rvx, rvy, c, nx, ny, nvx, nvy);
            const double s = evaluate(nx, ny, nvx, nvy, wrecks);
            if (s > best) {
                best = s;
                best_target = c.has_target;
                best_tx = static_cast<int>(c.tx);
                best_ty = static_cast<int>(c.ty);
                best_power = c.power;
            }
        }
        if (!best_target || best_power == 0) std::cout << "WAIT\n";
        else std::cout << best_tx << ' ' << best_ty << ' ' << best_power << '\n';
        std::cout << "WAIT\nWAIT\n" << std::flush;
    }
}
