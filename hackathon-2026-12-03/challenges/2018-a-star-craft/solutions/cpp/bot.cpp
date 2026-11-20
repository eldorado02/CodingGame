// A*Craft — flèches parcimonieuses, un seul tour de sortie.
#include <array>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int W = 19, H = 10, LIMIT = 4 * W * H, MAXR = 10;
constexpr int DX[4] = {0, 1, 0, -1};
constexpr int DY[4] = {-1, 0, 1, 0};
constexpr char CH[5] = "URDL";

int wrap(int v, int m) {
    if (v < 0) v += m;
    if (v >= m) v -= m;
    return v;
}
struct Robot { int x, y, d; };
using Placement = std::array<int, 3>;

class Solver {
public:
    std::array<std::array<int, W>, H> board{};
    std::array<std::array<bool, W>, H> isvoid{};
    std::array<std::array<bool, W>, H> isstart{};
    std::vector<Robot> robots;
    std::vector<Placement> initial;

    int build(const std::vector<int>& order, std::vector<Placement>& out) const {
        auto b = board;
        int vis[H * W][4] = {};
        out.clear();
        int total = 0;
        for (int ri : order) {
            int x = robots[ri].x, y = robots[ri].y, d = robots[ri].d;
            vis[y * W + x][d] = 1;
            for (int step = 0; step < LIMIT; ++step) {
                const int nx = wrap(x + DX[d], W), ny = wrap(y + DY[d], H);
                if (isvoid[ny][nx]) break;
                int nd = d;
                if (b[ny][nx] != -1) {
                    nd = b[ny][nx];
                } else if (isstart[ny][nx]) {
                    nd = d;
                } else {
                    int best_look = INT_MAX, best_d = d;
                    bool found = false;
                    for (int d2 = 0; d2 < 4; ++d2) {
                        if (vis[ny * W + nx][d2]) continue;
                        const int tx = wrap(nx + DX[d2], W), ty = wrap(ny + DY[d2], H);
                        const int look = isvoid[ty][tx]
                            ? 99
                            : vis[ty * W + tx][0] + vis[ty * W + tx][1]
                              + vis[ty * W + tx][2] + vis[ty * W + tx][3];
                        if (!found || look < best_look) {
                            best_look = look;
                            best_d = d2;
                            found = true;
                        }
                    }
                    if (!found) break;
                    nd = best_d;
                    if (nd != d) {
                        b[ny][nx] = nd;
                        out.push_back({nx, ny, nd});
                    }
                }
                if (vis[ny * W + nx][nd]) break;
                vis[ny * W + nx][nd] = 1;
                ++total;
                x = nx; y = ny; d = nd;
            }
        }
        return total;
    }
};
}  // namespace

int main() {
    Solver s;
    for (auto& row : s.board) row.fill(-1);
    for (int y = 0; y < H; ++y) {
        std::string line;
        if (!(std::cin >> line)) return 0;
        for (int x = 0; x < W; ++x) {
            char c = line[x];
            if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
            if (c == '#') s.isvoid[y][x] = true;
            for (int d = 0; d < 4; ++d) if (c == CH[d]) s.board[y][x] = d;
        }
    }
    int rc;
    if (!(std::cin >> rc)) return 0;
    if (rc > MAXR) rc = MAXR;
    for (int i = 0; i < rc; ++i) {
        int x, y;
        std::string dir;
        if (!(std::cin >> x >> y >> dir)) return 0;
        if (dir[0] >= 'a' && dir[0] <= 'z') dir[0] = static_cast<char>(dir[0] - 'a' + 'A');
        int d = 0;
        for (int k = 0; k < 4; ++k) if (dir[0] == CH[k]) d = k;
        s.robots.push_back({x, y, d});
        s.isstart[y][x] = true;
    }

    for (int i = 0; i < rc; ++i) {
        const int nx = wrap(s.robots[i].x + DX[s.robots[i].d], W);
        const int ny = wrap(s.robots[i].y + DY[s.robots[i].d], H);
        if (!s.isvoid[ny][nx]) continue;
        for (int d2 = 0; d2 < 4; ++d2) {
            const int tx = wrap(s.robots[i].x + DX[d2], W);
            const int ty = wrap(s.robots[i].y + DY[d2], H);
            if (!s.isvoid[ty][tx]) {
                s.board[s.robots[i].y][s.robots[i].x] = d2;
                s.robots[i].d = d2;
                s.initial.push_back({s.robots[i].x, s.robots[i].y, d2});
                break;
            }
        }
    }

    std::vector<int> order(rc);
    for (int i = 0; i < rc; ++i) order[i] = i;
    std::vector<Placement> p1, p2;
    const int s1 = s.build(order, p1);
    for (int i = 0; i < rc; ++i) order[i] = rc - 1 - i;
    const int s2 = s.build(order, p2);

    bool first = true;
    const auto emit = [&](const Placement& p) {
        if (!first) std::cout << ' ';
        std::cout << p[0] << ' ' << p[1] << ' ' << CH[p[2]];
        first = false;
    };
    for (const auto& p : s.initial) emit(p);
    for (const auto& p : (s2 > s1 ? p2 : p1)) emit(p);
    std::cout << '\n' << std::flush;
    return 0;
}
