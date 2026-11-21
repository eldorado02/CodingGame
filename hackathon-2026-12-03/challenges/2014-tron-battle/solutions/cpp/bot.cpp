// Tron Battle — Voronoi 1-ply + greedy primé. Même logique que Python/C.
#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

constexpr int W = 30, H = 20, MAXP = 4, INF = 99;
constexpr int DX[4] = {0, 0, -1, 1}, DY[4] = {-1, 1, 0, 0};
const char *const DIR_NAME[4] = {"UP", "DOWN", "LEFT", "RIGHT"};

using Grid = array<array<unsigned char, W>, H>;
using Dist = array<array<int, W>, H>;
using Cell = pair<int,int>;

static array<array<array<int,4>,W>,H> neigh;
static array<array<int,W>,H> neigh_n;

static void init_neigh() {
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        int n = 0;
        for (int d = 0; d < 4; ++d) {
            int nx = x + DX[d], ny = y + DY[d];
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) neigh[y][x][n++] = nx + ny*W;
        }
        neigh_n[y][x] = n;
    }
}

static Dist bfs(int sx, int sy, const Grid &blocked) {
    Dist dist;
    for (auto &row : dist) row.fill(INF);
    if (sx < 0) return dist; // la tête est occupée mais reste origine légitime
    deque<int> q;
    q.push_back(sx + sy*W);
    dist[sy][sx] = 0;
    while (!q.empty()) {
        int cell = q.front(); q.pop_front();
        int x = cell % W, y = cell / W;
        for (int i = 0; i < neigh_n[y][x]; ++i) {
            int nx = neigh[y][x][i] % W, ny = neigh[y][x][i] / W;
            if (!blocked[ny][nx] && dist[ny][nx] == INF) {
                dist[ny][nx] = dist[y][x] + 1;
                q.push_back(neigh[y][x][i]);
            }
        }
    }
    return dist;
}

static int greedy_fill(int sx, int sy, const Grid &blocked, int limit) {
    Grid local = blocked;
    int x = sx, y = sy, count = 0;
    while (count < limit) {
        int best_free = 999, bx = -1, by = -1;
        for (int i = 0; i < neigh_n[y][x]; ++i) {
            int nx = neigh[y][x][i] % W, ny = neigh[y][x][i] / W;
            if (local[ny][nx]) continue;
            int free_next = 0;
            for (int j = 0; j < neigh_n[ny][nx]; ++j) {
                int ax = neigh[ny][nx][j] % W, ay = neigh[ny][nx][j] / W;
                if (!local[ay][ax]) ++free_next;
            }
            if (make_tuple(free_next, -ny, -nx) < make_tuple(best_free, -by, -bx)) {
                best_free = free_next; bx = nx; by = ny;
            }
        }
        if (bx < 0) break;
        x = bx; y = by; local[y][x] = 1; ++count;
    }
    return count;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    init_neigh();
    int n, p;
    if (!(cin >> n >> p)) return 0;
    Grid blocked{};
    array<Cell,MAXP> heads{};
    array<Grid,MAXP> seen{};
    bool initialized = false;
    while (true) {
        if (initialized && !(cin >> n >> p)) return 0;
        for (int i = 0; i < n; ++i) {
            int x0,y0,x1,y1; cin >> x0 >> y0 >> x1 >> y1;
            heads[i] = {x1,y1};
        }
        if (!initialized) { for (auto &s : seen) s = Grid{}; initialized = true; }
        for (auto &row : blocked) row.fill(0);
        for (int i = 0; i < n; ++i) {
            if (heads[i].first < 0) { seen[i] = Grid{}; continue; }
            seen[i][heads[i].second][heads[i].first] = 1;
            for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
                if (seen[i][y][x]) blocked[y][x] = 1;
        }
        int me = heads[p].first, my = heads[p].second;
        if (me < 0) { cout << "UP\n" << flush; continue; }

        using Key = tuple<int,int,int,int>;
        Key best{-1, 0, 0, 0};
        bool have = false;
        for (int d = 0; d < 4; ++d) {
            int nx = me + DX[d], ny = my + DY[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H || blocked[ny][nx]) continue;
            Grid trial = blocked;
            trial[my][me] = 1; trial[ny][nx] = 1;
            Dist dm = bfs(nx, ny, trial);
            vector<Dist> ds;
            for (int i = 0; i < n; ++i)
                if (i != p && heads[i].first >= 0)
                    ds.push_back(bfs(heads[i].first, heads[i].second, trial));
            int voronoi = 0, reach = 0;
            for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
                if (trial[y][x] || dm[y][x] == INF) continue;
                ++reach;
                int b = INF;
                for (const auto &dd : ds) b = min(b, dd[y][x]);
                if (dm[y][x] < b) ++voronoi;
                else if (dm[y][x] > b) --voronoi;
            }
            int mob = 0;
            for (int i = 0; i < neigh_n[ny][nx]; ++i) {
                int ax = neigh[ny][nx][i] % W, ay = neigh[ny][nx][i] / W;
                if (!trial[ay][ax]) ++mob;
            }
            int fill = greedy_fill(nx, ny, trial, 600);
            Key cur{100000*voronoi + 100*reach + 10*mob + fill, -ny, -nx, d};
            if (!have || cur > best) { best = cur; have = true; }
        }
        cout << (have ? DIR_NAME[get<3>(best)] : "UP") << '\n' << flush;
    }
    return 0;
}
