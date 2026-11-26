// Back to the Code — C++ (submitted), v1 rectangle loops. Port of solutions/python/bot.py.
// Keep (or pick) the opponent-free rectangle with us on its border that
// maximises claimed cells / border cells to walk (2D prefix sums, O(1) per
// candidate), reject it when an opponent is closer than half of the walk,
// and walk its border clockwise corner by corner. No rectangle: nearest
// neutral cell. BACK is never used.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int WIDTH = 35, HEIGHT = 20;
constexpr int MAX_SIDE = 12;  // bigger loops get cut by the opponents
constexpr char NEUTRAL = '.', MINE = '0';

struct Point { int x, y; };
struct Rect { int x0, y0, x1, y1; };
struct Value { int gain, cost; };

// Count of cells matching a predicate in any rectangle, O(1).
struct Prefix {
    int sum[HEIGHT + 1][WIDTH + 1] = {};
    Prefix(const vector<string> &grid, const function<bool(char)> &match) {
        for (int y = 0; y < HEIGHT; ++y)
            for (int x = 0; x < WIDTH; ++x)
                sum[y + 1][x + 1] = sum[y][x + 1] + sum[y + 1][x] - sum[y][x] + (match(grid[y][x]) ? 1 : 0);
    }
    int count(int x0, int y0, int x1, int y1) const {
        if (x0 > x1 || y0 > y1) return 0;
        return sum[y1 + 1][x1 + 1] - sum[y0][x1 + 1] - sum[y1 + 1][x0] + sum[y0][x0];
    }
};

struct Counts { Prefix neutral, mine, foe; };

int distanceToRect(const Point &p, const Rect &r) {
    return max({r.x0 - p.x, 0, p.x - r.x1}) + max({r.y0 - p.y, 0, p.y - r.y1});
}

// Gain and cost of claiming this rectangle; false when it is not worth it.
bool rectValue(const Rect &r, const Counts &c, const vector<Point> &opponents, Value &value) {
    if (c.foe.count(r.x0, r.y0, r.x1, r.y1)) return false;
    int w = r.x1 - r.x0 + 1, h = r.y1 - r.y0 + 1;
    int gain = c.neutral.count(r.x0, r.y0, r.x1, r.y1);
    int border = w > 1 && h > 1 ? w * h - (w - 2) * (h - 2) : w * h;
    int innerMine = c.mine.count(r.x0 + 1, r.y0 + 1, r.x1 - 1, r.y1 - 1);
    int cost = border - (c.mine.count(r.x0, r.y0, r.x1, r.y1) - innerMine);
    if (gain == 0 || cost == 0) return false;
    for (const Point &o : opponents)
        if (2 * distanceToRect(o, r) < cost) return false;  // they could cut the loop
    value = {gain, cost};
    return true;
}

// Rectangle with us on its border maximising gain / cost; ties: bigger gain.
bool bestRect(const Point &me, const Counts &c, const vector<Point> &opponents, Rect &best) {
    bool found = false;
    Value bestValue{0, 1};
    for (int y0 = max(0, me.y - MAX_SIDE + 1); y0 <= me.y; ++y0)
        for (int y1 = me.y; y1 < min(HEIGHT, y0 + MAX_SIDE); ++y1)
            for (int x0 = max(0, me.x - MAX_SIDE + 1); x0 <= me.x; ++x0)
                for (int x1 = me.x; x1 < min(WIDTH, x0 + MAX_SIDE); ++x1) {
                    if (me.x != x0 && me.x != x1 && me.y != y0 && me.y != y1) continue;
                    Rect r{x0, y0, x1, y1};
                    Value v;
                    if (!rectValue(r, c, opponents, v)) continue;
                    long long lhs = (long long)v.gain * bestValue.cost, rhs = (long long)bestValue.gain * v.cost;
                    if (!found || lhs > rhs || (lhs == rhs && v.gain > bestValue.gain)) {
                        best = r;
                        bestValue = v;
                        found = true;
                    }
                }
    return found;
}

// Clockwise corner to aim at from our cell on the border.
Point nextCorner(const Point &me, const Rect &r) {
    if (me.y == r.y0 && me.x < r.x1) return {r.x1, r.y0};
    if (me.x == r.x1 && me.y < r.y1) return {r.x1, r.y1};
    if (me.y == r.y1 && me.x > r.x0) return {r.x0, r.y1};
    return {r.x0, r.y0};
}

bool onBorder(const Point &me, const Rect &r) {
    return r.x0 <= me.x && me.x <= r.x1 && r.y0 <= me.y && me.y <= r.y1 &&
           (me.x == r.x0 || me.x == r.x1 || me.y == r.y0 || me.y == r.y1);
}

Point nearestNeutral(const Point &me, const vector<string> &grid) {
    Point best = me;
    int bestD = -1;
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x) {
            int d = abs(x - me.x) + abs(y - me.y);
            if (grid[y][x] == NEUTRAL && (bestD < 0 || d < bestD)) { best = {x, y}; bestD = d; }
        }
    return best;
}

// Rectangle followed across turns.
struct Plan { bool active = false; Rect rect{}; };

Point decide(const Point &me, const vector<Point> &opponents, const vector<string> &grid, Plan &plan) {
    Counts c{Prefix(grid, [](char ch) { return ch == NEUTRAL; }), Prefix(grid, [](char ch) { return ch == MINE; }),
             Prefix(grid, [](char ch) { return ch != NEUTRAL && ch != MINE; })};
    Value unused;
    if (!plan.active || !onBorder(me, plan.rect) || !rectValue(plan.rect, c, opponents, unused))
        plan.active = bestRect(me, c, opponents, plan.rect);
    return plan.active ? nextCorner(me, plan.rect) : nearestNeutral(me, grid);
}

int main() {
    ios::sync_with_stdio(false);
    int opponentCount;
    if (!(cin >> opponentCount)) return 0;
    Plan plan;
    int round;
    while (cin >> round) {
        Point me;
        int back;
        cin >> me.x >> me.y >> back;
        vector<Point> opponents;
        for (int i = 0; i < opponentCount; ++i) {
            Point o;
            cin >> o.x >> o.y >> back;
            if (o.x >= 0) opponents.push_back(o);
        }
        vector<string> grid(HEIGHT);
        for (string &row : grid) {
            cin >> row;
            row.resize(WIDTH, NEUTRAL);
        }
        if (!cin) return 0;
        Point target = decide(me, opponents, grid, plan);
        cout << target.x << ' ' << target.y << endl;
    }
}
