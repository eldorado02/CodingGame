// Xmas Rush Bronze — poussées locales, distance de Manhattan atteignable, anti-stale.
// Port de xmas_v5.py.
#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr int N = 7, C = 49, MAXSTEP = 20;
constexpr int DX[4] = {0, 1, 0, -1};
constexpr int DY[4] = {-1, 0, 1, 0};
constexpr const char *DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

using Grid = std::array<std::array<int, N>, N>;
struct Pos { int x, y; };
bool operator==(const Pos &a, const Pos &b) { return a.x == b.x && a.y == b.y; }

int mask_of(const std::string &s) {
    int m = 0;
    for (int d = 0; d < 4; ++d) if (s[d] == '1') m |= 1 << d;
    return m;
}
int wrap(int v) { v %= N; return v < 0 ? v + N : v; }

bool connected(const Grid &g, int x, int y, int d) {
    if (!((g[y][x] >> d) & 1)) return false;
    int nx = x + DX[d], ny = y + DY[d];
    return nx >= 0 && nx < N && ny >= 0 && ny < N && ((g[ny][nx] >> ((d + 2) % 4)) & 1);
}

int bfs_dist(const Grid &g, Pos s, Pos t) {
    if (s == t) return 0;
    std::array<int, C> seen;
    seen.fill(-1);
    std::queue<int> q;
    int si = s.y * N + s.x;
    seen[si] = 0;
    q.push(si);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx == t.x && ny == t.y) {
                // distance = distance du prédécesseur + 1 ; on la stocke dans seen.
                return seen[cur] + 1;
            }
            int ni = ny * N + nx;
            if (seen[ni] == -1) { seen[ni] = seen[cur] + 1; q.push(ni); }
        }
    }
    return 999;
}

int manhattan(Pos p, const std::vector<Pos> &targets) {
    int best = 1000000;
    for (const Pos &t : targets) best = std::min(best, std::abs(p.x - t.x) + std::abs(p.y - t.y));
    return best;
}

struct MoveResult { int collected; std::vector<int> dirs; int md; };

MoveResult multi_bfs(const Grid &g, Pos start, const std::vector<Pos> &targets) {
    if (targets.empty()) return {0, {}, 0};
    int nt = static_cast<int>(targets.size());
    constexpr int SM = 1 << 12;  // 12 quêtes au plus (masque 4096)
    struct Par { int px, py, pm, d; };
    static std::array<std::array<std::array<Par, SM>, N>, N> parent{};
    for (auto &row : parent) for (auto &cell : row) for (auto &p : cell) p = {-1, -1, -1, -1};
    std::queue<std::array<int, 4>> q; // x,y,mask,steps
    q.push({start.x, start.y, 0, 0});
    parent[start.y][start.x][0] = {start.x, start.y, 0, -1};
    int bestGot = 0, bestSteps = 0, bestMD = manhattan(start, targets);
    int bx = start.x, by = start.y, bm = 0;
    while (!q.empty()) {
        auto st = q.front(); q.pop();
        int x = st[0], y = st[1], mask = st[2], steps = st[3];
        int md = manhattan({x, y}, targets);
        int got = __builtin_popcount(static_cast<unsigned>(mask));
        bool better = got > bestGot ||
                      (got == bestGot && -md > -bestMD) ||
                      (got == bestGot && md == bestMD && -steps > -bestSteps);
        if (better) {
            bestGot = got; bestSteps = steps; bestMD = md;
            bx = x; by = y; bm = mask;
        }
        if (steps >= MAXSTEP) continue;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int nm = mask;
            for (int i = nt - 1; i >= 0; --i)
                if (targets[i].x == nx && targets[i].y == ny) { nm |= 1 << i; break; }
            if (parent[ny][nx][nm].px == -1) {
                parent[ny][nx][nm] = {x, y, mask, d};
                q.push({nx, ny, nm, steps + 1});
            }
        }
    }
    std::vector<int> dirs;
    int x = bx, y = by, m = bm;
    while (true) {
        Par p = parent[y][x][m];
        if (p.d < 0) break;
        dirs.push_back(p.d);
        x = p.px; y = p.py; m = p.pm;
    }
    std::reverse(dirs.begin(), dirs.end());
    return {bestGot, dirs, bestMD};
}

int reachable_manhattan(const Grid &g, Pos start, const std::vector<Pos> &targets) {
    if (targets.empty()) return 0;
    int best = manhattan(start, targets);
    if (best == 0) return 0;
    std::array<char, C> seen{};
    std::queue<std::array<int, 3>> q; // x,y,steps
    seen[start.y * N + start.x] = 1;
    q.push({start.x, start.y, 0});
    while (!q.empty()) {
        auto st = q.front(); q.pop();
        int x = st[0], y = st[1], steps = st[2];
        if (steps >= MAXSTEP) continue;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (seen[ni]) continue;
            seen[ni] = 1;
            int md = manhattan({nx, ny}, targets);
            if (md < best) best = md;
            if (best == 0) return 0;
            q.push({nx, ny, steps + 1});
        }
    }
    return best;
}

std::vector<int> target_path(const Grid &g, Pos start, const std::vector<Pos> &targets) {
    int bd = 1000000, bx = -1, by = -1;
    for (const Pos &t : targets) {
        int d = bfs_dist(g, start, t);
        if (d < 999 && d < bd) { bd = d; bx = t.x; by = t.y; }
    }
    if (bx < 0 || bd == 0) return {};
    std::array<int, C> prev{};
    prev.fill(-2);
    std::queue<int> q;
    int si = start.y * N + start.x;
    prev[si] = si;
    q.push(si);
    int ti = by * N + bx;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == ti) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (prev[ni] == -2) { prev[ni] = cur; q.push(ni); }
        }
    }
    if (prev[ti] == -2) return {};
    std::vector<int> dirs;
    int cur = ti;
    while (cur != si) {
        int p = prev[cur];
        int x = cur % N, y = cur / N;
        int px = p % N, py = p / N;
        for (int d = 0; d < 4; ++d)
            if (px + DX[d] == x && py + DY[d] == y) { dirs.push_back(d); break; }
        cur = p;
    }
    std::reverse(dirs.begin(), dirs.end());
    if (static_cast<int>(dirs.size()) > MAXSTEP) dirs.resize(MAXSTEP);
    return dirs;
}

std::vector<int> edge_path(const Grid &g, Pos start) {
    std::vector<Pos> goals;
    for (int x = 0; x < N; ++x) { goals.push_back({x, 0}); goals.push_back({x, N - 1}); }
    for (int y = 0; y < N; ++y) { goals.push_back({0, y}); goals.push_back({N - 1, y}); }
    int bd = 1000000, bx = -1, by = -1;
    for (const Pos &t : goals) {
        int d = bfs_dist(g, start, t);
        if (d < 999 && d < bd) { bd = d; bx = t.x; by = t.y; }
    }
    if (bx < 0 || bd == 0) return {};
    std::array<int, C> prev{};
    prev.fill(-2);
    std::queue<int> q;
    int si = start.y * N + start.x;
    prev[si] = si;
    q.push(si);
    int ti = by * N + bx;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == ti) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int ni = ny * N + nx;
            if (prev[ni] == -2) { prev[ni] = cur; q.push(ni); }
        }
    }
    if (prev[ti] == -2) return {};
    std::vector<int> dirs;
    int cur = ti;
    while (cur != si) {
        int p = prev[cur];
        int x = cur % N, y = cur / N;
        int px = p % N, py = p / N;
        for (int d = 0; d < 4; ++d)
            if (px + DX[d] == x && py + DY[d] == y) { dirs.push_back(d); break; }
        cur = p;
    }
    std::reverse(dirs.begin(), dirs.end());
    if (static_cast<int>(dirs.size()) > MAXSTEP) dirs.resize(MAXSTEP);
    return dirs;
}

struct SimResult {
    Grid grid; Pos pos; std::vector<Pos> targets; int popped, instant;
};

SimResult simulate(const Grid &grid, int tile, Pos p, const std::vector<Pos> &targets,
                   int hand, int line, int d) {
    Grid g = grid;
    bool horizontal = (d == 1 || d == 3);
    Pos ins, pop;
    if (d == 1) { ins = {0, line}; pop = {N - 1, line};
        for (int x = N - 1; x > 0; --x) g[x][line] = g[x - 1][line]; }
    else if (d == 3) { ins = {N - 1, line}; pop = {0, line};
        for (int x = 0; x < N - 1; ++x) g[x][line] = g[x + 1][line]; }
    else if (d == 0) { ins = {line, N - 1}; pop = {line, 0};
        for (int y = 0; y < N - 1; ++y) g[line][y] = g[line][y + 1]; }
    else { ins = {line, 0}; pop = {line, N - 1};
        for (int y = N - 1; y > 0; --y) g[line][y] = g[line][y - 1]; }
    g[ins.y][ins.x] = tile;
    bool onLine = horizontal ? (p.y == line) : (p.x == line);
    Pos np = onLine ? Pos{wrap(p.x + DX[d]), wrap(p.y + DY[d])} : p;
    std::vector<Pos> nt;
    int popped = 0;
    for (const Pos &t : targets) {
        if (t == pop) ++popped;
        else if ((horizontal && t.y == line) || (!horizontal && t.x == line))
            nt.push_back({wrap(t.x + DX[d]), wrap(t.y + DY[d])});
        else nt.push_back(t);
    }
    int instant = onLine ? hand : 0;
    if (hand && !onLine) nt.push_back(ins);
    return {g, np, nt, popped, instant};
}

using Score = std::tuple<int, int, int, int, int, int, int, int>;

std::string best_push(const Grid &g, Pos p, int tile, const std::vector<Pos> &targets,
                      int hand, const std::string &lastPush, int stale) {
    std::vector<std::pair<Score, std::string>> cand;
    int lys[3] = {p.y - 1, p.y, p.y + 1};
    int lxs[3] = {p.x - 1, p.x, p.x + 1};
    for (int hor = 0; hor < 2; ++hor) {
        int *lines = hor ? lys : lxs;
        for (int k = 0; k < 3; ++k) {
            int line = lines[k];
            if (line < 0 || line >= N) continue;
            // Ordre Python : pour horizontal d in (1,3), vertical d in (0,2)
            for (int di = 0; di < 2; ++di) {
                int d = hor ? (di == 0 ? 1 : 3) : (di == 0 ? 0 : 2);
                auto r = simulate(g, tile, p, targets, hand, line, d);
                int md = r.targets.empty() ? 0 : reachable_manhattan(r.grid, r.pos, r.targets);
                int collectable = 0;
                for (const Pos &t : r.targets)
                    if (bfs_dist(r.grid, r.pos, t) <= 20) ++collectable;
                int conn = 0;
                for (int dd = 0; dd < 4; ++dd) if (connected(r.grid, r.pos.x, r.pos.y, dd)) ++conn;
                Score sc = std::make_tuple(-r.instant, r.popped, md, -collectable, -conn,
                                           hand, line, d);
                std::string cmd = "PUSH " + std::to_string(line) + " " + DIRS[d];
                cand.push_back({sc, cmd});
            }
        }
    }
    std::sort(cand.begin(), cand.end(), [](const auto &a, const auto &b) {
        if (a.first != b.first) return a.first < b.first;
        return a.second > b.second;
    });
    if (stale >= 3 && cand.size() > 1) {
        for (const auto &c : cand) if (c.second != lastPush) return c.second;
    }
    return cand[0].second;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string line;
    std::string lastPush;
    int stale = 0;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        int turnType = std::stoi(line);
        Grid grid{};
        for (int y = 0; y < N; ++y) {
            if (!std::getline(std::cin, line)) return 0;
            std::istringstream ss(line);
            std::string s;
            for (int x = 0; x < N; ++x) { ss >> s; grid[y][x] = mask_of(s); }
        }
        int px = 0, py = 0, tile = 0;
        for (int i = 0; i < 2; ++i) {
            if (!std::getline(std::cin, line)) return 0;
            std::istringstream ss(line);
            std::string pat;
            int nc, x, y;
            ss >> nc >> x >> y >> pat;
            if (i == 0) { px = x; py = y; tile = mask_of(pat); }
        }
        std::vector<std::string> itemName, itemPos;
        int nItems = 0;
        if (!std::getline(std::cin, line)) return 0;
        nItems = std::stoi(line);
        for (int i = 0; i < nItems; ++i) {
            if (!std::getline(std::cin, line)) return 0;
            std::istringstream ss(line);
            std::string nm; int x, y, pid;
            ss >> nm >> x >> y >> pid;
            itemName.push_back(nm);
            itemPos.push_back(std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(pid));
        }
        int nQuests = 0;
        if (!std::getline(std::cin, line)) return 0;
        nQuests = std::stoi(line);
        std::vector<std::string> quests;
        for (int i = 0; i < nQuests; ++i) {
            if (!std::getline(std::cin, line)) return 0;
            std::istringstream ss(line);
            std::string nm; int pid;
            ss >> nm >> pid;
            if (pid == 0) quests.push_back(nm);
        }
        std::vector<Pos> targets;
        int hand = 0;
        for (const std::string &q : quests) {
            for (size_t i = 0; i < itemName.size(); ++i) {
                if (itemName[i] != q) continue;
                std::istringstream ss(itemPos[i]);
                int x, y, pid; char comma;
                ss >> x >> comma >> y >> comma >> pid;
                if (pid != 0) continue;  // ignorer les items de l'adversaire
                if (x == -1) ++hand;
                else if (x != -2) targets.push_back({x, y});
                break;
            }
        }
        if (turnType == 1) {
            std::vector<int> dirs;
            if (!targets.empty()) dirs = multi_bfs(grid, {px, py}, targets).dirs;
            if (dirs.empty() && !targets.empty()) dirs = target_path(grid, {px, py}, targets);
            if (dirs.empty() && hand) dirs = edge_path(grid, {px, py});
            if (dirs.empty()) std::cout << "PASS\n" << std::flush;
            else {
                std::cout << "MOVE";
                for (int d : dirs) std::cout << ' ' << DIRS[d];
                std::cout << '\n' << std::flush;
            }
        } else {
            std::string cmd = best_push(grid, {px, py}, tile, targets, hand, lastPush, stale);
            if (cmd == lastPush) ++stale; else { stale = 0; lastPush = cmd; }
            std::cout << cmd << '\n' << std::flush;
        }
    }
    return 0;
}
