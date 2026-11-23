// Tower Dereference (CodinGame) — C++ (submitted version), port of solutions/python/bot.py.
// Constants from the referee (eulerscheZahl/TowerDefense, Constants.java). Upgrades apply at
// once, builds at the end of the turn: a tower built this turn cannot be upgraded yet.
// v1c, stateless: a gun is worth sum(weight(cell)) * damage rate over the shortest-path canyon
// cells in range; cells covered by our glue weigh 1 + GLUE_BONUS. Each turn we greedily buy the
// best gain per coin (build gun / glue on the best free plateau, or upgrade a gun). Damage counts
// up to DAMAGE_CAP; cells on the opponent half are discounted (lost build conflicts). Integer math.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const string GUN = "GUNTOWER", GLUE = "GLUETOWER";
constexpr int GUN_COST = 100, GLUE_COST = 70;
constexpr int GLUE_RANGE[4] = {3, 4, 5, 6};
constexpr int GLUE_BONUS = 3;  // slowed (speed / 4) attackers stay ~4x longer in range
constexpr int GUN_DAMAGE[4] = {5, 8, 15, 30};
constexpr int GUN_RANGE[4] = {3, 4, 5, 6};
constexpr int GUN_RELOAD[4] = {5, 4, 3, 2};
constexpr int UPGRADE_COST[3] = {50, 100, 150};
constexpr int MAX_LEVEL = 3;
constexpr int RATE_SCALE = 60;  // lcm of reload times: damage rate = dmg * 60 / reload
constexpr int DAMAGE_CAP = 15;  // wave HP is 12..18: damage above this is mostly overkill
constexpr int CONTESTED_PERCENT = 90;  // value kept for cells on the opponent half (lost build conflicts)
const char *PROPERTIES[3] = {"DAMAGE", "RANGE", "RELOAD"};

struct Cell { int x, y; };

// Canyon cells lying on a shortest path between the left and right ends.
vector<Cell> pathCells(const vector<string> &grid, int width, int height) {
    auto bfs = [&](int sourceX) {
        vector<int> dist(width * height, -1);
        vector<int> queue;
        for (int y = 0; y < height; ++y)
            if (grid[y][sourceX] == '.') { dist[y * width + sourceX] = 0; queue.push_back(y * width + sourceX); }
        for (size_t head = 0; head < queue.size(); ++head) {
            int c = queue[head], x = c % width, y = c / width;
            const int nx[4] = {x + 1, x - 1, x, x}, ny[4] = {y, y, y + 1, y - 1};
            for (int k = 0; k < 4; ++k) {
                if (nx[k] < 0 || nx[k] >= width || ny[k] < 0 || ny[k] >= height) continue;
                int n = ny[k] * width + nx[k];
                if (grid[ny[k]][nx[k]] == '.' && dist[n] < 0) { dist[n] = dist[c] + 1; queue.push_back(n); }
            }
        }
        return dist;
    };
    vector<int> left = bfs(0), right = bfs(width - 1);
    int total = -1;
    for (int y = 0; y < height; ++y) {
        int d = left[y * width + width - 1];
        if (grid[y][width - 1] == '.' && d >= 0 && (total < 0 || d < total)) total = d;
    }
    vector<Cell> path;
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) {
            if (grid[y][x] != '.') continue;
            int c = y * width + x;
            bool onShortest = left[c] >= 0 && right[c] >= 0 && left[c] + right[c] == total;
            if (total < 0 || onShortest) path.push_back({x, y});
        }
    return path;
}

bool inRange(const Cell &c, int x, int y, int radius) {
    return (c.x - x) * (c.x - x) + (c.y - y) * (c.y - y) <= radius * radius;
}

int damageRate(int damageLevel, int reloadLevel) {
    return min(GUN_DAMAGE[damageLevel], DAMAGE_CAP) * RATE_SCALE / GUN_RELOAD[reloadLevel];
}

// Simultaneous builds on one cell go to the player whose base is closer.
bool onOpponentHalf(int me, int x, int width) {
    int middle = width / 2;
    return me == 0 ? x > middle : x < middle;
}

int sideAdjusted(int value, int me, int x, int width) {
    return onOpponentHalf(me, x, width) ? value * CONTESTED_PERCENT / 100 : value;
}

// One of our towers (or one planned this turn: id -1, not upgradable).
struct Tower {
    int id;
    string kind;
    int x, y;
    int levels[3];  // damage, range, reload

    int radius(const int *lv) const { return (kind == GUN ? GUN_RANGE : GLUE_RANGE)[lv[1]]; }
    bool covers(const Cell &c, const int *lv) const { return inRange(c, x, y, radius(lv)); }
    bool covers(const Cell &c) const { return covers(c, levels); }
};

// Value model with per-cell weights and gun rates, refreshed after every purchase.
struct Defence {
    const vector<Cell> &path;
    vector<Tower> &towers;
    vector<int> glued, weight, rate;

    Defence(const vector<Cell> &p, vector<Tower> &t) : path(p), towers(t) { refresh(); }

    void refresh() {
        size_t n = path.size();
        glued.assign(n, 0); weight.assign(n, 1); rate.assign(n, 0);
        for (size_t i = 0; i < n; ++i) {
            for (const Tower &t : towers) {
                if (!t.covers(path[i])) continue;
                if (t.kind == GLUE) glued[i] = 1;
                else rate[i] += damageRate(t.levels[0], t.levels[2]);
            }
            if (glued[i]) weight[i] = 1 + GLUE_BONUS;
        }
    }

    int gunValue(const Tower &gun, const int *lv) const {
        int covered = 0;
        for (size_t i = 0; i < path.size(); ++i) if (gun.covers(path[i], lv)) covered += weight[i];
        return damageRate(lv[0], lv[2]) * covered;
    }

    // Extra value of a level-0 glue tower at (x, y).
    int glueValue(int x, int y) const {
        int sum = 0;
        for (size_t i = 0; i < path.size(); ++i)
            if (!glued[i] && inRange(path[i], x, y, GLUE_RANGE[0])) sum += rate[i];
        return GLUE_BONUS * sum;
    }

    int buildGunValue(int x, int y) const {
        int covered = 0;
        for (size_t i = 0; i < path.size(); ++i) if (inRange(path[i], x, y, GUN_RANGE[0])) covered += weight[i];
        return damageRate(0, 0) * covered;
    }
};

// Upgrade level of a printed property value, -1 if not in the table.
int levelOf(const int *table, string text) {
    for (char &ch : text) if (ch == ',') ch = '.';
    char *end = nullptr;
    double value = strtod(text.c_str(), &end);
    if (end == text.c_str()) return -1;
    for (int level = 0; level < 4; ++level)
        if (abs(table[level] - value) < 1e-9) return level;
    return -1;
}

struct Option {
    int gain, cost;
    string command;
    int kind;        // 0 build gun, 1 build glue, 2 upgrade
    int x, y;        // build cell
    int tower, prop; // upgrade target (index in towers)
};

// True when option a has a strictly higher gain per coin than b.
bool better(const Option &a, const Option &b) { return (long long)a.gain * b.cost > (long long)b.gain * a.cost; }

template <class F>
bool bestFreeCell(const vector<string> &grid, const vector<vector<bool>> &taken, F valueOf, int me, int &bx, int &by, int &bestValue) {
    bestValue = 0;
    bool found = false;
    for (int y = 0; y < (int)grid.size(); ++y)
        for (int x = 0; x < (int)grid[y].size(); ++x) {
            if (grid[y][x] != '#' || taken[y][x]) continue;
            int value = sideAdjusted(valueOf(x, y), me, x, (int)grid[y].size());
            if (value > bestValue) { bestValue = value; bx = x; by = y; found = true; }
        }
    return found;
}

vector<string> planTurn(const vector<string> &grid, const vector<Cell> &path, int money,
                        vector<Tower> &towers, vector<vector<bool>> &taken, int me) {
    Defence defence(path, towers);
    vector<string> commands;
    while (true) {
        vector<Option> candidates;
        int x = 0, y = 0, value = 0;
        if (bestFreeCell(grid, taken, [&](int a, int b) { return defence.buildGunValue(a, b); }, me, x, y, value))
            candidates.push_back({value, GUN_COST, "BUILD " + to_string(x) + " " + to_string(y) + " GUNTOWER", 0, x, y, -1, -1});
        if (bestFreeCell(grid, taken, [&](int a, int b) { return defence.glueValue(a, b); }, me, x, y, value))
            candidates.push_back({value, GLUE_COST, "BUILD " + to_string(x) + " " + to_string(y) + " GLUETOWER", 1, x, y, -1, -1});
        for (int t = 0; t < (int)towers.size(); ++t) {
            const Tower &gun = towers[t];
            if (gun.kind != GUN || gun.id < 0) continue;
            for (int prop = 0; prop < 3; ++prop) {
                int level = gun.levels[prop];
                if (level >= MAX_LEVEL) continue;
                int upgraded[3] = {gun.levels[0], gun.levels[1], gun.levels[2]};
                ++upgraded[prop];
                int gain = defence.gunValue(gun, upgraded) - defence.gunValue(gun, gun.levels);
                candidates.push_back({gain, UPGRADE_COST[level], "UPGRADE " + to_string(gun.id) + " " + PROPERTIES[prop], 2, 0, 0, t, prop});
            }
        }
        const Option *best = nullptr;
        for (const Option &o : candidates)
            if (o.gain > 0 && o.cost <= money && (!best || better(o, *best))) best = &o;
        if (!best) return commands;
        money -= best->cost;
        commands.push_back(best->command);
        if (best->kind == 2) {
            ++towers[best->tower].levels[best->prop];
        } else {
            taken[best->y][best->x] = true;
            towers.push_back({-1, best->kind == 0 ? GUN : GLUE, best->x, best->y, {0, 0, 0}});
        }
        defence.refresh();
    }
}

int readInt() {
    string line;
    getline(cin, line);
    return atoi(line.c_str());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int me = readInt();
    string line;
    getline(cin, line);
    int width = 0, height = 0;
    istringstream(line) >> width >> height;
    vector<string> grid(height);
    for (string &row : grid) {
        getline(cin, row);
        if (!row.empty() && row.back() == '\r') row.pop_back();
        row = row.substr(0, width);
        row.resize(width, '#');
    }
    vector<Cell> path = pathCells(grid, width, height);
    while (getline(cin, line)) {
        int money = atoi(line.c_str());
        getline(cin, line);  // opponent money and lives
        vector<Tower> towers;
        vector<vector<bool>> taken(height, vector<bool>(width, false));
        int towerCount = readInt();
        for (int i = 0; i < towerCount; ++i) {
            getline(cin, line);
            istringstream in(line);
            string type, damage, range, reload;
            int id, owner, x, y;
            in >> type >> id >> owner >> x >> y >> damage >> range >> reload;
            if (y >= 0 && y < height && x >= 0 && x < width) taken[y][x] = true;
            if ((type != GUN && type != GLUE) || owner != me) continue;
            Tower t{id, type, x, y, {0, 0, 0}};
            if (type == GUN) { t.levels[0] = levelOf(GUN_DAMAGE, damage); t.levels[1] = levelOf(GUN_RANGE, range); t.levels[2] = levelOf(GUN_RELOAD, reload); }
            else t.levels[1] = levelOf(GLUE_RANGE, range);
            if (t.levels[0] >= 0 && t.levels[1] >= 0 && t.levels[2] >= 0) towers.push_back(t);
        }
        int attackerCount = readInt();
        for (int i = 0; i < attackerCount; ++i) getline(cin, line);  // attackers are not used by v1b
        vector<string> commands = planTurn(grid, path, money, towers, taken, me);
        string out;
        for (size_t i = 0; i < commands.size(); ++i) out += (i ? ";" : "") + commands[i];
        cout << (out.empty() ? "PASS" : out) << endl;
    }
    return 0;
}
