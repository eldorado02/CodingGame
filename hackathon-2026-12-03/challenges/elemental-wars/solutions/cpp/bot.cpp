// Elemental Wars — Wood 2+: commande toutes les elementaux, fuit le predateur,
// poursuit la proie. Distances BFS precalculees par ennemi.
#include <array>
#include <deque>
#include <iostream>
#include <string>
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
    int sw, sf, sp;
    while (cin >> sw >> sf >> sp) {
        int count;
        if (!(cin >> count)) break;
        vector<El> els(count);
        for (int i = 0; i < count; ++i)
            cin >> els[i].id >> els[i].tribe >> els[i].x >> els[i].y >> els[i].prisoner;
        char prey = prey_of(me), pred = pred_of(me);
        vector<Dist> preyDist, predDist;
        for (const auto& e : els) {
            if (e.prisoner || e.tribe == me) continue;
            Dist d = bfs(grid, n, e.x, e.y);
            if (e.tribe == prey) preyDist.push_back(d);
            else if (e.tribe == pred) predDist.push_back(d);
        }
        string out;
        for (const auto& e : els) {
            if (e.tribe != me || e.prisoner) continue;
            int bx = e.x, by = e.y, best = -1000000000;
            for (int k = -1; k < 4; ++k) {
                int x = e.x, y = e.y;
                if (k >= 0) { x += DX[k]; y += DY[k]; }
                if (x < 0 || x >= n || y < 0 || y >= n || grid[y][x] == '#') continue;
                int dp = 100, dq = 100;
                for (const auto& d : preyDist) if (d[y][x] >= 0 && d[y][x] < dp) dp = d[y][x];
                for (const auto& d : predDist) if (d[y][x] >= 0 && d[y][x] < dq) dq = d[y][x];
                int score = -100 * (dp < 20 ? dp : 20) + 15 * (dq < 8 ? dq : 8);
                if (dp == 0) score += 10000;          // capture de la proie
                if (dq == 0) score -= 12000;          // On se fait capturer
                if (dq <= 1 && dp > 1) score -= 2000; // Ne pas entrer dans la zone mortelle
                if (score > best || (score == best &&
                    (x < bx || (x == bx && y < by)))) {
                    best = score; bx = x; by = y;
                }
            }
            if (!out.empty()) out += ';';
            if (bx == e.x && by == e.y) out += to_string(e.id) + " WAIT";
            else out += to_string(e.id) + " MOVE " + to_string(bx) + ' ' + to_string(by);
        }
        if (out.empty()) out = "MSG";
        cout << out << '\n' << flush;
    }
    return 0;
}
