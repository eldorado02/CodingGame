// Clash of Bots — C++ (submitted), v1 local rules. Port of solutions/python/bot.py.
// Each robot sees only its 5x5 minimap (row 0 = up) and decides alone:
// explode for >= 2 kills without allies, attack a side enemy, ambush the side
// cell an enemy at distance 2 can enter, approach, else drift up / guard.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

constexpr int SIZE = 5, CENTER = 2;
constexpr int EXPLOSION_DAMAGE = 4, KILLS_TO_EXPLODE = 2;
constexpr int NONE = -1;
struct Direction { const char *name; int dr, dc; };
constexpr Direction DIRECTIONS[] = {{"UP", -1, 0}, {"DOWN", 1, 0}, {"LEFT", 0, -1}, {"RIGHT", 0, 1}};  // tie-break order
constexpr int DRIFT = 0;  // UP: the whole army keeps its shape

using Grid = int[SIZE][SIZE];

int manhattan(int r1, int c1, int r2, int c2) { return abs(r1 - r2) + abs(c1 - c2); }
bool isCenter(int r, int c) { return r == CENTER && c == CENTER; }

bool explosionPays(const Grid g) {
    int kills = 0;
    for (int r = CENTER - 1; r <= CENTER + 1; ++r)
        for (int c = CENTER - 1; c <= CENTER + 1; ++c) {
            if (isCenter(r, c)) continue;
            if (g[r][c] > 0) return false;
            if (g[r][c] < 0 && -g[r][c] <= EXPLOSION_DAMAGE) ++kills;
        }
    return kills >= KILLS_TO_EXPLODE;
}

int adjacentAttack(const Grid g) {
    int best = NONE, bestHp = 0;
    for (int d = 0; d < 4; ++d) {
        int v = g[CENTER + DIRECTIONS[d].dr][CENTER + DIRECTIONS[d].dc];
        if (v < 0 && (best == NONE || -v < bestHp)) { best = d; bestHp = -v; }
    }
    return best;
}

// Side cell (empty) that the most enemies at distance 2 can move into.
int ambushAttack(const Grid g) {
    int best = NONE, bestCount = 0;
    for (int d = 0; d < 4; ++d) {
        int r = CENTER + DIRECTIONS[d].dr, c = CENTER + DIRECTIONS[d].dc;
        if (g[r][c] != 0) continue;
        int count = 0;
        for (int er = 0; er < SIZE; ++er)
            for (int ec = 0; ec < SIZE; ++ec)
                if (g[er][ec] < 0 && manhattan(er, ec, CENTER, CENTER) == 2 && manhattan(er, ec, r, c) == 1) ++count;
        if (count > bestCount) { best = d; bestCount = count; }
    }
    return best;
}

bool anyEnemy(const Grid g) {
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            if (g[r][c] < 0) return true;
    return false;
}

// First free side cell that brings us closer to the nearest enemy.
int approach(const Grid g) {
    int er = NONE, ec = NONE;
    for (int r = 0; r < SIZE; ++r)  // nearest, then smallest row, then column
        for (int c = 0; c < SIZE; ++c)
            if (g[r][c] < 0 && (er == NONE || manhattan(r, c, CENTER, CENTER) < manhattan(er, ec, CENTER, CENTER))) {
                er = r;
                ec = c;
            }
    if (er == NONE) return NONE;
    for (int d = 0; d < 4; ++d) {
        int r = CENTER + DIRECTIONS[d].dr, c = CENTER + DIRECTIONS[d].dc;
        if (g[r][c] == 0 && manhattan(er, ec, r, c) < manhattan(er, ec, CENTER, CENTER)) return d;
    }
    return NONE;
}

string decide(const Grid g) {
    if (explosionPays(g)) return "SELFDESTRUCTION";
    int target = adjacentAttack(g);
    if (target == NONE) target = ambushAttack(g);
    if (target != NONE) return string("ATTACK ") + DIRECTIONS[target].name;
    int step = approach(g);
    if (step != NONE) return string("MOVE ") + DIRECTIONS[step].name;
    const Direction &drift = DIRECTIONS[DRIFT];
    if (!anyEnemy(g) && g[CENTER + drift.dr][CENTER + drift.dc] == 0) return string("MOVE ") + drift.name;
    return "GUARD";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int count;
    while (cin >> count) {
        string out;
        for (int i = 0; i < count; ++i) {
            Grid g;
            for (auto &row : g)
                for (int &v : row) cin >> v;
            out += decide(g) + "\n";
        }
        cout << out << flush;
    }
}
