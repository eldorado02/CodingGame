// Spring Challenge 2020 (Wood 4) — BFS + meilleur pellet, évitement collision.
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
constexpr int MAXC = 64;
constexpr int MAXR = 64;
int W, H;
std::array<std::array<char, MAXC>, MAXR> grid{};
std::array<std::array<int, MAXC>, MAXR> dista{}, px_{}, py_{};
int target_x = -1, target_y = -1;
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};

int wrapx(int x) { x %= W; return x < 0 ? x + W : x; }

void bfs(int sx, int sy) {
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            dista[y][x] = -1; px_[y][x] = -1; py_[y][x] = -1;
        }
    int qx[MAXR * MAXC], qy[MAXR * MAXC], head = 0, tail = 0;
    dista[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; ++tail;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        for (int k = 0; k < 4; ++k) {
            int nx = wrapx(x + DX[k]), ny = y + DY[k];
            if (nx < 0 || ny < 0 || ny >= H) continue;
            if (grid[ny][nx] == '#' || dista[ny][nx] != -1) continue;
            dista[ny][nx] = dista[y][x] + 1;
            px_[ny][nx] = x; py_[ny][nx] = y;
            qx[tail] = nx; qy[tail] = ny; ++tail;
        }
    }
}

bool first_step(int sx, int sy, int tx, int ty, int &ox, int &oy) {
    if (sx == tx && sy == ty) return false;
    int x = tx, y = ty, guard = 0;
    while (!(x == sx && y == sy)) {
        int ax = px_[y][x], ay = py_[y][x];
        if (ax < 0 || ++guard > W * H) return false;
        if (ax == sx && ay == sy) { ox = x; oy = y; return true; }
        x = ax; y = ay;
    }
    return false;
}

struct Key {
    int penalty, cost, negval, d, tx, ty;
    bool operator<(const Key &o) const {
        return std::tie(penalty, cost, negval, d, tx, ty) <
               std::tie(o.penalty, o.cost, o.negval, o.d, o.tx, o.ty);
    }
};

std::vector<std::string> tokens(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string t;
    while (iss >> t) out.push_back(t);
    return out;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    auto head = tokens(line);
    if (head.size() < 2) return 0;
    W = std::stoi(head[0]); H = std::stoi(head[1]);
    for (int y = 0; y < H; ++y) {
        std::getline(std::cin, line);
        for (int x = 0; x < W; ++x)
            grid[y][x] = x < static_cast<int>(line.size()) ? line[x] : ' ';
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        auto f = tokens(line);
        if (f.size() < 2) break;
        int np = std::stoi([&] { std::getline(std::cin, line); return line; }());
        int px = -1, py = -1, ox = -1, oy = -1;
        for (int i = 0; i < np; ++i) {
            std::getline(std::cin, line);
            auto pt = tokens(line);
            if (pt.size() < 4) continue;
            int id = std::stoi(pt[0]), mine = std::stoi(pt[1]);
            int x = std::stoi(pt[2]), y = std::stoi(pt[3]);
            if (mine) { px = x; py = y; }
            else if (ox < 0 || id == 0) { ox = x; oy = y; }
        }
        int npel = 0;
        std::getline(std::cin, line);
        npel = std::stoi(line);
        struct Pellet { int x, y, v; };
        std::vector<Pellet> pellets;
        for (int i = 0; i < npel; ++i) {
            std::getline(std::cin, line);
            auto pt = tokens(line);
            if (pt.size() >= 3)
                pellets.push_back({std::stoi(pt[0]), std::stoi(pt[1]), std::stoi(pt[2])});
        }
        if (px < 0) { std::cout << "MOVE 0 0 0\n" << std::flush; continue; }

        bfs(px, py);
        bool target_ok = false;
        for (const auto &p : pellets)
            if (p.x == target_x && p.y == target_y && target_y >= 0 && target_y < H &&
                target_x >= 0 && target_x < W && dista[target_y][target_x] >= 0)
                target_ok = true;
        if (target_x == px && target_y == py) target_ok = false;
        if (target_ok) { std::cout << "MOVE 0 " << target_x << ' ' << target_y << '\n' << std::flush; continue; }
        if (pellets.empty()) { std::cout << "MOVE 0 " << px << ' ' << py << '\n' << std::flush; continue; }

        int osx = -1, osy = -1;
        if (ox >= 0 && oy >= 0 && ox < W && oy < H) {
            auto sd = dista, sdx = px_, sdy = py_;
            bfs(ox, oy);
            Key bo{0, 0, 0, 0, -1, -1};
            for (const auto &p : pellets) {
                if (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H || dista[p.y][p.x] < 0) continue;
                int d = dista[p.y][p.x];
                Key k{0, d, -p.v, d, p.x, p.y};
                if (bo.tx < 0 || k < bo) bo = k;
            }
            if (bo.tx >= 0 && first_step(ox, oy, bo.tx, bo.ty, osx, osy)) {} else { osx = osy = -1; }
            dista = sd; px_ = sdx; py_ = sdy;
        }

        Key best{0, 0, 0, 0, -1, -1};
        for (const auto &p : pellets) {
            if (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H || dista[p.y][p.x] < 0) continue;
            int sx1, sy1;
            if (!first_step(px, py, p.x, p.y, sx1, sy1)) continue;
            int penalty = (osx >= 0 && sx1 == osx && sy1 == osy) ? 1 : 0;
            int d = dista[p.y][p.x];
            Key k{penalty, d * 10 / p.v, -p.v, d, p.x, p.y};
            if (best.tx < 0 || k < best) best = k;
        }
        if (best.tx < 0) { std::cout << "MOVE 0 " << px << ' ' << py << '\n' << std::flush; continue; }
        target_x = best.tx; target_y = best.ty;
        std::cout << "MOVE 0 " << target_x << ' ' << target_y << '\n' << std::flush;
    }
    return 0;
}
