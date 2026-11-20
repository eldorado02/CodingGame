// Green Circle (Wood 4) — releases utiles et bureaux utiles.
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
using Hand = std::array<int, 10>;
using App = std::array<int, 8>;

std::vector<std::string> tokens(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string t;
    while (iss >> t) out.push_back(t);
    return out;
}

int good_score(const Hand &hand, const App &app) {
    int covered = 0, total = 0;
    for (int i = 0; i < 8; ++i) {
        covered += std::min(app[i], 2 * hand[i]);
        total += app[i];
    }
    return covered + std::min(hand[8], total - covered);
}

int best_app(const Hand &hand, const std::vector<int> &ids,
             const std::vector<std::pair<int, App>> &apps) {
    int best_id = -1, best_score = -1;
    for (int aid : ids) {
        auto it = std::find_if(apps.begin(), apps.end(),
                               [aid](const auto &a) { return a.first == aid; });
        if (it == apps.end()) continue;
        int score = good_score(hand, it->second);
        if (score > best_score || (score == best_score && aid < best_id)) {
            best_score = score;
            best_id = aid;
        }
    }
    return best_id;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        auto phase_tokens = tokens(line);
        std::string phase = phase_tokens.empty() ? "" : phase_tokens[0];

        int app_count = 0;
        std::getline(std::cin, line);
        app_count = std::stoi(line);
        std::vector<std::pair<int, App>> apps;
        for (int i = 0; i < app_count; ++i) {
            std::getline(std::cin, line);
            auto f = tokens(line);
            if (f.size() < 10) continue;
            App req{};
            for (int j = 0; j < 8; ++j) req[j] = std::stoi(f[2 + j]);
            apps.emplace_back(std::stoi(f[1]), req);
        }

        std::getline(std::cin, line);  // nous
        std::getline(std::cin, line);  // adversaire

        Hand hand{};
        int loc_count = 0;
        std::getline(std::cin, line);
        loc_count = std::stoi(line);
        for (int i = 0; i < loc_count; ++i) {
            std::getline(std::cin, line);
            auto f = tokens(line);
            if (f.size() < 11 || f[0] != "HAND") continue;
            for (int j = 0; j < 10; ++j) hand[j] = std::stoi(f[1 + j]);
        }

        int move_count = 0;
        std::getline(std::cin, line);
        move_count = std::stoi(line);
        std::vector<std::string> moves;
        for (int i = 0; i < move_count; ++i) {
            std::getline(std::cin, line);
            moves.push_back(line);
        }

        if (phase == "MOVE") {
            std::vector<int> all_ids;
            for (const auto &a : apps) all_ids.push_back(a.first);
            int chosen = -1, best_score = -1;
            for (const auto &m : moves) {
                auto f = tokens(m);
                if (f.size() < 2 || f[0] != "MOVE") continue;
                int dest = std::stoi(f[1]);
                Hand h2 = hand;
                h2[dest] += 1;
                int aid = best_app(h2, all_ids, apps);
                int score = 0;
                if (aid >= 0) {
                    auto it = std::find_if(apps.begin(), apps.end(),
                                           [aid](const auto &a) { return a.first == aid; });
                    score = good_score(h2, it->second);
                }
                if (score > best_score || (score == best_score && dest < chosen)) {
                    best_score = score;
                    chosen = dest;
                }
            }
            if (chosen >= 0) std::cout << "MOVE " << chosen << '\n';
            else std::cout << (moves.empty() ? "WAIT" : moves[0]) << '\n';
        } else if (phase == "RELEASE") {
            std::vector<int> ids;
            for (const auto &m : moves) {
                auto f = tokens(m);
                if (f.size() >= 2 && f[0] == "RELEASE") ids.push_back(std::stoi(f[1]));
            }
            int aid = best_app(hand, ids, apps);
            if (aid >= 0) std::cout << "RELEASE " << aid << '\n';
            else std::cout << "WAIT\n";
        } else {
            std::cout << (moves.empty() ? "WAIT" : moves[0]) << '\n';
        }
        std::cout << std::flush;
    }
    return 0;
}
