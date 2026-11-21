// Back Track King (Wood 1+) — cible la région ennemie la plus instable.
#include <iostream>
#include <string>
#include <vector>

namespace {
bool is_inked(const std::string &s) {
    return !s.empty() && s[0] != '0' && s[0] != 'f' && s[0] != 'F';
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int my_id, width, height;
    if (!(std::cin >> my_id >> width >> height)) return 0;
    int cells = width * height;
    std::vector<int> region_at(cells);
    for (int i = 0; i < cells; ++i) {
        int type;
        std::cin >> region_at[i] >> type;
    }

    int town_count;
    std::cin >> town_count;
    std::vector<bool> town_region(4096, false);
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        std::string desired;
        std::cin >> id >> x >> y >> desired;
        int idx = y * width + x;
        if (idx >= 0 && idx < cells) {
            int rid = region_at[idx];
            if (rid >= 0 && rid < static_cast<int>(town_region.size()))
                town_region[rid] = true;
        }
    }

    int opponent = 1 - my_id;
    int my_score, foe_score;
    while (std::cin >> my_score >> foe_score) {
        int best_region = -1, best_instability = -1;
        for (int i = 0; i < cells; ++i) {
            int owner, instability;
            std::string inked, active;
            std::cin >> owner >> instability >> inked >> active;
            int rid = region_at[i];
            if (owner != opponent || rid < 0 ||
                rid >= static_cast<int>(town_region.size()) ||
                town_region[rid] || is_inked(inked)) {
                continue;
            }
            if (instability > best_instability ||
                (instability == best_instability &&
                 (best_region < 0 || rid < best_region))) {
                best_instability = instability;
                best_region = rid;
            }
        }
        std::cout << (best_region >= 0 ? "DISRUPT " + std::to_string(best_region)
                                       : std::string("WAIT"))
                  << '\n'
                  << std::flush;
    }
    return 0;
}
