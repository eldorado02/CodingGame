// Back Track King (Wood) — une seule connexion suffit : on demande à
// AUTOPLACE de construire le chemin entre le couple de villes le plus proche.
#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
int distance(int ax, int ay, int bx, int by) {
    return std::abs(ax - bx) + std::abs(ay - by);
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int my_id, width, height;
    if (!(std::cin >> my_id >> width >> height)) return 0;
    for (int i = 0; i < width * height; ++i) {
        int region, type;
        std::cin >> region >> type;
    }

    int town_count;
    std::cin >> town_count;
    std::unordered_map<int, std::pair<int, int>> towns;
    std::vector<std::pair<int, int>> pairs;  // demandeur, cible
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        std::string desired;
        std::cin >> id >> x >> y >> desired;
        towns[id] = {x, y};
        if (desired == "x") continue;
        size_t start = 0;
        while (start <= desired.size()) {
            size_t comma = desired.find(',', start);
            std::string token = desired.substr(
                start, comma == std::string::npos ? std::string::npos : comma - start);
            pairs.emplace_back(id, std::stoi(token));
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
    }

    auto key = [&](const std::pair<int, int> &p) {
        const auto &a = towns[p.first];
        const auto &b = towns[p.second];
        return std::make_tuple(distance(a.first, a.second, b.first, b.second),
                               p.first, p.second);
    };
    auto best = pairs.front();
    for (const auto &p : pairs) {
        if (key(p) < key(best)) best = p;
    }
    const auto &from = towns[best.first];
    const auto &to = towns[best.second];

    int my_score, foe_score;
    while (std::cin >> my_score >> foe_score) {
        for (int i = 0; i < width * height; ++i) {
            int owner, instability;
            std::string inked, active;
            std::cin >> owner >> instability >> inked >> active;
        }
        std::cout << "AUTOPLACE " << from.first << ' ' << from.second << ' '
                  << to.first << ' ' << to.second << '\n'
                  << std::flush;
    }
    return 0;
}
