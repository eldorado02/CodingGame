// Back Track King (Bronze+) — connexion désirée la moins chère et DISRUPT
// ciblé sur les régions où l'adversaire possède le plus de rails.
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace {
const int DIRS[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

bool is_inked(const std::string &s) {
    return !s.empty() && s[0] != '0' && s[0] != 'f' && s[0] != 'F';
}

std::set<std::pair<int, int>> parse_pairs(const std::string &s) {
    std::set<std::pair<int, int>> out;
    size_t start = 0;
    while (start < s.size()) {
        size_t comma = s.find(',', start);
        std::string item = s.substr(start, comma - start);
        size_t dash = item.find('-');
        if (dash != std::string::npos && dash > 0 && dash + 1 < item.size()) {
            bool ok = true;
            for (char c : item.substr(0, dash)) ok &= c >= '0' && c <= '9';
            for (char c : item.substr(dash + 1)) ok &= c >= '0' && c <= '9';
            if (ok) {
                int a = std::stoi(item.substr(0, dash));
                int b = std::stoi(item.substr(dash + 1));
                out.insert({std::min(a, b), std::max(a, b)});
            }
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return out;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int my_id, width, height;
    if (!(std::cin >> my_id >> width >> height)) return 0;
    int cells = width * height;
    std::vector<int> region_at(cells), terrain(cells);
    for (int i = 0; i < cells; ++i) std::cin >> region_at[i] >> terrain[i];

    std::vector<std::pair<int, int>> towns(64, {-1, -1});
    std::set<std::pair<int, int>> desired;
    std::set<int> town_regions;
    int town_count;
    std::cin >> town_count;
    for (int i = 0; i < town_count; ++i) {
        int id, x, y;
        std::string wanted;
        std::cin >> id >> x >> y >> wanted;
        if (id < 0 || id >= static_cast<int>(towns.size())) continue;
        towns[id] = {x, y};
        town_regions.insert(region_at[y * width + x]);
        if (wanted == "x") continue;
        size_t start = 0;
        while (start < wanted.size()) {
            size_t comma = wanted.find(',', start);
            std::string item = wanted.substr(start, comma - start);
            bool digits = !item.empty();
            for (char c : item) digits &= c >= '0' && c <= '9';
            if (digits) {
                int v = std::stoi(item);
                desired.insert({std::min(id, v), std::max(id, v)});
            }
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
    }

    int my_score, foe_score;
    while (std::cin >> my_score >> foe_score) {
        std::vector<int> owner(cells), instability(cells);
        std::vector<bool> inked(cells);
        std::vector<std::set<std::pair<int, int>>> cell_active(cells);
        std::set<std::pair<int, int>> active;
        for (int i = 0; i < cells; ++i) {
            std::string ink, act;
            std::cin >> owner[i] >> instability[i] >> ink >> act;
            inked[i] = is_inked(ink);
            cell_active[i] = parse_pairs(act);
            active.insert(cell_active[i].begin(), cell_active[i].end());
        }

        auto path_cost = [&](int a, int b) -> long long {
            if (towns[a].first < 0 || towns[b].first < 0) return -1;
            int si = towns[a].second * width + towns[a].first;
            int ti = towns[b].second * width + towns[b].first;
            const long long INF = 4e18;
            std::vector<long long> dist(cells, INF);
            std::priority_queue<std::pair<long long, int>,
                                std::vector<std::pair<long long, int>>,
                                std::greater<std::pair<long long, int>>> pq;
            dist[si] = 0;
            pq.push({0, si});
            while (!pq.empty()) {
                auto [d, i] = pq.top();
                pq.pop();
                if (d != dist[i]) continue;
                if (i == ti) return d;
                int x = i % width, y = i / width;
                for (auto &dir : DIRS) {
                    int nx = x + dir[0], ny = y + dir[1];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    int ni = ny * width + nx;
                    if (inked[ni]) continue;
                    long long nd = d + (owner[ni] >= 0 ? 0 : terrain[ni] + 1);
                    if (nd < dist[ni]) { dist[ni] = nd; pq.push({nd, ni}); }
                }
            }
            return -1;
        };

        using Target = std::tuple<long long, int, int, int, int, int, int>;
        std::vector<Target> targets;
        for (auto [a, b] : desired) {
            if (active.count({a, b})) continue;
            long long d = path_cost(a, b);
            if (d < 0 || d == 0) continue;
            targets.emplace_back(d, a, b, towns[a].first, towns[a].second,
                                 towns[b].first, towns[b].second);
        }
        std::sort(targets.begin(), targets.end());

        struct Stats { int mine = 0, foe = 0, inst = 0, active = 0; };
        std::vector<Stats> stats(4096);
        for (int i = 0; i < cells; ++i) {
            int rid = region_at[i];
            if (rid < 0 || rid >= static_cast<int>(stats.size()) ||
                town_regions.count(rid) || inked[i])
                continue;
            if (owner[i] == my_id) ++stats[rid].mine;
            else if (owner[i] >= 0 && owner[i] != 2) ++stats[rid].foe;
            stats[rid].inst = std::max(stats[rid].inst, instability[i]);
            if (!cell_active[i].empty()) stats[rid].active = 1;
        }
        using Cand = std::tuple<int, int, int, int>;
        std::vector<Cand> candidates;
        for (int rid = 0; rid < static_cast<int>(stats.size()); ++rid) {
            const auto &s = stats[rid];
            if (s.foe > s.mine)
                candidates.emplace_back(s.active, s.foe, s.inst, rid);
        }
        std::sort(candidates.begin(), candidates.end(), std::greater<Cand>());

        std::string answer;
        if (!targets.empty()) {
            const auto &t = targets.front();
            answer = "AUTOPLACE " + std::to_string(std::get<3>(t)) + " " +
                     std::to_string(std::get<4>(t)) + " " +
                     std::to_string(std::get<5>(t)) + " " +
                     std::to_string(std::get<6>(t));
        }
        int disrupted = -1;
        if (!candidates.empty()) {
            disrupted = std::get<3>(candidates.front());
            if (!answer.empty()) answer += ";";
            answer += "DISRUPT " + std::to_string(disrupted);
        }

        // Fin de partie : vol des chemins actifs par le tie-break N/E/S/O.
        if (targets.empty()) {
            std::vector<bool> town_cell(cells, false);
            for (const auto &t : towns)
                if (t.first >= 0)
                    town_cell[t.second * width + t.first] = true;
            using Steal = std::tuple<int, int, int, int, int, int>;
            std::vector<Steal> steal;  // n, foe, -terrain, y, x, idx
            for (int i = 0; i < cells; ++i) {
                if (owner[i] != -1 || inked[i] || town_cell[i]) continue;
                if (disrupted >= 0 && region_at[i] == disrupted) continue;
                int x = i % width, y = i / width, n = 0, f = 0;
                for (const auto &dir : DIRS) {
                    int nx = x + dir[0], ny = y + dir[1];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    int j = ny * width + nx;
                    if (cell_active[j].empty()) continue;
                    ++n;
                    if (owner[j] >= 0 && owner[j] != 2 && owner[j] != my_id) ++f;
                }
                if (n) steal.emplace_back(n, f, -terrain[i], y, x, i);
            }
            std::sort(steal.begin(), steal.end(), std::greater<Steal>());
            for (size_t k = 0; k < steal.size() && k < 3; ++k) {
                if (!answer.empty()) answer += ";";
                answer += "PLACE_TRACKS " + std::to_string(std::get<4>(steal[k])) +
                          " " + std::to_string(std::get<3>(steal[k]));
            }
        }
        std::cout << (answer.empty() ? "WAIT" : answer) << '\n' << std::flush;
    }
    return 0;
}
