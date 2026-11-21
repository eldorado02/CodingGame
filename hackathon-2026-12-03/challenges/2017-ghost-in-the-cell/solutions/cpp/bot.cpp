// Ghost in the Cell (Wood 4) — heuristics time-aware + plusieurs ordres/tour.
// Port de /tmp/opencode/gic_strong.py (stratégie inspirée du post-mortem Agade).
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr int kMaxN = 16;
constexpr int kInf = 1000000;
constexpr int kHorizon = 6;

struct Arrival {
    int owner, src, dst, cnt, eta;
};

struct State {
    std::array<int, kMaxN> owner{}, units{}, prod{}, block{};
    std::vector<std::pair<int, int>> bombEvents;

    // (owner, ours, hostile) à l'usine i après t tours.
    std::array<int, 3> project(int i, int t,
                               const std::vector<Arrival> &arrivals) const {
        int own = owner[i] == 1 ? units[i] : 0;
        int hostile = owner[i] != 1 ? units[i] : 0;
        int ownr = owner[i];
        int blk = block[i];
        for (int turn = 1; turn <= t; ++turn) {
            for (const Arrival &a : arrivals) {
                if (a.dst == i && a.eta == turn) {
                    if (a.owner == 1) own += a.cnt;
                    else hostile += a.cnt;
                }
            }
            bool suppressed = blk > 0;
            for (const auto &be : bombEvents) {
                if (be.first == i && be.second <= turn && turn <= be.second + 4)
                    suppressed = true;
            }
            if (!suppressed && ownr != 0) {
                if (ownr == 1) own += prod[i];
                else hostile += prod[i];
            }
            for (const auto &be : bombEvents) {
                if (be.first == i && be.second == turn) {
                    int victim = ownr == 1 ? own : hostile;
                    int loss = std::max(10, victim / 2);
                    if (ownr == 1) own = std::max(0, own - loss);
                    else hostile = std::max(0, hostile - loss);
                }
            }
            if (blk > 0) --blk;
            if (own > hostile) {
                ownr = 1; own -= hostile; hostile = 0;
            } else if (hostile > own) {
                ownr = -1; hostile -= own; own = 0;
            } else {
                own = hostile = 0;
            }
        }
        return {ownr, own, hostile};
    }

    int defenseNeed(int i, const std::vector<Arrival> &arrivals) const {
        if (owner[i] != 1) return 0;
        int need = 0;
        for (int t = 1; t <= kHorizon; ++t) {
            auto p = project(i, t, arrivals);
            if (p[2] > p[1]) need = std::max(need, p[2] + 1 - p[1]);
        }
        return need;
    }

    int captureNeed(int i, int eta, const std::vector<Arrival> &arrivals) const {
        auto p = project(i, eta, arrivals);
        if (p[1] > p[2]) return 0;
        return p[2] + 1;
    }
};

int usedFrom(const std::vector<Arrival> &arrivals, int src) {
    int used = 0;
    for (const Arrival &a : arrivals) if (a.src == src) used += a.cnt;
    return used;
}

bool hasMoveOrder(const std::vector<std::string> &orders, int src, int dst) {
    std::string prefix = "MOVE " + std::to_string(src) + " " + std::to_string(dst) + " ";
    for (const std::string &o : orders) {
        if (o.compare(0, prefix.size(), prefix) == 0) return true;
    }
    return false;
}

int decide(State &state, const std::vector<std::vector<int>> &dist,
           const std::vector<double> &value, int n, int turn, int bombsLeft,
           std::vector<std::string> &orders, std::vector<Arrival> &arrivals) {
    // 1) Renforts défensifs, usine la plus précieuse d'abord.
    std::vector<int> threatened;
    for (int i = 0; i < n; ++i) if (state.owner[i] == 1) threatened.push_back(i);
    std::stable_sort(threatened.begin(), threatened.end(),
                     [&](int a, int b) { return value[a] > value[b]; });
    for (int dst : threatened) {
        for (int rep = 0; rep < 2; ++rep) {
            int need = state.defenseNeed(dst, arrivals);
            if (need <= 0) break;
            std::vector<int> helpers;
            for (int s = 0; s < n; ++s) {
                if (s != dst && state.owner[s] == 1 && dist[s][dst] < kInf
                    && dist[s][dst] <= need + 2) helpers.push_back(s);
            }
            std::stable_sort(helpers.begin(), helpers.end(), [&](int a, int b) {
                if (dist[a][dst] != dist[b][dst]) return dist[a][dst] < dist[b][dst];
                return state.units[a] > state.units[b];
            });
            bool sent = false;
            for (int src : helpers) {
                int spare = state.units[src] - usedFrom(arrivals, src)
                            - state.defenseNeed(src, arrivals);
                if (spare <= 0) continue;
                int cnt = std::min(spare, need);
                if (cnt <= 0) continue;
                arrivals.push_back({1, src, dst, cnt, dist[src][dst]});
                orders.push_back("MOVE " + std::to_string(src) + " " +
                                 std::to_string(dst) + " " + std::to_string(cnt));
                need -= cnt;
                sent = true;
                if (need <= 0) break;
            }
            if (!sent) break;
        }
    }

    // 2) Attaques.
    for (int rep = 0; rep < 40; ++rep) {
        double bestScore = 0.0;
        int bs = -1, bd = -1, bc = 0;
        for (int src = 0; src < n; ++src) {
            if (state.owner[src] != 1) continue;
            int spare = state.units[src] - usedFrom(arrivals, src)
                        - state.defenseNeed(src, arrivals);
            if (spare <= 0) continue;
            for (int dst = 0; dst < n; ++dst) {
                if (dst == src || state.owner[dst] == 1 || dist[src][dst] >= kInf)
                    continue;
                int eta = dist[src][dst];
                int need = state.captureNeed(dst, eta, arrivals);
                double score;
                int cnt;
                if (state.owner[dst] == 0) {
                    cnt = std::min(spare, need);
                    if (cnt < 1) continue;
                    double denom = (eta + 0.35) * (eta + 0.35)
                                   * std::max(2.0, std::pow((double)need, 0.7));
                    score = value[dst] / denom;
                } else {
                    if ((double)spare < 0.7 * need) continue;
                    cnt = spare;
                    score = value[dst] / ((eta + 0.35) * (eta + 0.35) * 8.0);
                }
                score += 0.0001 * cnt;
                if (bs < 0 || score > bestScore) {
                    bestScore = score; bs = src; bd = dst; bc = cnt;
                }
            }
        }
        if (bs < 0) break;
        arrivals.push_back({1, bs, bd, bc, dist[bs][bd]});
        orders.push_back("MOVE " + std::to_string(bs) + " " +
                         std::to_string(bd) + " " + std::to_string(bc));
    }

    // 3) Bombes.
    if (bombsLeft > 0) {
        std::vector<int> bombed;
        for (const auto &be : state.bombEvents) bombed.push_back(be.first);
        int bombDst = -1;
        double bombScore = 0.0;
        if (turn <= 2) {
            std::vector<int> sources;
            for (int i = 0; i < n; ++i) if (state.owner[i] == -1) sources.push_back(i);
            if (sources.empty())
                for (int i = 0; i < n; ++i) if (state.owner[i] == 0) sources.push_back(i);
            for (int dst = 0; dst < n; ++dst) {
                if (state.owner[dst] == 1) continue;
                if (std::find(bombed.begin(), bombed.end(), dst) != bombed.end()) continue;
                int et = kInf;
                for (int e : sources) et = std::min(et, dist[e][dst]);
                if (et >= kInf || et > 10 || state.prod[dst] <= 0) continue;
                double score = (double)state.prod[dst] * state.prod[dst] * state.prod[dst]
                               / std::max(1, et);
                if (bombDst < 0 || score > bombScore) {
                    bombScore = score; bombDst = dst;
                }
            }
        } else {
            for (int dst = 0; dst < n; ++dst) {
                if (state.owner[dst] != -1 || state.units[dst] < 18) continue;
                if (std::find(bombed.begin(), bombed.end(), dst) != bombed.end()) continue;
                double score = state.units[dst] + 10.0 * state.prod[dst];
                if (bombDst < 0 || score > bombScore) {
                    bombScore = score; bombDst = dst;
                }
            }
        }
        if (bombDst >= 0) {
            int src = -1;
            for (int s = 0; s < n; ++s) {
                if (state.owner[s] != 1 || dist[s][bombDst] >= kInf) continue;
                if (hasMoveOrder(orders, s, bombDst)) continue;
                if (src < 0 || dist[s][bombDst] < dist[src][bombDst]) src = s;
            }
            if (src >= 0) {
                orders.push_back("BOMB " + std::to_string(src) + " " +
                                 std::to_string(bombDst));
                --bombsLeft;
            }
        }
    }
    return bombsLeft;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    int n = std::stoi(line);
    if (!std::getline(std::cin, line)) return 0;
    int m = std::stoi(line);

    std::vector<std::vector<int>> dist(n, std::vector<int>(n, kInf));
    for (int i = 0; i < n; ++i) dist[i][i] = 0;
    for (int k = 0; k < m; ++k) {
        if (!std::getline(std::cin, line)) return 0;
        int a, b, w;
        std::istringstream(line) >> a >> b >> w;
        if (w < dist[a][b]) dist[a][b] = dist[b][a] = w;
    }
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int nd = dist[i][k] + dist[k][j];
                if (nd < dist[i][j]) dist[i][j] = nd;
            }
        }
    }
    std::vector<int> through(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            if (i == k) continue;
            for (int j = 0; j < n; ++j) {
                if (j == i || j == k) continue;
                if (dist[i][j] == dist[i][k] + dist[k][j]) ++through[k];
            }
        }
    }

    int turn = 0, bombsLeft = 2;
    while (true) {
        if (!std::getline(std::cin, line)) break;
        int cnt = std::stoi(line);
        State state;
        std::vector<Arrival> arrivals;
        int ownBombs = 0;
        std::vector<std::pair<int, int>> bombEvents;
        for (int e = 0; e < cnt; ++e) {
            if (!std::getline(std::cin, line)) return 0;
            std::istringstream ss(line);
            int eid;
            std::string type;
            std::array<int, 5> arg{};
            if (!(ss >> eid >> type >> arg[0] >> arg[1] >> arg[2] >> arg[3] >> arg[4]))
                return 0;
            if (type == "FACTORY") {
                state.owner[eid] = arg[0];
                state.units[eid] = arg[1];
                state.prod[eid] = arg[2];
                state.block[eid] = arg[3];
            } else if (type == "TROOP") {
                arrivals.push_back({arg[0], -1, arg[2], arg[3], arg[4]});
            } else if (type == "BOMB") {
                if (arg[0] == 1) {
                    ++ownBombs;
                    if (arg[2] >= 0 && arg[3] > 0)
                        bombEvents.push_back({arg[2], arg[3]});
                }
            }
        }
        bombsLeft = std::min(bombsLeft, 2 - ownBombs);
        std::vector<double> value(n);
        for (int i = 0; i < n; ++i)
            value[i] = state.prod[i] + 0.1 + 0.01 * std::sqrt((double)through[i]);
        state.bombEvents = bombEvents;
        std::vector<std::string> orders;
        bombsLeft = decide(state, dist, value, n, turn, bombsLeft, orders, arrivals);
        std::string out;
        for (size_t i = 0; i < orders.size(); ++i) {
            if (i) out += ';';
            out += orders[i];
        }
        std::cout << (out.empty() ? "WAIT" : out) << '\n' << std::flush;
        ++turn;
    }
    return 0;
}
