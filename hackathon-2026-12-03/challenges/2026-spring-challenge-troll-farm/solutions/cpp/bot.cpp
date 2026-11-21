// Troll Farm (Wood 1+) — entraîne un troll polyvalent, récolte, dépose.
#include <array>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

namespace {
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};

struct Tree {
    int x, y, fruits;
};
struct Troll {
    int id, x, y, speed, carry, load;
};
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int w, h;
    if (!(std::cin >> w >> h)) return 0;
    std::vector<std::string> grid(h);
    for (auto &row : grid) std::cin >> row;
    int sx = 0, sy = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (grid[y][x] == '0') { sx = x; sy = y; }
    auto walkable = [&](int x, int y) {
        return x >= 0 && x < w && y >= 0 && y < h && grid[y][x] == '.';
    };
    auto idx = [w](int x, int y) { return y * w + x; };
    std::vector<int> drop;
    for (int d = 0; d < 4; ++d)
        if (walkable(sx + DX[d], sy + DY[d])) drop.push_back(idx(sx + DX[d], sy + DY[d]));

    const int MAX_TROLLS = 3;
    while (true) {
        std::array<int, 6> inv{};
        if (!(std::cin >> inv[0])) break;
        for (int k = 1; k < 6; ++k) std::cin >> inv[k];
        for (int k = 0; k < 6; ++k) { int tmp; std::cin >> tmp; }

        int tree_n;
        if (!(std::cin >> tree_n)) break;
        std::vector<Tree> trees(tree_n);
        for (auto &t : trees) {
            std::string kind;
            int size, health, cooldown;
            std::cin >> kind >> t.x >> t.y >> size >> health >> t.fruits >> cooldown;
        }

        int troll_n;
        if (!(std::cin >> troll_n)) break;
        std::vector<Troll> mine;
        std::vector<char> occupied(w * h, 0);
        for (int i = 0; i < troll_n; ++i) {
            int id, player, x, y, speed, carry, power, reserved;
            int a, b, c, d, e, f;
            std::cin >> id >> player >> x >> y >> speed >> carry >> power >> reserved
                     >> a >> b >> c >> d >> e >> f;
            if (player == 0) {
                mine.push_back({id, x, y, speed, carry, a + b + c + d});
            }
            occupied[idx(x, y)] = 1;
        }

        std::vector<std::string> actions;
        if (static_cast<int>(mine.size()) < MAX_TROLLS) {
            int n = static_cast<int>(mine.size());
            int best_score = -1, best_cost = 1 << 30, bs = 1, bc = 1, bh = 1;
            for (int s = 1; s <= 3; ++s)
                for (int c = 1; c <= 3; ++c)
                    for (int hv = 1; hv <= 3; ++hv) {
                        int cp = n + s * s, cl = n + c * c, ca = n + hv * hv;
                        if (cp > inv[0] || cl > inv[1] || ca > inv[2]) continue;
                        int score = 3 * hv + 2 * c + s;
                        int cost = cp + cl + ca;
                        if (score > best_score || (score == best_score && cost < best_cost)) {
                            best_score = score; best_cost = cost;
                            bs = s; bc = c; bh = hv;
                        }
                    }
            if (best_score >= 0)
                actions.push_back("TRAIN " + std::to_string(bs) + " " +
                                  std::to_string(bc) + " " + std::to_string(bh) + " 0");
        }

        for (const auto &t : mine) {
            std::vector<int> dist(w * h, -1), first(w * h, -1);
            std::deque<int> q;
            int start = idx(t.x, t.y);
            dist[start] = 0;
            first[start] = start;
            q.push_back(start);
            while (!q.empty()) {
                int cur = q.front(); q.pop_front();
                int x = cur % w, y = cur / w;
                for (int d = 0; d < 4; ++d) {
                    int nx = x + DX[d], ny = y + DY[d];
                    if (!walkable(nx, ny)) continue;
                    int ni = idx(nx, ny);
                    if (occupied[ni] || dist[ni] >= 0) continue;
                    dist[ni] = dist[cur] + 1;
                    first[ni] = (cur == start) ? ni : first[cur];
                    q.push_back(ni);
                }
            }
            auto step = [&](int goal) {
                if (dist[goal] <= t.speed) return goal;
                int cur = goal;
                while (dist[cur] > t.speed) cur = first[cur];
                return cur;
            };

            bool on_fruit = false;
            for (const auto &tr : trees)
                if (tr.fruits > 0 && tr.x == t.x && tr.y == t.y) on_fruit = true;
            if (on_fruit && t.load < t.carry) {
                actions.push_back("HARVEST " + std::to_string(t.id));
                continue;
            }
            if (t.load > 0) {
                bool adjacent = false;
                for (int cell : drop) if (cell == start) adjacent = true;
                if (adjacent) {
                    actions.push_back("DROP " + std::to_string(t.id));
                    continue;
                }
                int best = -1;
                for (int cell : drop) {
                    if (dist[cell] < 0) continue;
                    int x = cell % w, y = cell / w;
                    int bx = best < 0 ? 0 : best % w, by = best < 0 ? 0 : best / w;
                    if (best < 0 || dist[cell] < dist[best] ||
                        (dist[cell] == dist[best] &&
                         (y < by || (y == by && x < bx)))) {
                        best = cell;
                    }
                }
                if (best < 0) actions.push_back("WAIT");
                else {
                    int st = step(best);
                    actions.push_back("MOVE " + std::to_string(t.id) + " " +
                                      std::to_string(st % w) + " " + std::to_string(st / w));
                }
                continue;
            }
            int best = -1;
            std::vector<int> fruits(w * h, 0);
            for (const auto &tr : trees) fruits[idx(tr.x, tr.y)] = tr.fruits;
            for (const auto &tr : trees) {
                if (tr.fruits <= 0) continue;
                int cell = idx(tr.x, tr.y);
                if (dist[cell] < 0) continue;
                int bx = best < 0 ? 0 : best % w, by = best < 0 ? 0 : best / w;
                bool better = best < 0 || dist[cell] < dist[best] ||
                              (dist[cell] == dist[best] &&
                               (tr.fruits > fruits[best] ||
                                (tr.fruits == fruits[best] &&
                                 (tr.y < by || (tr.y == by && tr.x < bx)))));
                if (better) best = cell;
            }
            if (best < 0) actions.push_back("WAIT");
            else {
                int st = step(best);
                actions.push_back("MOVE " + std::to_string(t.id) + " " +
                                  std::to_string(st % w) + " " + std::to_string(st / w));
            }
        }
        if (actions.empty()) actions.push_back("WAIT");
        for (size_t i = 0; i < actions.size(); ++i) {
            if (i) std::cout << ';';
            std::cout << actions[i];
        }
        std::cout << '\n' << std::flush;
    }
    return 0;
}
