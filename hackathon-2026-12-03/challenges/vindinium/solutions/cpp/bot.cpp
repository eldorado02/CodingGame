// Vindinium — C++ (submitted), v2 mines, beer and fights. Port of solutions/python/bot.py.
// Kill a weak enemy owning mines within KILL_REACH; avoid cells next to an
// enemy at least as healthy as us. Beer when life <= LOW_LIFE or a tavern is next to us and life <= TOP_UP;
// else the closest mine we do not own if life - path - MINE_FIGHT > SAFETY;
// else the closest tavern. BFS first steps; ties: target (row, col), N E S W.
// Axis of `x` (column or row) detected on the first turn from our spawn digit.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

constexpr int KILL_LIFE = 20;   // one hit kills
constexpr int KILL_REACH = 2;   // moves to stand next to the prey
constexpr int LOW_LIFE = 50, TOP_UP = 80, MINE_FIGHT = 20, SAFETY = 15, BEER_PRICE = 2;
const string WALKABLE = ".0123";
constexpr char TAVERN = 'T';
struct Step { const char *name; int dr, dc; };
constexpr Step MOVES[4] = {{"NORTH", -1, 0}, {"EAST", 0, 1}, {"SOUTH", 1, 0}, {"WEST", 0, -1}};
constexpr int NO_MOVE = -1, UNDECIDED = -1;

using Cell = pair<int, int>;
struct Entity { int owner, x, y, life, gold; };
struct Reach { int dist, first; };
struct Target { int cost, move; };

struct Game {
    vector<string> grid;
    int size = 0, me = 0, xIsColumn = UNDECIDED;

    bool inside(int r, int c) const { return r >= 0 && r < size && c >= 0 && c < size; }
    Cell cell(int x, int y) const { return xIsColumn ? Cell{y, x} : Cell{x, y}; }

    void detectAxes(int x, int y) {
        if (xIsColumn != UNDECIDED) return;
        char spawn = char('0' + me);
        bool rowFirst = inside(x, y) && grid[x][y] == spawn && !(inside(y, x) && grid[y][x] == spawn);
        xIsColumn = !rowFirst;
    }

    map<Cell, Reach> bfs(Cell start, const set<Cell> &blocked) const {
        if (!inside(start.first, start.second)) return {};
        map<Cell, Reach> reach{{start, {0, NO_MOVE}}};
        deque<Cell> queue{start};
        while (!queue.empty()) {
            Cell cur = queue.front();
            queue.pop_front();
            Reach here = reach[cur];
            for (int m = 0; m < 4; ++m) {
                Cell n{cur.first + MOVES[m].dr, cur.second + MOVES[m].dc};
                if (reach.count(n) || !inside(n.first, n.second) || WALKABLE.find(grid[n.first][n.second]) == string::npos ||
                    blocked.count(n))
                    continue;
                reach[n] = {here.dist + 1, here.first == NO_MOVE ? m : here.first};
                queue.push_back(n);
            }
        }
        return reach;
    }

    // Cost and move to step into the best target cell; cost -1 when none.
    Target closest(const map<Cell, Reach> &reach, vector<Cell> targets) const {
        sort(targets.begin(), targets.end());
        Target best{-1, NO_MOVE};
        Cell bestCell{0, 0};
        for (const Cell &t : targets)
            for (int m = 0; m < 4; ++m) {
                Cell n{t.first - MOVES[m].dr, t.second - MOVES[m].dc};  // we step from n into t with move m
                auto it = reach.find(n);
                if (it == reach.end()) continue;
                int cost = it->second.dist + 1;
                if (best.cost < 0 || cost < best.cost || (cost == best.cost && t < bestCell)) {
                    best = {cost, it->second.first == NO_MOVE ? m : it->second.first};
                    bestCell = t;
                }
            }
        return best;
    }

    // Cells next to an enemy at least as healthy as us (never our own cell).
    set<Cell> dangerZone(const map<int, Entity> &heroes, const Entity &hero, Cell pos) const {
        set<Cell> zone;
        for (const auto &[id, h] : heroes) {
            if (id == me || h.life < hero.life) continue;
            Cell c = cell(h.x, h.y);
            for (const Step &m : MOVES) zone.insert({c.first + m.dr, c.second + m.dc});
        }
        zone.erase(pos);
        return zone;
    }

    // First move towards a cell next to a weak enemy owning mines, or "".
    string preyMove(const map<int, Entity> &heroes, const vector<Entity> &mines, const map<Cell, Reach> &reach) const {
        set<int> owners;
        for (const Entity &m : mines) owners.insert(m.owner);
        int bestDist = -1;
        string best;
        for (const auto &[id, h] : heroes) {
            if (id == me || h.life > KILL_LIFE || !owners.count(id)) continue;
            Cell c = cell(h.x, h.y);
            for (const Step &m : MOVES) {
                auto it = reach.find({c.first + m.dr, c.second + m.dc});
                if (it == reach.end() || it->second.dist > KILL_REACH) continue;
                if (bestDist < 0 || it->second.dist < bestDist) {
                    bestDist = it->second.dist;
                    best = it->second.first == NO_MOVE ? "WAIT" : MOVES[it->second.first].name;
                }
            }
        }
        return best;
    }

    string decide(const map<int, Entity> &heroes, const vector<Entity> &mines) const {
        const Entity &hero = heroes.at(me);
        Cell pos = cell(hero.x, hero.y);
        set<Cell> blocked;
        for (const auto &[id, h] : heroes)
            if (id != me) blocked.insert(cell(h.x, h.y));
        string prey = preyMove(heroes, mines, bfs(pos, blocked));
        if (!prey.empty()) return prey;
        for (const Cell &c : dangerZone(heroes, hero, pos)) blocked.insert(c);
        map<Cell, Reach> reach = bfs(pos, blocked);
        vector<Cell> taverns;
        for (int r = 0; r < size; ++r)
            for (int c = 0; c < size; ++c)
                if (grid[r][c] == TAVERN) taverns.push_back({r, c});
        Target toTavern = closest(reach, taverns);
        bool canPay = hero.gold >= BEER_PRICE;
        if (toTavern.cost >= 0 && canPay && (hero.life <= LOW_LIFE || (toTavern.cost == 1 && hero.life <= TOP_UP)))
            return MOVES[toTavern.move].name;
        vector<Cell> targets;
        for (const Entity &m : mines)
            if (m.owner != me) targets.push_back(cell(m.x, m.y));
        Target toMine = closest(reach, targets);
        if (toMine.cost >= 0 && hero.life - toMine.cost - MINE_FIGHT > SAFETY) return MOVES[toMine.move].name;
        if (toTavern.cost >= 0 && canPay) return MOVES[toTavern.move].name;
        return toMine.cost >= 0 ? MOVES[toMine.move].name : "WAIT";
    }
};

int main() {
    ios::sync_with_stdio(false);
    Game game;
    if (!(cin >> game.size)) return 0;
    game.grid.resize(game.size);
    for (string &row : game.grid) {
        cin >> row;
        row.resize(game.size, '#');
    }
    cin >> game.me;
    int count;
    while (cin >> count) {
        map<int, Entity> heroes;
        vector<Entity> mines;
        for (int i = 0; i < count; ++i) {
            string kind;
            Entity e;
            cin >> kind >> e.owner >> e.x >> e.y >> e.life >> e.gold;
            if (kind == "HERO") heroes[e.owner] = e;
            else mines.push_back(e);
        }
        if (!cin) return 0;
        if (!heroes.count(game.me)) { cout << "WAIT" << endl; continue; }
        game.detectAxes(heroes[game.me].x, heroes[game.me].y);
        cout << game.decide(heroes, mines) << endl;
    }
}
