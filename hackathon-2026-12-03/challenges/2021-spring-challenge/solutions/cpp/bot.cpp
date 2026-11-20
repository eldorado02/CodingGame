// Spring Challenge 2021 (Wood 4) — compléter les meilleurs arbres au plus vite.
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
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
    int n = std::stoi(line);
    std::array<int, 64> richness{};
    for (int i = 0; i < n; ++i) {
        std::getline(std::cin, line);
        auto f = tokens(line);
        if (f.size() >= 2) richness[std::stoi(f[0])] = std::stoi(f[1]);
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        std::getline(std::cin, line);  // nutrients
        std::getline(std::cin, line);
        auto f = tokens(line);
        if (f.size() < 2) break;
        int my_sun = std::stoi(f[0]);
        std::getline(std::cin, line);  // adversaire
        int nt = std::stoi([] { std::string l; std::getline(std::cin, l); return l; }());
        int best_rich = 0, best_idx = -1;
        for (int i = 0; i < nt; ++i) {
            std::getline(std::cin, line);
            auto t = tokens(line);
            if (t.size() < 4) continue;
            int idx = std::stoi(t[0]), size = std::stoi(t[1]), mine = std::stoi(t[2]);
            if (mine == 1 && size == 3) {
                int rch = richness[idx];
                if (best_idx < 0 || rch > best_rich || (rch == best_rich && idx < best_idx)) {
                    best_rich = rch; best_idx = idx;
                }
            }
        }
        int na = std::stoi([] { std::string l; std::getline(std::cin, l); return l; }());
        for (int i = 0; i < na; ++i) std::getline(std::cin, line);
        if (my_sun >= 4 && best_idx >= 0) std::cout << "COMPLETE " << best_idx << '\n';
        else std::cout << "WAIT\n";
        std::cout << std::flush;
    }
    return 0;
}
