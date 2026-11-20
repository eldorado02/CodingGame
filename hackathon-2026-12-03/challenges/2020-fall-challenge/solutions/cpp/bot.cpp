// Fall Challenge 2020 (Wood 4) — brasser la recette la plus rentable.
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
struct Order {
    int id;
    std::array<int, 4> need;
    int price;
};

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
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        int n = std::stoi(line);
        std::vector<Order> orders;
        for (int i = 0; i < n; ++i) {
            std::getline(std::cin, line);
            auto f = tokens(line);
            if (f.size() < 7 || f[1] != "BREW") continue;
            Order o;
            o.id = std::stoi(f[0]);
            for (int j = 0; j < 4; ++j) o.need[j] = -std::stoi(f[2 + j]);
            o.price = std::stoi(f[6]);
            orders.push_back(o);
        }
        std::getline(std::cin, line);
        auto mine = tokens(line);
        if (mine.size() < 5) break;
        std::array<int, 4> inv{};
        for (int j = 0; j < 4; ++j) inv[j] = std::stoi(mine[j]);
        std::getline(std::cin, line);  // adversaire

        int best_id = -1, best_price = -1, best_sum = 0;
        for (const auto &o : orders) {
            int sum = 0;
            bool ok = true;
            for (int j = 0; j < 4; ++j) {
                if (o.need[j] > inv[j]) { ok = false; break; }
                sum += o.need[j];
            }
            if (!ok) continue;
            if (best_id < 0 || o.price > best_price ||
                (o.price == best_price && sum < best_sum) ||
                (o.price == best_price && sum == best_sum && o.id < best_id)) {
                best_id = o.id; best_price = o.price; best_sum = sum;
            }
        }
        if (best_id >= 0) std::cout << "BREW " << best_id << '\n';
        else std::cout << "WAIT\n";
        std::cout << std::flush;
    }
    return 0;
}
