// Tulips and Daisies — C++ (submitted), v3 window + gap greedy. Port of solutions/python/bot.py.
// Move score (planted cell overridden, no grid copy): NET_WEIGHT * (harvest -
// cost) - THREAT_WEIGHT * best opponent harvest on the cells where they could
// harvest before + 4-cell windows (ATTACK by our flowers if clean, DEFENCE by
// theirs if we block). Harvest N = 1 + sum(run - 1) over runs >= 4, gold =
// first N Fibonacci. Keep RESERVE_TURNS grass plantings of gold (running out
// loses), else the cheapest move. GAP_WEIGHT * best harvest of filling one
// cell next to our new run (3 + gap + 3 pays 33, not 7). Ties: row-major order.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

constexpr int DIRECTIONS[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
constexpr int ROW = 4;  // flowers in a line to harvest
constexpr long long NET_WEIGHT = 10, THREAT_WEIGHT = 10;
constexpr long long ATTACK[ROW + 1] = {0, 1, 4, 16, 64};    // by our flowers in a clean window
constexpr long long DEFENCE[ROW + 1] = {0, 0, 2, 12, 48};   // by their flowers in a window we block
constexpr long long RESERVE_TURNS = 10;
constexpr long long GAP_WEIGHT = 5;  // a gap harvest is worth half of a sure one
constexpr char SOIL = 'S', GRASS = 'G', ROCKS = 'R';
constexpr int NOT_ALLOWED = -1;

using Cell = pair<int, int>;

long long fibSum(int n) {
    long long total = 0, a = 1, b = 1;
    for (int i = 0; i < n; ++i) {
        total += a;
        long long next = a + b;
        a = b;
        b = next;
    }
    return total;
}

struct Field {
    int width = 0, height = 0, costSoil = 0, costGrass = 0, costRocks = 0, costFlower = 0;
    char mine = 'T', theirs = 'D';
    vector<string> grid;
    Cell placed{-1, -1};  // cell holding our flower in the current hypothesis

    bool inside(int r, int c) const { return r >= 0 && r < height && c >= 0 && c < width; }
    char at(int r, int c) const { return Cell{r, c} == placed ? mine : grid[r][c]; }

    // Planting cost of `flower` on `tile`, NOT_ALLOWED on its own flower.
    int cost(char tile, char flower) const {
        if (tile == flower) return NOT_ALLOWED;
        if (tile == SOIL) return costSoil;
        if (tile == GRASS) return costGrass;
        if (tile == ROCKS) return costRocks;
        return costFlower;
    }

    // Gold earned by planting `flower` at (r, c).
    long long harvest(int r, int c, char flower) const {
        int count = 1;
        bool harvested = false;
        for (const auto &d : DIRECTIONS) {
            int length = 1;
            for (int sign : {1, -1}) {
                int nr = r + sign * d[0], nc = c + sign * d[1];
                while (inside(nr, nc) && at(nr, nc) == flower) {
                    ++length;
                    nr += sign * d[0];
                    nc += sign * d[1];
                }
            }
            if (length >= ROW) { count += length - 1; harvested = true; }
        }
        return harvested ? fibSum(count) : 0;
    }

    // Our flowers after (r, c) along (dr, dc); (r, c) becomes the first other cell.
    int runEnd(int &r, int &c, int dr, int dc) const {
        int count = 0;
        r += dr;
        c += dc;
        while (inside(r, c) && at(r, c) == mine) { ++count; r += dr; c += dc; }
        return count;
    }

    // Best harvest - cost of filling one cell next to our run through (r, c).
    long long gapPotential(int r, int c) const {
        long long best = 0;
        for (const auto &d : DIRECTIONS) {
            int fr = r, fc = c, br = r, bc = c;
            int forward = runEnd(fr, fc, d[0], d[1]), backward = runEnd(br, bc, -d[0], -d[1]);
            int length = 1 + forward + backward;
            const int ends[2][4] = {{fr, fc, d[0], d[1]}, {br, bc, -d[0], -d[1]}};
            for (const auto &e : ends) {
                if (!inside(e[0], e[1])) continue;
                int er = e[0], ec = e[1];
                int total = length + runEnd(er, ec, e[2], e[3]) + 1;
                if (total >= ROW) best = max(best, fibSum(total) - cost(at(e[0], e[1]), mine));
            }
        }
        return best;
    }

    // Best opponent harvest - cost among the threat cells (0 when none pays).
    long long reply(const vector<Cell> &threats, long long gold) const {
        long long best = 0;
        for (const Cell &q : threats) {
            int price = cost(at(q.first, q.second), theirs);
            if (price == NOT_ALLOWED || price > gold) continue;
            long long earned = harvest(q.first, q.second, theirs);
            if (earned && earned - price > best) best = earned - price;
        }
        return best;
    }

    long long windows(int r, int c) const {
        long long score = 0;
        for (const auto &d : DIRECTIONS)
            for (int shift = 0; shift < ROW; ++shift) {
                int ours = 0, theirsNow = 0, oursBefore = 0, theirsBefore = 0;
                bool ok = true;
                for (int k = 0; k < ROW && ok; ++k) {
                    int cr = r + (k - shift) * d[0], cc = c + (k - shift) * d[1];
                    if (!inside(cr, cc)) { ok = false; break; }
                    ours += at(cr, cc) == mine;
                    theirsNow += at(cr, cc) == theirs;
                    oursBefore += Cell{cr, cc} != Cell{r, c} && grid[cr][cc] == mine;
                    theirsBefore += grid[cr][cc] == theirs;
                }
                if (!ok) continue;
                if (theirsNow == 0) score += ATTACK[ours];
                if (oursBefore == 0) score += DEFENCE[theirsBefore];
            }
        return score;
    }

    vector<Cell> threats(long long gold) {
        placed = {-1, -1};
        vector<Cell> out;
        for (int r = 0; r < height; ++r)
            for (int c = 0; c < width; ++c) {
                int price = cost(grid[r][c], theirs);
                if (price != NOT_ALLOWED && price <= gold && harvest(r, c, theirs)) out.push_back({r, c});
            }
        return out;
    }

    Cell choose(long long gold, long long opponentGold) {
        vector<Cell> danger = threats(opponentGold);
        long long reserve = min(gold, RESERVE_TURNS * costGrass);
        bool haveBest = false, haveCheap = false;
        long long bestScore = 0, cheapCost = 0, cheapScore = 0;
        Cell best{0, 0}, cheap{0, 0};
        for (int r = 0; r < height; ++r)
            for (int c = 0; c < width; ++c) {
                int price = cost(grid[r][c], mine);
                if (price == NOT_ALLOWED || price > gold) continue;
                placed = {-1, -1};
                long long earned = harvest(r, c, mine);
                placed = {r, c};
                long long score = NET_WEIGHT * (earned - price) - THREAT_WEIGHT * reply(danger, opponentGold) + windows(r, c) +
                                  (earned ? 0 : GAP_WEIGHT * gapPotential(r, c));
                if ((price == 0 || gold - price + earned >= reserve) && (!haveBest || score > bestScore)) {
                    best = {r, c};
                    bestScore = score;
                    haveBest = true;
                }
                if (!haveCheap || price < cheapCost || (price == cheapCost && score > cheapScore)) {
                    cheap = {r, c};
                    cheapCost = price;
                    cheapScore = score;
                    haveCheap = true;
                }
            }
        return haveBest ? best : cheap;
    }
};

int main() {
    ios::sync_with_stdio(false);
    Field f;
    string mine, theirs;
    if (!(cin >> f.width >> f.height >> f.costSoil >> f.costGrass >> f.costRocks >> f.costFlower >> mine >> theirs)) return 0;
    f.mine = toupper((unsigned char)mine[0]);
    f.theirs = toupper((unsigned char)theirs[0]);
    int turnsLeft;
    long long gold, opponentGold;
    while (cin >> turnsLeft >> gold >> opponentGold) {
        auto t0 = chrono::steady_clock::now();
        f.grid.assign(f.height, "");
        for (string &row : f.grid) {
            cin >> row;
            row.resize(f.width, SOIL);
        }
        if (!cin) return 0;
        Cell move = f.choose(gold, opponentGold);
        cerr << "gold=" << gold << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << move.first << ' ' << move.second << endl;
    }
}
