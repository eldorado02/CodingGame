// Cellularena (Wood) — BFS multi-source, on pousse vers la protéine A la plus proche.
#include <array>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

namespace {

struct Cell {
    bool wall = false;
    bool taken = false;
    bool protein = false;
};

struct Step {
    int parent, fx, fy, dist;
};

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int width, height;
    if (!(std::cin >> width >> height)) return 0;
    while (true) {
        int n;
        if (!(std::cin >> n)) return 0;
        std::vector<std::vector<Cell>> grid(height, std::vector<Cell>(width));
        std::vector<std::array<int, 3>> mine;  // x, y, id
        for (int i = 0; i < n; ++i) {
            int x, y, owner, id, parent, root;
            std::string kind, dir;
            std::cin >> x >> y >> kind >> owner >> id >> dir >> parent >> root;
            if (x < 0 || x >= width || y < 0 || y >= height) continue;
            Cell &cell = grid[y][x];
            if (kind == "WALL") {
                cell.wall = true;
            } else if (kind.size() == 1 && kind[0] >= 'A' && kind[0] <= 'D') {
                cell.protein = true;
            } else {
                cell.taken = true;
                if (owner == 1) mine.push_back({x, y, id});
            }
        }
        int my_a, my_b, my_c, my_d, opp_a, opp_b, opp_c, opp_d, required;
        std::cin >> my_a >> my_b >> my_c >> my_d;
        std::cin >> opp_a >> opp_b >> opp_c >> opp_d;
        std::cin >> required;

        std::vector<std::vector<Step>> info(height, std::vector<Step>(width, {-1, 0, 0, 0}));
        const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
        if (my_a > 0 && !mine.empty()) {
            std::queue<std::pair<int, int>> q;
            for (const auto &o : mine) {
                for (int k = 0; k < 4; ++k) {
                    int nx = o[0] + dx[k], ny = o[1] + dy[k];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    const Cell &c = grid[ny][nx];
                    if (c.wall || c.taken || info[ny][nx].parent != -1) continue;
                    info[ny][nx] = {o[2], nx, ny, 1};
                    q.push({nx, ny});
                }
            }
            while (!q.empty()) {
                auto [x, y] = q.front();
                q.pop();
                for (int k = 0; k < 4; ++k) {
                    int nx = x + dx[k], ny = y + dy[k];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    const Cell &c = grid[ny][nx];
                    if (c.wall || c.taken || info[ny][nx].parent != -1) continue;
                    const Step &from = info[y][x];
                    info[ny][nx] = {from.parent, from.fx, from.fy, from.dist + 1};
                    q.push({nx, ny});
                }
            }
        }

        // On privilégie toujours une source de protéines ; à défaut, la case
        // libre la plus proche. Deux passes évitent de mélanger les deux buts.
        auto pick = [&](bool want_source) {
            std::tuple<int, int, int> best{-1, 0, 0};  // dist, y, x
            Step chosen{-1, 0, 0, 0};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (info[y][x].parent == -1) continue;
                    if (grid[y][x].protein != want_source) continue;
                    std::tuple<int, int, int> key{info[y][x].dist, y, x};
                    if (std::get<0>(best) < 0 || key < best) {
                        best = key;
                        chosen = info[y][x];
                    }
                }
            }
            return chosen;
        };
        Step chosen = pick(true);
        if (chosen.parent < 0) chosen = pick(false);
        if (chosen.parent >= 0) {
            std::cout << "GROW " << chosen.parent << ' ' << chosen.fx << ' '
                      << chosen.fy << " BASIC\n";
        } else {
            std::cout << "WAIT\n";
        }
        for (int i = 1; i < required; ++i) std::cout << "WAIT\n";
        std::cout.flush();
    }
    return 0;
}
