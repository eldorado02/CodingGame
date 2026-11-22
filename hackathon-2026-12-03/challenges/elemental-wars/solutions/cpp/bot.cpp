// Elemental Wars — Wood 1+: predicted opponent moves + intercept + evade.
#include <array>
#include <deque>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

static const int NMAX = 16;
static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

using Grid = array<array<char, NMAX>, NMAX>;
using Dist = array<array<int, NMAX>, NMAX>;

static char prey_of(char t) { return t == 'W' ? 'F' : t == 'F' ? 'P' : 'W'; }
static char pred_of(char t) { return t == 'W' ? 'P' : t == 'F' ? 'W' : 'F'; }

static Dist bfs(const Grid& g, int n, int sx, int sy) {
    Dist d{};
    for (auto& r : d) r.fill(-1);
    if (sx < 0 || sx >= n || sy < 0 || sy >= n || g[sy][sx] == '#') return d;
    d[sy][sx] = 0;
    deque<pair<int,int>> q;
    q.push_back({sx, sy});
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop_front();
        for (int k = 0; k < 4; ++k) {
            int nx = x + DX[k], ny = y + DY[k];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (g[ny][nx] == '#' || d[ny][nx] >= 0) continue;
            d[ny][nx] = d[y][x] + 1;
            q.push_back({nx, ny});
        }
    }
    return d;
}

struct El { int id, x, y, prisoner; char tribe; };

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    char me;
    int n;
    if (!(cin >> me >> n)) return 0;
    Grid grid{};
    string row;
    for (int y = 0; y < n; ++y) {
        cin >> row;
        for (int x = 0; x < n; ++x) grid[y][x] = x < (int)row.size() ? row[x] : '.';
    }
    char pred = pred_of(me), prey = prey_of(me);
    int campX = -1, campY = -1;
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x)
            if (grid[y][x] == pred) { campX = x; campY = y; }

    int sw, sf, sp;
    while (cin >> sw >> sf >> sp) {
        int count;
        if (!(cin >> count)) break;
        vector<El> els(count);
        for (int i = 0; i < count; ++i)
            cin >> els[i].id >> els[i].tribe >> els[i].x >> els[i].y >> els[i].prisoner;

        int prisoners = 0;
        vector<int> mine;
        for (int i = 0; i < count; ++i) {
            if (els[i].tribe != me) continue;
            if (els[i].prisoner) ++prisoners;
            else mine.push_back(i);
        }

        // Distances from each free enemy elemental.
        vector<Dist> preyDist, predDist;
        for (const auto& e : els) {
            if (e.prisoner || e.tribe == me) continue;
            Dist d = bfs(grid, n, e.x, e.y);
            if (e.tribe == prey) preyDist.push_back(d);
            else if (e.tribe == pred) predDist.push_back(d);
        }

        // Predict each enemy's next cell: step toward its nearest free prey.
        vector<pair<int,int>> predNext, preyNext;
        for (const auto& e : els) {
            if (e.prisoner || e.tribe == me) continue;
            char et = prey_of(e.tribe);
            int tx = -1, ty = -1, best = 1 << 28;
            for (const auto& q : els) {
                if (q.prisoner || q.tribe != et) continue;
                int d = abs(q.x - e.x) + abs(q.y - e.y);
                if (d < best) { best = d; tx = q.x; ty = q.y; }
            }
            int nx = e.x, ny = e.y;
            if (tx >= 0) {
                Dist dt = bfs(grid, n, tx, ty);
                int cur = dt[e.y][e.x];
                if (cur > 0) {
                    for (int k = 0; k < 4; ++k) {
                        int ax = e.x + DX[k], ay = e.y + DY[k];
                        if (ax < 0 || ax >= n || ay < 0 || ay >= n) continue;
                        if (grid[ay][ax] == '#' || dt[ay][ax] != cur - 1) continue;
                        nx = ax; ny = ay; break;
                    }
                }
            }
            if (e.tribe == prey) preyNext.push_back({nx, ny});
            else predNext.push_back({nx, ny});
        }

        Dist dcamp;
        bool haveCamp = (prisoners > 0 && campX >= 0);
        if (haveCamp) dcamp = bfs(grid, n, campX, campY);
        int liberator = -1;
        if (haveCamp) {
            int best = 1 << 28;
            for (int i : mine) {
                int d = dcamp[els[i].y][els[i].x];
                if (d >= 0 && d < best) { best = d; liberator = i; }
            }
            if (best > 14) liberator = -1;
        }

        vector<pair<int,int>> assigned;
        string out;
        for (int idx : mine) {
            const El& e = els[idx];
            bool lib = (idx == liberator);
            int bx = e.x, by = e.y;
            long long bestScore = -(1LL << 50);
            for (int k = -1; k < 4; ++k) {
                int x = e.x, y = e.y;
                if (k >= 0) { x += DX[k]; y += DY[k]; }
                if (x < 0 || x >= n || y < 0 || y >= n || grid[y][x] == '#') continue;
                int dp = 100, dq = 100;
                for (const auto& d : preyDist) if (d[y][x] >= 0 && d[y][x] < dp) dp = d[y][x];
                for (const auto& d : predDist) if (d[y][x] >= 0 && d[y][x] < dq) dq = d[y][x];

                long long score;
                if (lib) {
                    int dc = dcamp[y][x] >= 0 ? dcamp[y][x] : 100;
                    score = -100LL * (dc < 20 ? dc : 20) + 20LL * (dq < 8 ? dq : 8);
                    if (dc == 0) score += 3000LL * prisoners;
                    if (dq == 0) score -= 20000;
                    else if (dq == 1) score -= 5000;
                } else {
                    score = -100LL * (dp < 20 ? dp : 20) + 20LL * (dq < 8 ? dq : 8);
                    // Predicted interception / evasion.
                    for (const auto& p : preyNext) if (p.first == x && p.second == y) score += 12000;
                    for (const auto& p : predNext) if (p.first == x && p.second == y) score -= 18000;
                    if (dq == 0) score -= 20000;
                    else if (dq == 1) score -= 4000;
                }
                for (const auto& c : assigned)
                    if (c.first == x && c.second == y) score -= 5000;
                if (score > bestScore || (score == bestScore &&
                    (x < bx || (x == bx && y < by)))) {
                    bestScore = score; bx = x; by = y;
                }
            }
            assigned.push_back({bx, by});
            if (!out.empty()) out += ';';
            if (bx == e.x && by == e.y) out += to_string(e.id) + " WAIT";
            else out += to_string(e.id) + " MOVE " + to_string(bx) + ' ' + to_string(by);
        }
        if (out.empty()) out = "MSG";
        cout << out << '\n' << flush;
    }
    return 0;
}
