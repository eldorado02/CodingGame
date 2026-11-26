// Code Keeper - The Hero — C++ (submitted), v1 rule-based explorer.
// Port of solutions/python/bot.py (same rules, same tie-breaks): remember the
// maze, fight with the cheapest sure weapon, walk by BFS to the nearest item
// or frontier, leave through the exit when time or health runs out.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

constexpr int WIDTH = 16, HEIGHT = 12, CELLS = WIDTH * HEIGHT;
constexpr int TURN_LIMIT = 150, HERO_FOV = 3;
constexpr int UNKNOWN = 0, FLOOR = 1, WALL = 2;
constexpr int EXIT = 0, OBSTACLE = 1, TREASURE = 2, POTION = 3;
constexpr int FIRST_CHEST = 4, LAST_CHEST = 6, BOX = 7;
constexpr int FIRST_MONSTER = 8, LAST_MONSTER = 11;
constexpr int VIEW_RANGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3};
constexpr int ATTACK_RANGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1};
constexpr int DAMAGE[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3};
constexpr int SWORD = 0, HAMMER = 1, SCYTHE = 2, BOW = 3;
constexpr int WEAPON_DAMAGE[] = {10, 6, 7, 8};
constexpr int BOW_RANGE = 3;
constexpr int EXIT_MARGIN = 6;      // spare turns kept when heading for the exit
constexpr int LOW_HEALTH = 10;      // a potion is worth a detour below this
constexpr int CRITICAL_HEALTH = 6;  // leave through the exit if it is known
constexpr int CAUTION_HEALTH = 10;  // below this, walk around cells monsters can see
constexpr int CARDINAL[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
constexpr int NONE = -1;

int cellOf(int x, int y) { return y * WIDTH + x; }
int xOf(int c) { return c % WIDTH; }
int yOf(int c) { return c / WIDTH; }
bool inside(int x, int y) { return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT; }
bool isMonsterType(int t) { return t >= FIRST_MONSTER && t <= LAST_MONSTER; }
bool isChest(int t) { return t >= FIRST_CHEST && t <= LAST_CHEST; }

int chebyshev(int a, int b) { return max(abs(xOf(a) - xOf(b)), abs(yOf(a) - yOf(b))); }
bool isCardinalNeighbour(int a, int b) { return abs(xOf(a) - xOf(b)) + abs(yOf(a) - yOf(b)) == 1; }

struct Monster { int cell, type, hp; };

struct Memory {
    int known[CELLS] = {};
    int items[CELLS];
    // Monsters move only when they see the hero, i.e. inside our view: remember them.
    int monsterType[CELLS], monsterHp[CELLS];
    int exit = NONE, turn = 0;
    Memory() {
        fill(items, items + CELLS, NONE);
        fill(monsterType, monsterType + CELLS, NONE);
    }

    // Every in-bounds cell within the field of view is now known; returns the
    // remembered monsters by increasing cell.
    vector<Monster> update(int hero, const vector<array<int, 4>> &entities) {
        int hx = xOf(hero), hy = yOf(hero);
        for (int y = max(0, hy - HERO_FOV); y < min(HEIGHT, hy + HERO_FOV + 1); ++y)
            for (int x = max(0, hx - HERO_FOV); x < min(WIDTH, hx + HERO_FOV + 1); ++x) {
                known[cellOf(x, y)] = FLOOR;
                items[cellOf(x, y)] = NONE;
                monsterType[cellOf(x, y)] = NONE;
            }
        for (const auto &e : entities) {
            if (!inside(e[0], e[1])) continue;
            int c = cellOf(e[0], e[1]), t = e[2];
            if (t == OBSTACLE) known[c] = WALL;
            else if (t == EXIT || t == TREASURE || t == POTION || isChest(t)) {
                items[c] = t;
                if (t == EXIT) exit = c;
            } else if (t == BOX || isMonsterType(t)) {
                monsterType[c] = t;
                monsterHp[c] = e[3];
            }
        }
        vector<Monster> monsters;
        for (int c = 0; c < CELLS; ++c)
            if (monsterType[c] != NONE) monsters.push_back({c, monsterType[c], monsterHp[c]});
        return monsters;
    }
};

struct Paths { int dist[CELLS], first[CELLS]; };

// Distances and first steps over known floor, avoiding blocked cells;
// dead-end cells can be reached but the path never goes through them.
void bfs(const Memory &m, int start, const bool *blocked, const bool *deadEnd, Paths &p) {
    fill(p.dist, p.dist + CELLS, NONE);
    p.dist[start] = 0;
    p.first[start] = start;
    deque<int> queue{start};
    while (!queue.empty()) {
        int c = queue.front();
        queue.pop_front();
        for (const auto &d : CARDINAL) {
            int nx = xOf(c) + d[0], ny = yOf(c) + d[1];
            if (!inside(nx, ny)) continue;
            int n = cellOf(nx, ny);
            if (p.dist[n] != NONE || m.known[n] != FLOOR || blocked[n]) continue;
            p.dist[n] = p.dist[c] + 1;
            p.first[n] = c == start ? n : p.first[c];
            if (!deadEnd[n]) queue.push_back(n);
        }
    }
}

bool isFrontier(const Memory &m, int c) {
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = xOf(c) + dx, ny = yOf(c) + dy;
            if (inside(nx, ny) && m.known[cellOf(nx, ny)] == UNKNOWN) return true;
        }
    return false;
}

// Higher is better: kills first, then the most dangerous, then the weakest.
bool betterTarget(const Monster &a, const Monster &b, int damage) {
    int ka = a.hp <= damage, kb = b.hp <= damage;
    if (ka != kb) return ka > kb;
    if (DAMAGE[a.type] != DAMAGE[b.type]) return DAMAGE[a.type] > DAMAGE[b.type];
    return a.hp < b.hp;
}

const Monster *pick(const vector<const Monster *> &candidates, int damage) {
    const Monster *best = nullptr;
    for (const Monster *m : candidates)
        if (!best || betterTarget(*m, *best, damage)) best = m;
    return best;
}

string attack(int weapon, int cell) {
    return "ATTACK " + to_string(weapon) + " " + to_string(xOf(cell)) + " " + to_string(yOf(cell));
}
string moveTo(int cell) { return "MOVE " + to_string(xOf(cell)) + " " + to_string(yOf(cell)); }

enum FightKind { NO_FIGHT, ACT, CHASE };
struct Fight { FightKind kind = NO_FIGHT; string command; int chase = NONE; };

Fight combat(int hero, const int *charges, const vector<Monster> &monsters) {
    vector<const Monster *> threats, adjacent, killable, diagonal, hitting;
    for (const Monster &m : monsters)
        if (isMonsterType(m.type) && chebyshev(m.cell, hero) <= VIEW_RANGE[m.type]) threats.push_back(&m);
    if (threats.empty()) return {};
    for (const Monster *m : threats) {
        int d = chebyshev(m->cell, hero);
        if (isCardinalNeighbour(m->cell, hero)) adjacent.push_back(m);
        if (m->hp <= WEAPON_DAMAGE[BOW] && d <= BOW_RANGE) killable.push_back(m);
        if (d == 1) diagonal.push_back(m);
        if (d <= ATTACK_RANGE[m->type]) hitting.push_back(m);
    }
    if (!adjacent.empty()) return {ACT, attack(SWORD, pick(adjacent, WEAPON_DAMAGE[SWORD])->cell)};
    if (charges[BOW] > 0 && !killable.empty())
        return {ACT, attack(BOW, pick(killable, WEAPON_DAMAGE[BOW])->cell)};
    if (!diagonal.empty()) {
        const Monster *target = pick(diagonal, WEAPON_DAMAGE[SCYTHE]);
        if (charges[SCYTHE] > 0 && target->hp <= WEAPON_DAMAGE[SCYTHE]) return {ACT, attack(SCYTHE, target->cell)};
        if (charges[HAMMER] > 0) return {ACT, attack(HAMMER, target->cell)};
    }
    if (!hitting.empty()) return {CHASE, "", pick(hitting, WEAPON_DAMAGE[SWORD])->cell};  // reach a cardinal side
    return {};
}

// Cell to walk to, or NONE.
int chooseTarget(const Memory &m, int hero, int health, const Paths &p) {
    int remaining = TURN_LIMIT - m.turn;
    bool exitReachable = m.exit != NONE && p.dist[m.exit] != NONE;
    if (exitReachable && (remaining <= p.dist[m.exit] + EXIT_MARGIN || health <= CRITICAL_HEALTH)) return m.exit;
    int best = NONE;
    for (int c = 0; c < CELLS; ++c) {
        if (p.dist[c] == NONE || c == m.exit) continue;
        int t = m.items[c];
        bool wanted = t == TREASURE || isChest(t) || (t == POTION && health <= LOW_HEALTH) ||
                      (c != hero && isFrontier(m, c));
        if (wanted && (best == NONE || p.dist[c] < p.dist[best])) best = c;
    }
    if (best != NONE) return best;
    return exitReachable ? m.exit : NONE;
}

// Floor cells inside the view range of a visible monster.
void markWatched(const vector<Monster> &monsters, bool *watched) {
    for (const Monster &mo : monsters) {
        if (!isMonsterType(mo.type)) continue;
        int mx = xOf(mo.cell), my = yOf(mo.cell), r = VIEW_RANGE[mo.type];
        for (int y = max(0, my - r); y < min(HEIGHT, my + r + 1); ++y)
            for (int x = max(0, mx - r); x < min(WIDTH, mx + r + 1); ++x) watched[cellOf(x, y)] = true;
    }
}

// First step towards the chase cell or the chosen target, or NONE.
// With throughMonsters, monster cells are walkable (the step is a sword hit).
int planStep(const Memory &m, int hero, int health, const vector<Monster> &monsters, int chase,
             const bool *extraBlocked, bool throughMonsters) {
    bool blocked[CELLS], deadEnd[CELLS];
    copy(extraBlocked, extraBlocked + CELLS, blocked);
    if (!throughMonsters)
        for (const Monster &mo : monsters)
            if (mo.type != BOX) blocked[mo.cell] = true;
    blocked[hero] = false;
    copy(blocked, blocked + CELLS, deadEnd);
    if (m.exit != NONE) deadEnd[m.exit] = true;
    if (chase != NONE) { blocked[chase] = false; deadEnd[chase] = true; }
    Paths p;
    bfs(m, hero, blocked, deadEnd, p);
    int target = chase != NONE && p.dist[chase] != NONE ? chase : chooseTarget(m, hero, health, p);
    if (target == NONE || target == hero) return NONE;
    return p.first[target];
}

string decide(const Memory &m, int hero, int health, const int *charges, const vector<Monster> &monsters) {
    Fight fight = combat(hero, charges, monsters);
    if (fight.kind == ACT) return fight.command;
    int chase = fight.kind == CHASE ? fight.chase : NONE;
    bool noneBlocked[CELLS] = {};
    int step = NONE;
    if (health < CAUTION_HEALTH && chase == NONE) {
        bool watched[CELLS] = {};
        markWatched(monsters, watched);
        step = planStep(m, hero, health, monsters, chase, watched, false);
    }
    if (step == NONE) step = planStep(m, hero, health, monsters, chase, noneBlocked, false);
    if (step == NONE)  // a monster that never moves may block the only corridor
        step = planStep(m, hero, health, monsters, chase, noneBlocked, true);
    return moveTo(step != NONE ? step : hero);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Memory memory;
    int x, y, health, score, charges[4];
    while (cin >> x >> y >> health >> score >> charges[HAMMER] >> charges[SCYTHE] >> charges[BOW]) {
        charges[SWORD] = 0;
        int count;
        cin >> count;
        vector<array<int, 4>> entities(count);
        for (auto &e : entities) cin >> e[0] >> e[1] >> e[2] >> e[3];
        int hero = cellOf(x, y);
        vector<Monster> monsters = memory.update(hero, entities);
        string action = decide(memory, hero, health, charges, monsters);
        cerr << "t=" << memory.turn << " hp=" << health << " exit=" << memory.exit << endl;
        ++memory.turn;
        cout << action << endl;
    }
}
