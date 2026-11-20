// Ghost in the Cell Wood 3 — forces effectives + meilleure action globale.
#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int INF = 1'000'000;
constexpr int RESERVE = 5;
}  // namespace

int main() {
    int n, m;
    if (!(std::cin >> n >> m)) return 0;
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, INF));
    for (int i = 0; i < n; ++i) dist[i][i] = 0;
    for (int i = 0; i < m; ++i) {
        int a, b, d;
        std::cin >> a >> b >> d;
        dist[a][b] = dist[b][a] = d;
    }
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);

    for (;;) {
        int count;
        if (!(std::cin >> count)) return 0;
        std::vector<int> owner(n, 0), cyborgs(n, 0), prod(n, 0);
        std::vector<std::array<int, 4>> troops;  // owner, src, dst, count
        for (int i = 0; i < count; ++i) {
            int id, a, b, c, d, e;
            std::string kind;
            if (!(std::cin >> id >> kind >> a >> b >> c >> d >> e)) return 0;
            if (kind == "FACTORY") {
                owner[id] = a;
                cyborgs[id] = b;
                prod[id] = c;
            } else if (kind == "TROOP") {
                troops.push_back({a, b, c, d});
            }
        }
        // Appliquer les troupes après toutes les usines (l'ordre des entités
        // est arbitraire : une FACTORY lue après une TROOP l'écraserait).
        for (const auto& t : troops) cyborgs[t[2]] += (t[0] == 1) ? t[3] : -t[3];
        for (int i = 0; i < n; ++i) if (owner[i] == 0) cyborgs[i] = 0;

        int best_score = 0, best_src = -1, best_dst = -1, best_count = 0;
        for (int i = 0; i < n; ++i) {
            if (owner[i] != 1 || cyborgs[i] >= 0) continue;
            for (int j = 0; j < n; ++j) {
                if (owner[j] != 1 || cyborgs[j] <= -cyborgs[i] + RESERVE) continue;
                int cnt = std::min(cyborgs[j] - RESERVE, -cyborgs[i] + 2);
                int score = 1000 - dist[j][i];
                if (score > best_score) {
                    best_score = score; best_src = j; best_dst = i; best_count = cnt;
                }
            }
        }
        if (best_src < 0) {
            for (int src = 0; src < n; ++src) {
                if (owner[src] != 1 || cyborgs[src] <= RESERVE) continue;
                for (int dst = 0; dst < n; ++dst) {
                    if (owner[dst] == 1 || dist[src][dst] >= INF) continue;
                    int need = cyborgs[dst] + 1;
                    if (need <= 0 || cyborgs[src] - RESERVE < need) continue;
                    int score = (owner[dst] == 0 ? 100 : 0) + prod[dst] * 20
                                - dist[src][dst] * 2 - need;
                    if (score > best_score) {
                        best_score = score; best_src = src; best_dst = dst; best_count = need;
                    }
                }
            }
        }
        if (best_src < 0) std::cout << "WAIT\n";
        else std::cout << "MOVE " << best_src << ' ' << best_dst << ' ' << best_count << '\n';
        std::cout << std::flush;
    }
}
