// Olymbits (Wood 1, 4 courses) — énumération des 4^5 séquences d'actions.
// Toutes les machines reçoivent la même action ; on minimise la somme des
// tours restants estimés sur toutes les courses actives.
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kHorizon = 5;
constexpr int kPow4[5] = {1, 4, 16, 64, 256};
constexpr std::array<const char *, 4> kNames{"LEFT", "DOWN", "RIGHT", "UP"};
constexpr std::array<int, 4> kSteps{1, 2, 3, 2};
constexpr std::array<bool, 4> kJump{false, false, false, true};

struct State {
    std::string track;
    int pos, stun, last;
};

void step(const State &s, int *pos, int *stun, int move) {
    /* Certains registres (archerie) sont des coordonnées négatives. */
    if (*pos < 0) *pos = 0;
    if (*stun > 0) {
        --*stun;
        return;
    }
    const int steps = kSteps[move];
    if (kJump[move]) {
        int np = *pos + 2;
        if (np > s.last) np = s.last;
        *stun = (*pos + 2 < s.last && s.track[*pos + 2] == '#') ? 3 : 0;
        *pos = np;
        return;
    }
    for (int k = 1; k <= steps; ++k) {
        if (*pos + k > s.last) {
            *pos = s.last;
            *stun = 0;
            return;
        }
        if (s.track[*pos + k] == '#') {
            *pos += k;
            *stun = 3;
            return;
        }
    }
    *pos += steps;
    *stun = 0;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    int player = 0, games = 0;
    if (!std::getline(std::cin, line)) return 0;
    std::istringstream(line) >> player;
    if (!std::getline(std::cin, line)) return 0;
    std::istringstream(line) >> games;

    while (true) {
        bool alive = true;
        for (int i = 0; i < 3 && alive; ++i)
            alive = static_cast<bool>(std::getline(std::cin, line));
        if (!alive) break;

        std::vector<State> states;
        for (int g = 0; g < games; ++g) {
            if (!std::getline(std::cin, line)) break;
            std::istringstream ss(line);
            std::string gpu;
            std::array<int, 7> reg{};
            if (!(ss >> gpu)) continue;
            for (int i = 0; i < 7; ++i) ss >> reg[i];
            if (gpu == "GAME_OVER") continue;
            states.push_back({gpu, reg[player], reg[3 + player],
                              static_cast<int>(gpu.size()) - 1});
        }

        int best = -1, best_first = 0;
        for (int code = 0; code < 1024; ++code) {
            int total = 0;
            for (const State &s : states) {
                int pos = s.pos, stun = s.stun, done = -1;
                for (int t = 0; t < kHorizon; ++t) {
                    int m = (code / kPow4[kHorizon - 1 - t]) % 4;
                    step(s, &pos, &stun, m);
                    if (pos >= s.last) {
                        done = t + 1;
                        break;
                    }
                }
                if (done < 0) done = kHorizon + (s.last - pos);
                total += done;
            }
            if (best < 0 || total < best) {
                best = total;
                best_first = (code / kPow4[kHorizon - 1]) % 4;
                if (best == 0) break;
            }
        }
        std::cout << kNames[best_first] << '\n' << std::flush;
    }
    return 0;
}
