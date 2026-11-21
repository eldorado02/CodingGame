// Olymbits (Silver, 4 mini-jeux) — décision jointe, arithmétique entière.
// Une seule action est envoyée aux quatre machines. Pour chaque action on
// estime un score dans chaque mini-jeu, on les pondère, et on joue l'action
// totale maximale (le plus petit index gagne en cas d'égalité).
#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr std::array<const char *, 4> kNames{"LEFT", "DOWN", "RIGHT", "UP"};
constexpr std::array<int, 4> kDx{-1, 0, 1, 0};
constexpr std::array<int, 4> kDy{0, 1, 0, -1};
constexpr std::array<char, 4> kLetter{'L', 'D', 'R', 'U'};
constexpr std::array<int, 4> kSteps{1, 2, 3, 2};
constexpr std::array<bool, 4> kJump{false, false, false, true};

void hurdleStep(const std::string &track, int pos, int stun, int steps,
                bool jump, int *outPos, int *outStun) {
    if (stun > 0) {
        *outPos = pos;
        *outStun = stun - 1;
        return;
    }
    if (pos < 0) pos = 0;
    const int last = static_cast<int>(track.size()) - 1;
    if (jump) {
        const int np = std::min(last, pos + 2);
        *outStun = (np < last && track[np] == '#') ? 3 : 0;
        *outPos = np;
        return;
    }
    int np = pos;
    for (int s = 1; s <= steps; ++s) {
        if (pos + s >= last) {
            *outPos = last;
            *outStun = 0;
            return;
        }
        if (track[pos + s] == '#') {
            *outPos = pos + s;
            *outStun = 3;
            return;
        }
        np = pos + s;
    }
    *outPos = np;
    *outStun = 0;
}

std::array<int, 4> hurdleScores(const std::string &track, int pos, int stun) {
    const int last = static_cast<int>(track.size()) - 1;
    const std::array<int, 4> options{2, 1, 3, 0};  // RIGHT, DOWN, UP, LEFT
    std::array<int, 4> out{};
    for (int a = 0; a < 4; ++a) {
        int p = pos, s = stun;
        hurdleStep(track, p, s, kSteps[a], kJump[a], &p, &s);
        int done = -1;
        if (p >= last) {
            done = 1;
        } else {
            for (int t = 0; t < 3; ++t) {
                bool moved = false;
                for (int m : options) {
                    int q = p, ss = s;
                    hurdleStep(track, q, ss, kSteps[m], kJump[m], &q, &ss);
                    if (q > p) {
                        p = q;
                        s = ss;
                        moved = true;
                        break;
                    }
                }
                if (!moved) hurdleStep(track, p, s, 2, true, &p, &s);
                if (p >= last) {
                    done = t + 2;
                    break;
                }
            }
        }
        if (done < 0) done = 5 + (last - p) / 2;
        out[a] = -100 * done;
    }
    return out;
}

int clamp20(int v) {
    return std::max(-20, std::min(20, v));
}

std::array<int, 4> archeryScores(const std::string &gpu, int x, int y) {
    std::vector<int> winds;
    for (char c : gpu) {
        if (c >= '0' && c <= '9') winds.push_back(c - '0');
    }
    const int w = winds.empty() ? 1 : winds[0];
    std::array<int, 4> out{};
    for (int a = 0; a < 4; ++a) {
        int nx = clamp20(x + kDx[a] * w);
        int ny = clamp20(y + kDy[a] * w);
        int d2 = nx * nx + ny * ny;
        int px = nx, py = ny;
        for (size_t i = 1; i < winds.size() && i <= 5; ++i) {
            const int fw = winds[i];
            if (px > 0) px = std::max(-20, px - fw);
            else if (px < 0) px = std::min(20, px + fw);
            if (py > 0) py = std::max(0, py - fw);
            else if (py < 0) py = std::min(0, py + fw);
        }
        out[a] = -2 * d2 - (px * px + py * py);
    }
    return out;
}

std::array<int, 4> rollerScores(const std::string &gpu, int pos, int risk,
                                int turnsLeft, const std::vector<int> &others) {
    const std::string order = gpu.substr(0, 4);
    const std::array<int, 5> steps{0, 1, 2, 2, 3};
    std::array<int, 4> out{};
    for (int a = 0; a < 4; ++a) {
        const size_t found = order.find(kLetter[a]);
        if (found == std::string::npos) {
            out[a] = -10000;
        } else if (risk < 0) {
            out[a] = -100;
        } else {
            const int idx = static_cast<int>(found);
            int nr = risk + (idx == 0 ? -1 : idx == 1 ? 0 : idx == 2 ? 1 : 2);
            int collide = 0;
            for (int p : others) {
                int diff = (p - pos) % 10;
                if (diff < 0) diff += 10;
                if (diff == 0) collide = 1;
            }
            if (collide) nr += 2;
            if (nr >= 5) {
                out[a] = -100 - turnsLeft;
            } else {
                out[a] = 10 * steps[idx] - 7 * nr - 40 * collide;
            }
        }
    }
    return out;
}

std::array<int, 4> divingScores(const std::string &gpu, int combo) {
    const char goal = gpu.empty() ? 'U' : gpu[0];
    std::array<int, 4> out{};
    for (int a = 0; a < 4; ++a) {
        if (kLetter[a] == goal) {
            out[a] = 10 * (8 + std::min(combo + 1, 10));
        } else {
            out[a] = 10 * (-std::min(combo, 10) - 1);
        }
    }
    return out;
}

bool allOf(const std::string &s, const char *set, size_t n) {
    if (s.empty()) return false;
    for (char c : s) {
        bool ok = false;
        for (size_t i = 0; i < n; ++i) ok = ok || c == set[i];
        if (!ok) return false;
    }
    return true;
}

bool isDigits(const std::string &s) {
    return allOf(s, "0123456789", 10);
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

        std::array<long long, 4> total{0, 0, 0, 0};
        for (int g = 0; g < games; ++g) {
            if (!std::getline(std::cin, line)) break;
            std::istringstream ss(line);
            std::string gpu;
            std::array<int, 7> reg{};
            if (!(ss >> gpu)) continue;
            int nr = 0;
            for (; nr < 7; ++nr) if (!(ss >> reg[nr])) break;
            if (nr < 7 || gpu == "GAME_OVER") continue;

            std::array<int, 4> vals{};
            int weight = 0;
            if (gpu.size() >= 10 && allOf(gpu, ".#", 2)) {
                vals = hurdleScores(gpu, reg[player], reg[3 + player]);
                weight = 3;
            } else if (isDigits(gpu)) {
                vals = archeryScores(gpu, reg[2 * player], reg[2 * player + 1]);
                weight = 1;
            } else if (gpu.size() == 4 && allOf(gpu, "ULDR", 4) && reg[6] >= 0) {
                std::vector<int> others;
                for (int i = 0; i < 3; ++i) {
                    if (i != player) others.push_back(reg[i]);
                }
                vals = rollerScores(gpu, reg[player], reg[3 + player],
                                    reg[6], others);
                weight = 2;
            } else if (allOf(gpu, "ULDR", 4)) {
                vals = divingScores(gpu, reg[3 + player]);
                weight = 2;
            }
            for (int a = 0; a < 4; ++a) total[a] += weight * vals[a];
        }

        int best = 0;
        for (int a = 1; a < 4; ++a) {
            if (total[a] > total[best]) best = a;
        }
        std::cout << kNames[best] << '\n' << std::flush;
    }
    return 0;
}
