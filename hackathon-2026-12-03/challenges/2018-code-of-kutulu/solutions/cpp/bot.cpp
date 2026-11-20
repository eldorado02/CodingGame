// Code of Kutulu Wood 3 — réassurance prioritaire, fuite des wanderers.
#include <climits>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int DX[5] = {0, 0, 0, -1, 1};
constexpr int DY[5] = {0, -1, 1, 0, 0};
struct Point { int x, y; };
}  // namespace

int main() {
    int w, h;
    if (!(std::cin >> w >> h)) return 0;
    std::vector<std::string> grid(h);
    for (auto& row : grid) std::cin >> row;
    int a, b, c, d;
    std::cin >> a >> b >> c >> d;

    for (;;) {
        int ec;
        if (!(std::cin >> ec)) return 0;
        int mx = 0, my = 0;
        std::vector<Point> others, wanderers;
        for (int i = 0; i < ec; ++i) {
            std::string type;
            int id, x, y, p0, p1, p2;
            if (!(std::cin >> type >> id >> x >> y >> p0 >> p1 >> p2)) return 0;
            if (i == 0) {
                mx = x; my = y;
            } else if (type == "EXPLORER") {
                if (p0 > 0) others.push_back({x, y});
            } else if (type == "WANDERER" && p1 == 1) {
                wanderers.push_back({x, y});
            }
        }

        long long best = LLONG_MIN;
        int bx = mx, by = my;
        for (int k = 0; k < 5; ++k) {
            const int px = mx + DX[k], py = my + DY[k];
            if (px < 0 || px >= w || py < 0 || py >= h || grid[py][px] == '#') continue;
            bool reassured = false;
            int de = 100;
            for (const auto& o : others) {
                const int dd = std::abs(px - o.x) + std::abs(py - o.y);
                if (dd <= 2) reassured = true;
                if (dd < de) de = dd;
            }
            int dw = 100;
            for (const auto& t : wanderers) {
                const int dd = std::abs(px - t.x) + std::abs(py - t.y);
                if (dd < dw) dw = dd;
            }
            const long long score = (reassured ? 100000LL : 0) + 100LL * dw - de;
            if (score > best) { best = score; bx = px; by = py; }
        }
        if (bx == mx && by == my) std::cout << "WAIT\n";
        else std::cout << "MOVE " << bx << ' ' << by << '\n';
        std::cout.flush();
    }
    return 0;
}
