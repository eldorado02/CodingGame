// Coders of the Caribbean Wood 3 — viser le baril le plus proche.
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    int ships;
    while (std::cin >> ships) {
        int n;
        if (!(std::cin >> n)) return 0;
        int sx = -1, sy = -1;
        std::vector<std::pair<int, int>> barrels;
        for (int i = 0; i < n; ++i) {
            int id, x, y, a1, a2, a3, a4;
            std::string kind;
            if (!(std::cin >> id >> kind >> x >> y >> a1 >> a2 >> a3 >> a4)) return 0;
            if (kind == "SHIP" && a4 == 1) {
                sx = x;
                sy = y;
            } else if (kind == "BARREL") {
                barrels.emplace_back(x, y);
            }
        }
        if (sx < 0 || barrels.empty()) {
            std::cout << "WAIT\n";
        } else {
            const auto best = *std::min_element(barrels.begin(), barrels.end(),
                [sx, sy](const auto& a, const auto& b) {
                    const long long da = 1LL * (a.first - sx) * (a.first - sx)
                                       + 1LL * (a.second - sy) * (a.second - sy);
                    const long long db = 1LL * (b.first - sx) * (b.first - sx)
                                       + 1LL * (b.second - sy) * (b.second - sy);
                    return da < db;
                });
            std::cout << "MOVE " << best.first << ' ' << best.second << '\n';
        }
        std::cout << std::flush;
    }
    return 0;
}
