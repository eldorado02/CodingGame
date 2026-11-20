// Xmas Rush — BFS sur les tuiles, poussée qui minimise la distance à la quête.
#include <array>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr int N = 7, MAXSTEP = 20;
constexpr int DX[4] = {0, 1, 0, -1};
constexpr int DY[4] = {-1, 0, 1, 0};
constexpr const char* DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

using Grid = std::array<std::array<int, N>, N>;
struct Pos { int x, y; };

int mask_of(const std::string& s) {
    int m = 0;
    for (int d = 0; d < 4; ++d) if (s[d] == '1') m |= 1 << d;
    return m;
}
int wrap(int v) { v %= N; return v < 0 ? v + N : v; }

std::optional<std::vector<int>> path_bfs(const Grid& g, Pos s, Pos t, bool distance_only) {
    if (s.x == t.x && s.y == t.y) return std::vector<int>{};
    constexpr int C = N * N;
    std::array<int, C> prev{}, pd{};
    prev.fill(-2); pd.fill(-1);
    std::queue<int> q;
    int si = s.y * N + s.x;
    prev[si] = si;
    q.push(si);
    int ti = t.y * N + t.x;
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == ti) break;
        int x = cur % N, y = cur / N;
        for (int d = 0; d < 4; ++d) {
            if (!(g[y][x] & (1 << d))) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!(g[ny][nx] & (1 << ((d + 2) % 4)))) continue;
            int ni = ny * N + nx;
            if (prev[ni] != -2) continue;
            prev[ni] = cur; pd[ni] = d;
            q.push(ni);
        }
    }
    if (prev[ti] == -2) return std::nullopt;
    if (distance_only) {
        int n = 0;
        for (int cur = ti; cur != si; cur = prev[cur]) ++n;
        return std::vector<int>(n);
    }
    std::vector<int> rev;
    for (int cur = ti; cur != si; cur = prev[cur]) rev.push_back(pd[cur]);
    return std::vector<int>(rev.rbegin(), rev.rend());
}

struct Sim { Grid g; Pos p; std::optional<Pos> item; };

Sim simulate(const Grid& in, int tile, Pos p, std::optional<Pos> item, int line, int d) {
    Sim r{in, p, item};
    int insx, insy, popx, popy;
    if (d == 1) {
        insx = 0; insy = line; popx = N - 1; popy = line;
        for (int x = N - 1; x > 0; --x) r.g[x][line] = r.g[x - 1][line];
    } else if (d == 3) {
        insx = N - 1; insy = line; popx = 0; popy = line;
        for (int x = 0; x < N - 1; ++x) r.g[x][line] = r.g[x + 1][line];
    } else if (d == 0) {
        insx = line; insy = N - 1; popx = line; popy = 0;
        for (int y = 0; y < N - 1; ++y) r.g[line][y] = r.g[line][y + 1];
    } else {
        insx = line; insy = 0; popx = line; popy = N - 1;
        for (int y = N - 1; y > 0; --y) r.g[line][y] = r.g[line][y - 1];
    }
    r.g[insy][insx] = tile;
    if (!item) r.item = Pos{insx, insy};
    else if (item->x == popx && item->y == popy) r.item.reset();
    else if (((d == 1 || d == 3) && item->y == line) ||
             ((d == 0 || d == 2) && item->x == line))
        r.item = Pos{wrap(item->x + DX[d]), wrap(item->y + DY[d])};
    if (((d == 1 || d == 3) && p.y == line) ||
        ((d == 0 || d == 2) && p.x == line))
        r.p = Pos{wrap(p.x + DX[d]), wrap(p.y + DY[d])};
    return r;
}
}  // namespace

int main() {
    std::string first;
    while (std::getline(std::cin, first)) {
        if (first.empty()) continue;
        const int turn = std::stoi(first);
        Grid g{};
        for (int y = 0; y < N; ++y) {
            std::string row;
            std::getline(std::cin, row);
            std::istringstream is(row);
            std::string cell;
            for (int x = 0; x < N; ++x) { is >> cell; g[y][x] = mask_of(cell); }
        }
        int px = 0, py = 0, tile = 0;
        for (int i = 0; i < 2; ++i) {
            int cards, x, y;
            std::string tp;
            std::getline(std::cin, first);
            std::istringstream is(first);
            is >> cards >> x >> y >> tp;
            if (i == 0) { px = x; py = y; tile = mask_of(tp); }
        }
        int nitems;
        std::getline(std::cin, first);
        std::istringstream(first) >> nitems;
        std::string myitem;
        std::optional<Pos> item;
        for (int i = 0; i < nitems; ++i) {
            std::getline(std::cin, first);
            std::istringstream is(first);
            std::string name;
            int x, y, pid;
            is >> name >> x >> y >> pid;
            if (pid == 0) {
                myitem = name;
                item = (x < 0) ? std::optional<Pos>{} : std::optional<Pos>{Pos{x, y}};
            }
        }
        int nquests;
        std::getline(std::cin, first);
        std::istringstream(first) >> nquests;
        bool found = false;
        for (int i = 0; i < nquests; ++i) {
            std::getline(std::cin, first);
            std::istringstream is(first);
            std::string name;
            int pid;
            is >> name >> pid;
            if (pid == 0 && name == myitem) found = true;
        }
        if (!found) item.reset();

        if (turn == 1) {
            if (!item) { std::cout << "PASS\n"; continue; }
            auto path = path_bfs(g, Pos{px, py}, *item, false);
            if (!path || path->empty()) { std::cout << "PASS\n"; continue; }
            std::cout << "MOVE";
            for (size_t i = 0; i < path->size() && i < MAXSTEP; ++i)
                std::cout << ' ' << DIRS[(*path)[i]];
            std::cout << '\n';
            continue;
        }

        int best = 1'000'000, bestline = 0, bestd = 0;
        for (int horizontal = 1; horizontal >= 0; --horizontal) {
            for (int line = 0; line < N; ++line) {
                const int dirs[2] = {horizontal ? 1 : 0, horizontal ? 3 : 2};
                for (int d : dirs) {
                    auto s = simulate(g, tile, Pos{px, py}, item, line, d);
                    int dist;
                    if (!s.item) dist = 1000;
                    else if (s.item->x == s.p.x && s.item->y == s.p.y) dist = -1;
                    else {
                        auto path = path_bfs(s.g, s.p, *s.item, true);
                        dist = path ? static_cast<int>(path->size()) : 100;
                    }
                    if (dist < best || (dist == best &&
                        (line < bestline || (line == bestline && d < bestd)))) {
                        best = dist; bestline = line; bestd = d;
                    }
                }
            }
        }
        std::cout << "PUSH " << bestline << ' ' << DIRS[bestd] << '\n' << std::flush;
    }
    return 0;
}
