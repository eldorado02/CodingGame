// Elemental Wars — fuir le prédateur, poursuivre la proie.
#include <array>
#include <deque>
#include <iostream>
#include <string>
using namespace std;

static const int NMAX = 16;
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

static char prey_of(char t) { return t == 'W' ? 'F' : t == 'F' ? 'P' : 'W'; }
static char pred_of(char t) { return t == 'W' ? 'P' : t == 'F' ? 'W' : 'F'; }

using Grid = array<array<char, NMAX>, NMAX>;
using Dist = array<array<int, NMAX>, NMAX>;

static Dist bfs(const Grid& grid, int n, int sx, int sy) {
    Dist dist{};
    for (auto& row : dist) row.fill(-1);
    if (sx < 0 || sx >= n || sy < 0 || sy >= n || grid[sy][sx] == '#') return dist;
    dist[sy][sx] = 0;
    deque<pair<int,int>> q;
    q.push_back({sx, sy});
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop_front();
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (grid[ny][nx] == '#' || dist[ny][nx] >= 0) continue;
            dist[ny][nx] = dist[y][x] + 1;
            q.push_back({nx, ny});
        }
    }
    return dist;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    char tribe;
    int n;
    if (!(cin >> tribe >> n)) return 0;
    Grid grid{};
    string row;
    for (int y = 0; y < n; ++y) {
        cin >> row;
        for (int x = 0; x < n; ++x) grid[y][x] = x < (int)row.size() ? row[x] : '.';
    }
    int sw, sf, sp;
    while (cin >> sw >> sf >> sp) {
        int count;
        if (!(cin >> count)) break;
        array<int, 8> uid{}, ux{}, uy{}, up{};
        array<char, 8> ut{};
        int my = -1, py = -1, qy = -1;
        for (int i = 0; i < count; ++i) {
            cin >> uid[i] >> ut[i] >> ux[i] >> uy[i] >> up[i];
            if (ut[i] == tribe) my = i;
            if (ut[i] == prey_of(tribe)) py = i;
            if (ut[i] == pred_of(tribe)) qy = i;
        }
        if (my < 0 || py < 0) { cout << "0 WAIT\n" << flush; continue; }
        if (up[my] || up[py]) { cout << uid[my] << " WAIT\n" << flush; continue; }
        int px = py >= 0 ? ux[py] : 100, pyy = py >= 0 ? uy[py] : 100;
        int qx = qy >= 0 && !up[qy] ? ux[qy] : 100;
        int qyy = qy >= 0 && !up[qy] ? uy[qy] : 100;
        Dist dprey = bfs(grid, n, px, pyy);
        Dist dpred = bfs(grid, n, qx, qyy);
        int bestx = ux[my], besty = uy[my], bestScore = -1000000000;
        for (int k = -1; k < 4; ++k) {
            int x = ux[my], y = uy[my];
            if (k >= 0) { x += DX[k]; y += DY[k]; }
            if (x < 0 || x >= n || y < 0 || y >= n || grid[y][x] == '#') continue;
            int dp = dprey[y][x] >= 0 ? dprey[y][x] : 100;
            int dq = dpred[y][x] >= 0 ? dpred[y][x] : 100;
            int score = -100 * dp + 20 * (dq < 6 ? dq : 6);
            if (x == px && y == pyy && !(x == qx && y == qyy)) score += 10000;
            if (x == qx && y == qyy) score -= 10000;
            if (dq <= 1 && dp > 1) score -= 2000;
            if (score > bestScore ||
                (score == bestScore && (x < bestx || (x == bestx && y < besty)))) {
                bestScore = score; bestx = x; besty = y;
            }
        }
        cout << uid[my] << " MOVE " << bestx << ' ' << besty << '\n' << flush;
    }
    return 0;
}
