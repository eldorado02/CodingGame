// Olymbits — Hurdle Race : BFS, on écarte tout déplacement heurtant une haie.
#include <array>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr std::array<const char *, 4> kNames{"LEFT", "DOWN", "RIGHT", "UP"};
constexpr std::array<int, 4> kSteps{1, 2, 3, 2};
constexpr std::array<bool, 4> kJump{false, false, false, true};

int move_pos(const std::string &track, int pos, int steps, bool jump) {
    const int last = static_cast<int>(track.size()) - 1;
    int np = pos;
    for (int k = 0; k < steps; ++k) {
        if (np < last) {
            ++np;
        }
        if (track[np] == '#' && !jump) {
            return -1;
        }
        if (np == last) {
            break;
        }
        jump = false;  // UP n'ignore que la première case.
    }
    return np;
}

const char *best_action(const std::string &track, int pos) {
    const int last = static_cast<int>(track.size()) - 1;
    if (pos >= last) {
        return "LEFT";
    }
    std::vector<int> parent(last + 1, -2), first(last + 1, -1);
    std::queue<int> q;
    q.push(pos);
    parent[pos] = -1;
    while (!q.empty()) {
        const int p = q.front();
        q.pop();
        for (int a = 0; a < 4; ++a) {
            const int np = move_pos(track, p, kSteps[a], kJump[a]);
            if (np < 0 || parent[np] != -2) {
                continue;
            }
            parent[np] = p;
            first[np] = p == pos ? a : first[p];
            if (np == last) {
                return kNames[first[np]];
            }
            q.push(np);
        }
    }
    return "LEFT";
}

}  // namespace

int main() {
    std::string line;
    int player = 0, games = 0;
    if (!std::getline(std::cin, line)) {
        return 0;
    }
    std::istringstream(line) >> player;
    if (!std::getline(std::cin, line)) {
        return 0;
    }
    std::istringstream(line) >> games;

    while (true) {
        bool alive = true;
        for (int i = 0; i < 3 && alive; ++i) {
            alive = static_cast<bool>(std::getline(std::cin, line));
        }
        if (!alive) {
            break;
        }
        std::string gpu;
        std::array<int, 7> reg{};
        int count = 0;
        for (int g = 0; g < games; ++g) {
            if (!std::getline(std::cin, line)) {
                return 0;
            }
            std::istringstream ss(line);
            if (ss >> gpu) {
                for (int i = 0; i < 7; ++i) {
                    ss >> reg[i];
                }
                ++count;
            }
        }
        if (count == 0 || gpu == "GAME_OVER") {
            std::cout << "LEFT\n";
        } else {
            std::cout << best_action(gpu, reg[player]) << '\n';
        }
        std::cout.flush();
    }
    return 0;
}
