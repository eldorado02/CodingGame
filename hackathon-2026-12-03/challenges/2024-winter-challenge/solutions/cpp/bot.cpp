// Cellularena (Wood 3+) — HARVESTER puis croissance greedy.
// Les sources déjà harvesées sont bloquées dans le BFS pour ne pas les absorber.
#include <array>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

namespace {

const int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
const std::array<const char *, 4> kDirName{"N", "E", "S", "W"};
// Ordre lexicographique des noms : E < N < S < W.
const int kDirRank[4] = {1, 0, 2, 3};

struct Cell {
    bool wall = false, taken = false;
    char protein = 0;
};

struct Step {
    int parent = -1, fx = 0, fy = 0, dist = 0;
};

int dir_index(const std::string &s) {
    for (int k = 0; k < 4; ++k)
        if (s == kDirName[k]) return k;
    return -1;
}

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
        std::vector<std::pair<int, int>> harvested;
        for (int i = 0; i < n; ++i) {
            int x, y, owner, id, parent, root;
            std::string kind, dir;
            std::cin >> x >> y >> kind >> owner >> id >> dir >> parent >> root;
            if (x < 0 || x >= width || y < 0 || y >= height) continue;
            Cell &cell = grid[y][x];
            if (kind == "WALL") {
                cell.wall = true;
            } else if (kind.size() == 1 && kind[0] >= 'A' && kind[0] <= 'D') {
                cell.protein = kind[0];
            } else {
                cell.taken = true;
                if (owner == 1) {
                    mine.push_back({x, y, id});
                    if (kind == "HARVESTER") {
                        int k = dir_index(dir);
                        if (k >= 0) {
                            int sx = x + DX[k], sy = y + DY[k];
                            if (sx >= 0 && sx < width && sy >= 0 && sy < height)
                                harvested.emplace_back(sx, sy);
                        }
                    }
                }
            }
        }
        std::array<int, 4> my, opp;
        for (int &v : my) std::cin >> v;
        for (int &v : opp) std::cin >> v;
        int required;
        std::cin >> required;

        auto is_harvested = [&](int x, int y) {
            for (const auto &p : harvested)
                if (p.first == x && p.second == y) return true;
            return false;
        };

        std::vector<std::vector<Step>> info(height, std::vector<Step>(width));
        if (required > 0 && !mine.empty()) {
            std::queue<std::pair<int, int>> q;
            for (const auto &o : mine) {
                for (int k = 0; k < 4; ++k) {
                    int nx = o[0] + DX[k], ny = o[1] + DY[k];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    const Cell &c = grid[ny][nx];
                    if (c.wall || c.taken || is_harvested(nx, ny) || info[ny][nx].parent != -1)
                        continue;
                    info[ny][nx] = {o[2], nx, ny, 1};
                    q.push({nx, ny});
                }
            }
            while (!q.empty()) {
                auto [x, y] = q.front();
                q.pop();
                for (int k = 0; k < 4; ++k) {
                    int nx = x + DX[k], ny = y + DY[k];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    const Cell &c = grid[ny][nx];
                    if (c.wall || c.taken || is_harvested(nx, ny) || info[ny][nx].parent != -1)
                        continue;
                    const Step &from = info[y][x];
                    info[ny][nx] = {from.parent, from.fx, from.fy, from.dist + 1};
                    q.push({nx, ny});
                }
            }
        }

        int answer = 0;  // 0=WAIT, 1=HARVESTER, 2=BASIC
        int a_id = 0, a_x = 0, a_y = 0, a_dir = 0;
        if (my[2] > 0 && my[3] > 0) {
            using Key = std::tuple<int, int, int, int>;  // prio, y, x, rang(dir)
            Key best{9, 0, 0, 9};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (info[y][x].parent < 0 || info[y][x].dist != 1) continue;
                    for (int k = 0; k < 4; ++k) {
                        int sx = x + DX[k], sy = y + DY[k];
                        if (sx < 0 || sx >= width || sy < 0 || sy >= height) continue;
                        if (!grid[sy][sx].protein || is_harvested(sx, sy)) continue;
                        Key key{grid[sy][sx].protein == 'A' ? 0 : 1, y, x, kDirRank[k]};
                        if (key < best) {
                            best = key;
                            a_id = info[y][x].parent; a_x = x; a_y = y; a_dir = k;
                            answer = 1;
                        }
                    }
                }
            }
        }
        if (!answer && my[0] > 0) {
            auto pick = [&](int phase) -> Step {
                std::tuple<int, int, int> best{-1, 0, 0};
                Step chosen;
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        if (info[y][x].parent < 0) continue;
                        if (phase == 0 && grid[y][x].protein != 'A') continue;
                        if (phase == 1 && !grid[y][x].protein) continue;
                        std::tuple<int, int, int> key{info[y][x].dist, y, x};
                        if (std::get<0>(best) < 0 || key < best) {
                            best = key;
                            chosen = info[y][x];
                        }
                    }
                }
                return chosen;
            };
            Step chosen = pick(0);
            if (chosen.parent < 0) chosen = pick(1);
            if (chosen.parent < 0) chosen = pick(2);
            if (chosen.parent >= 0) {
                a_id = chosen.parent; a_x = chosen.fx; a_y = chosen.fy;
                answer = 2;
            }
        }

        if (answer == 1)
            std::cout << "GROW " << a_id << ' ' << a_x << ' ' << a_y
                      << " HARVESTER " << kDirName[a_dir] << '\n';
        else if (answer == 2)
            std::cout << "GROW " << a_id << ' ' << a_x << ' ' << a_y << " BASIC\n";
        else
            std::cout << "WAIT\n";
        for (int i = 1; i < required; ++i) std::cout << "WAIT\n";
        std::cout.flush();
    }
    return 0;
}
