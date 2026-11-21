// Cultist Wars — conversions, neutralisation des menaces, tirs.
#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct Unit { int id, type, hp, x, y, owner; };

static int W, H;
static vector<string> grid;

static int manhattan(const Unit &a, const Unit &b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Bresenham du moteur : tracé depuis la coordonnée Y la plus basse.
// Renvoie l'indice de l'unité touchée, ou -1.
static int ray_victim(const Unit &shooter, const Unit &target,
                      const vector<Unit> &units) {
    int x0, y0, x1, y1;
    bool backward;
    if (shooter.y < target.y) {
        x0 = shooter.x; y0 = shooter.y; x1 = target.x; y1 = target.y;
        backward = false;
    } else {
        x0 = target.x; y0 = target.y; x1 = shooter.x; y1 = shooter.y;
        backward = true;
    }
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx - dy, cx = x0, cy = y0;
    int bx = x1, by = y1;
    while (true) {
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; cx += sx; }
        if (e2 < dx) { err += dx; cy += sy; }
        if (cx == x1 && cy == y1) break;
        if (cx < 0 || cx >= W || cy < 0 || cy >= H) return -1;
        int blocker = -1;
        if (grid[cy][cx] == 'x') blocker = -2;
        else {
            for (int i = 0; i < (int)units.size(); ++i)
                if (units[i].hp > 0 && units[i].x == cx && units[i].y == cy) {
                    blocker = i; break;
                }
        }
        if (blocker != -1) {
            if (backward) { bx = cx; by = cy; }
            else if (blocker >= 0) return blocker;
            else return -1;
        }
    }
    for (int i = 0; i < (int)units.size(); ++i)
        if (units[i].hp > 0 && units[i].x == bx && units[i].y == by) return i;
    return -1;
}

static int bfs_distance(const Unit &start, const Unit &goal) {
    if (start.x == goal.x && start.y == goal.y) return 0;
    vector<vector<int>> seen(H, vector<int>(W, 0));
    vector<array<int, 3>> q{{start.x, start.y, 0}};
    seen[start.y][start.x] = 1;
    const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0};
    for (size_t i = 0; i < q.size(); ++i) {
        int x = q[i][0], y = q[i][1], d = q[i][2];
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (seen[ny][nx] || grid[ny][nx] == 'x') continue;
            if (nx == goal.x && ny == goal.y) return d + 1;
            seen[ny][nx] = 1;
            q.push_back({nx, ny, d + 1});
        }
    }
    return 1000000;
}

using ShotKey = tuple<int, int, int, int, int>;

static bool better_shot(const ShotKey &a, const ShotKey &b) { return a < b; }

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int my_id;
    if (!(cin >> my_id)) return 0;
    cin >> W >> H;
    grid.resize(H);
    for (int y = 0; y < H; ++y) cin >> grid[y];

    int n;
    while (cin >> n) {
        vector<Unit> units(n);
        for (int i = 0; i < n; ++i)
            cin >> units[i].id >> units[i].type >> units[i].hp
                >> units[i].x >> units[i].y >> units[i].owner;

        int leader = -1;
        vector<int> mine, cultists, enemies, enemy_cultists, neutrals;
        for (int i = 0; i < n; ++i) {
            if (units[i].hp <= 0) continue;
            if (units[i].owner == my_id) {
                mine.push_back(i);
                if (units[i].type == 1) leader = i;
                else cultists.push_back(i);
            } else if (units[i].owner == 2) neutrals.push_back(i);
            else {
                enemies.push_back(i);
                if (units[i].type == 0) enemy_cultists.push_back(i);
            }
        }

        auto find_shot = [&](const vector<int> *allowed) {
            ShotKey best{9, 0, 0, 0, 0};
            int shooter = -1, victim = -1;
            for (int c : cultists) {
                for (int e : enemies) {
                    if (allowed && find(allowed->begin(), allowed->end(), e) == allowed->end()) continue;
                    if (manhattan(units[c], units[e]) > 6) continue;
                    int v = ray_victim(units[c], units[e], units);
                    if (v < 0 || units[v].owner == my_id || units[v].owner == 2) continue;
                    int d = manhattan(units[c], units[v]);
                    int dmg = 7 - d;
                    ShotKey key{dmg >= units[v].hp ? 0 : 1, -dmg, d, units[v].id, units[c].id};
                    if (better_shot(key, best)) { best = key; shooter = c; victim = v; }
                }
            }
            return make_pair(shooter, victim);
        };

        // 1. Neutraliser un cultiste ennemi qui menace nos unités.
        vector<int> threats;
        for (int e : enemy_cultists) {
            bool found = false;
            for (int u : mine) {
                if (manhattan(units[e], units[u]) > 6) continue;
                int v = ray_victim(units[e], units[u], units);
                if (v == u) { found = true; break; }
            }
            if (found) threats.push_back(e);
        }
        if (!threats.empty()) {
            auto [c, v] = find_shot(&threats);
            if (c >= 0) {
                cout << units[c].id << " SHOOT " << units[v].id << '\n' << flush;
                continue;
            }
        }

        // 2. Le leader convertit les neutres de sa moitié.
        if (leader >= 0) {
            int mid = W / 2, target = -1, best_d = 1000000;
            for (int idx : neutrals) {
                if ((my_id == 0 && units[idx].x > mid) ||
                    (my_id == 1 && units[idx].x < mid)) continue;
                int d = bfs_distance(units[leader], units[idx]);
                if (d < best_d || (d == best_d && target >= 0 &&
                                   units[idx].id < units[target].id)) {
                    best_d = d; target = idx;
                }
            }
            if (target >= 0) {
                cout << units[leader].id << " CONVERT " << units[target].id << '\n' << flush;
                continue;
            }
        }

        // 3. Vol d'un cultiste ennemi voisin.
        if (leader >= 0) {
            int target = -1, best_d = 1000000;
            for (int e : enemy_cultists) {
                int d = bfs_distance(units[leader], units[e]);
                if (d <= 2 && (d < best_d || (d == best_d && target >= 0 &&
                               units[e].id < units[target].id))) {
                    best_d = d; target = e;
                }
            }
            if (target >= 0) {
                cout << units[leader].id << " CONVERT " << units[target].id << '\n' << flush;
                continue;
            }
        }

        // 4. Tir libre.
        {
            auto [c, v] = find_shot(nullptr);
            if (c >= 0) {
                cout << units[c].id << " SHOOT " << units[v].id << '\n' << flush;
                continue;
            }
        }

        // 5. Un SHOOT hors portée déplace le cultiste vers la cible.
        if (!cultists.empty() && !enemies.empty()) {
            int c = cultists[0], e = enemies[0], best_d = 1000000;
            for (int ci : cultists) {
                int md = 1000000;
                for (int ei : enemies) md = min(md, manhattan(units[ci], units[ei]));
                if (md < best_d || (md == best_d && units[ci].id < units[c].id)) {
                    best_d = md; c = ci;
                }
            }
            best_d = 1000000;
            for (int ei : enemies) {
                int d = manhattan(units[c], units[ei]);
                if (d < best_d || (d == best_d && units[ei].id < units[e].id)) {
                    best_d = d; e = ei;
                }
            }
            cout << units[c].id << " SHOOT " << units[e].id << '\n' << flush;
            continue;
        }
        cout << "WAIT\n" << flush;
    }
    return 0;
}
