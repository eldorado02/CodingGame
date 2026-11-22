// Xmas Rush (Silver) — évaluation des poussées par distance BFS/Manhattan, anti-stale.
// Port du prototype Python v17 validé à 3/5 contre le boss Silver 2215206.
#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr int N = 7;
constexpr int DX[4] = {0, 1, 0, -1};
constexpr int DY[4] = {-1, 0, 1, 0};
constexpr const char* DIRS[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

using Grid = std::array<std::array<int, N>, N>;
using Pos = std::pair<int, int>;

inline int wrap(int v) { int r = v % N; return r < 0 ? r + N : r; }

bool connected(const Grid& g, int x, int y, int d) {
    if (!((g[y][x] >> d) & 1)) return false;
    int nx = x + DX[d], ny = y + DY[d];
    return nx >= 0 && nx < N && ny >= 0 && ny < N &&
           ((g[ny][nx] >> ((d + 2) % 4)) & 1);
}

int bfs_dist(const Grid& g, int sx, int sy, int tx, int ty) {
    if (sx == tx && sy == ty) return 0;
    bool seen[N][N] = {};
    int qx[49], qy[49], qs[49];
    int head = 0, tail = 0;
    qx[tail] = sx; qy[tail] = sy; qs[tail] = 0; ++tail;
    seen[sy][sx] = true;
    while (head < tail) {
        int x = qx[head], y = qy[head], s = qs[head]; ++head;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (nx == tx && ny == ty) return s + 1;
            if (!seen[ny][nx]) {
                seen[ny][nx] = true;
                qx[tail] = nx; qy[tail] = ny; qs[tail] = s + 1; ++tail;
            }
        }
    }
    return 999;
}

void dist_field(const Grid& g, int tx, int ty, int dist[N][N]) {
    for (int y = 0; y < N; ++y) for (int x = 0; x < N; ++x) dist[y][x] = 999;
    dist[ty][tx] = 0;
    int qx[49], qy[49], head = 0, tail = 0;
    qx[tail] = tx; qy[tail] = ty; ++tail;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        int nd = dist[y][x] + 1;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (dist[ny][nx] == 999) {
                dist[ny][nx] = nd;
                qx[tail] = nx; qy[tail] = ny; ++tail;
            }
        }
    }
}

// Meilleur chemin de <= 20 pas collectant le plus de quêtes.
int multi_bfs(const Grid& g, int px, int py, const std::vector<Pos>& targets,
              std::vector<std::string>& moves) {
    moves.clear();
    if (targets.empty()) return 0;
    int fields[3][N][N];
    for (int i = 0; i < static_cast<int>(targets.size()) && i < 3; ++i)
        dist_field(g, targets[i].first, targets[i].second, fields[i]);
    auto exact = [&](int x, int y) {
        int best = 999;
        for (int i = 0; i < static_cast<int>(targets.size()) && i < 3; ++i)
            best = std::min(best, fields[i][y][x]);
        return best;
    };

    constexpr int SZ = N * N * 8;
    std::array<int, SZ> idx{}; idx.fill(-1);
    std::array<int, SZ> pre{}, pdir{}, xx{}, yy{}, mm{}, stp{};
    int start = (py * N + px) * 8;
    idx[start] = 0; pre[0] = -1;
    xx[0] = px; yy[0] = py; mm[0] = 0; stp[0] = 0;
    int q[SZ], head = 0, tail = 0;
    q[tail++] = 0;
    int bestGot = 0, bestSteps = 0, bestState = 0, bestMd = 999;
    int initialMd = exact(px, py);
    bestMd = initialMd;
    while (head < tail) {
        int id = q[head++];
        int x = xx[id], y = yy[id], mask = mm[id], st = stp[id];
        int got = __builtin_popcount(static_cast<unsigned>(mask));
        int md = got < static_cast<int>(targets.size()) ? exact(x, y) : 0;
        if (got > bestGot || (got == bestGot &&
            (md < bestMd || (md == bestMd && st < bestSteps)))) {
            bestGot = got; bestSteps = st; bestState = id; bestMd = md;
        }
        if (st >= 20) continue;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int nm = mask;
            for (int i = 0; i < static_cast<int>(targets.size()); ++i)
                if (targets[i].first == nx && targets[i].second == ny) nm |= 1 << i;
            int code = (ny * N + nx) * 8 + nm;
            if (idx[code] != -1) continue;
            int nid = tail;
            idx[code] = nid; pre[nid] = id; pdir[nid] = d;
            xx[nid] = nx; yy[nid] = ny; mm[nid] = nm; stp[nid] = st + 1;
            q[tail++] = nid;
        }
    }
    std::vector<int> rev;
    for (int cur = bestState; pre[cur] >= 0; cur = pre[cur]) rev.push_back(pdir[cur]);
    for (auto it = rev.rbegin(); it != rev.rend(); ++it) moves.push_back(DIRS[*it]);
    return bestGot;
}

// BFS avec reconstruction de chemin vers une cible précise.
void path_to_goal(const Grid& g, int px, int py, int tx, int ty,
                  std::vector<std::string>& out) {
    out.clear();
    constexpr int SZ = N * N;
    std::array<int, SZ> prev{}; prev.fill(-2);
    std::array<int, SZ> pdir{};
    int q[SZ], head = 0, tail = 0;
    int s = py * N + px;
    prev[s] = -1; q[tail++] = s;
    int target = -1;
    while (head < tail) {
        int id = q[head++];
        if (id == ty * N + tx) { target = id; break; }
        int x = id % N, y = id / N;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            int nid = ny * N + nx;
            if (prev[nid] != -2) continue;
            prev[nid] = id; pdir[nid] = d; q[tail++] = nid;
        }
    }
    if (target < 0) return;
    std::vector<int> rev;
    for (int cur = target; prev[cur] >= 0; cur = prev[cur]) rev.push_back(pdir[cur]);
    for (auto it = rev.rbegin(); it != rev.rend(); ++it) out.push_back(DIRS[*it]);
    if (out.size() > 20) out.resize(20);
}

// Repli Python : cible la plus proche ( premier en cas d'égalité ), puis BFS.
void nearest_path(const Grid& g, int px, int py, const std::vector<Pos>& goals,
                  std::vector<std::string>& out) {
    out.clear();
    int best = 1000000, tx = -1, ty = -1;
    for (const Pos& p : goals) {
        int d = bfs_dist(g, px, py, p.first, p.second);
        if (d < 999 && d < best) { best = d; tx = p.first; ty = p.second; }
    }
    if (tx < 0 || best == 0) return;
    path_to_goal(g, px, py, tx, ty, out);
}

struct SimResult {
    Grid g;
    int nx, ny;
    std::vector<Pos> targets;
    int popped, instant;
};

SimResult simulate(const Grid& grid, int tile, int px, int py,
                   const std::vector<Pos>& board_targets, int hand_count,
                   int line_id, int d) {
    SimResult r;
    r.g = grid;
    r.popped = 0;
    r.instant = 0;
    bool horizontal = (d == 1 || d == 3);
    int insx, insy, popx, popy;
    if (d == 1) {
        insx = 0; insy = line_id; popx = N - 1; popy = line_id;
        for (int x = N - 1; x > 0; --x) r.g[line_id][x] = r.g[line_id][x - 1];
    } else if (d == 3) {
        insx = N - 1; insy = line_id; popx = 0; popy = line_id;
        for (int x = 0; x < N - 1; ++x) r.g[line_id][x] = r.g[line_id][x + 1];
    } else if (d == 0) {
        insx = line_id; insy = N - 1; popx = line_id; popy = 0;
        for (int y = 0; y < N - 1; ++y) r.g[y][line_id] = r.g[y + 1][line_id];
    } else {
        insx = line_id; insy = 0; popx = line_id; popy = N - 1;
        for (int y = N - 1; y > 0; --y) r.g[y][line_id] = r.g[y - 1][line_id];
    }
    r.g[insy][insx] = tile;
    bool on_line = horizontal ? (py == line_id) : (px == line_id);
    r.nx = on_line ? wrap(px + DX[d]) : px;
    r.ny = on_line ? wrap(py + DY[d]) : py;
    for (const Pos& p : board_targets) {
        if (p.first == popx && p.second == popy) {
            ++r.popped;
        } else if ((horizontal && p.second == line_id) ||
                   (!horizontal && p.first == line_id)) {
            r.targets.emplace_back(wrap(p.first + DX[d]), wrap(p.second + DY[d]));
        } else {
            r.targets.push_back(p);
        }
    }
    if (on_line && r.nx == insx && r.ny == insy) r.instant = hand_count;
    if (hand_count && !on_line) r.targets.emplace_back(insx, insy);
    return r;
}

int reachable_count(const Grid& g, int sx, int sy) {
    bool seen[N][N] = {};
    int qx[49], qy[49], head = 0, tail = 0;
    qx[tail] = sx; qy[tail] = sy; ++tail;
    seen[sy][sx] = true;
    int total = 1;
    while (head < tail) {
        int x = qx[head], y = qy[head]; ++head;
        for (int d = 0; d < 4; ++d) {
            if (!connected(g, x, y, d)) continue;
            int nx = x + DX[d], ny = y + DY[d];
            if (!seen[ny][nx]) {
                seen[ny][nx] = true; ++total;
                qx[tail] = nx; qy[tail] = ny; ++tail;
            }
        }
    }
    return total;
}

struct Input {
    int turn_type = 0, px = 0, py = 0, tile = 0, ox = 0, oy = 0, hand = 0;
    Grid grid{};
    std::vector<Pos> board_targets, opp_targets, our_all;
};

bool read_input(Input& in) {
    if (!(std::cin >> in.turn_type)) return false;
    std::string tok;
    const std::string order = "URDL";
    for (int y = 0; y < N; ++y) {
        in.grid[y].fill(0);
        for (int x = 0; x < N; ++x) {
            if (!(std::cin >> tok)) return false;
            int mask = 0;
            for (int d = 0; d < 4; ++d) if (tok[d] == '1') mask |= 1 << d;
            in.grid[y][x] = mask;
        }
    }
    for (int p = 0; p < 2; ++p) {
        int count;
        if (!(std::cin >> count)) return false;
        int& x = (p == 0 ? in.px : in.ox);
        int& y = (p == 0 ? in.py : in.oy);
        if (!(std::cin >> x >> y >> tok)) return false;
        int mask = 0;
        for (int d = 0; d < 4; ++d) if (tok[d] == '1') mask |= 1 << d;
        if (p == 0) in.tile = mask;
    }
    in.board_targets.clear(); in.opp_targets.clear(); in.our_all.clear(); in.hand = 0;
    int n_items;
    if (!(std::cin >> n_items)) return false;
    std::vector<std::string> names[2];
    std::vector<Pos> positions[2];
    for (int i = 0; i < n_items; ++i) {
        std::string name; int x, y, pid;
        if (!(std::cin >> name >> x >> y >> pid)) return false;
        if (pid < 0 || pid > 1) continue;
        names[pid].push_back(name);
        positions[pid].emplace_back(x, y);
    }
    int n_quests;
    if (!(std::cin >> n_quests)) return false;
    std::vector<std::string> quests[2];
    for (int i = 0; i < n_quests; ++i) {
        std::string name; int pid;
        if (!(std::cin >> name >> pid)) return false;
        if (pid >= 0 && pid <= 1) quests[pid].push_back(name);
    }
    for (int pid = 0; pid < 2; ++pid) {
        for (size_t i = 0; i < names[pid].size(); ++i) {
            int x = positions[pid][i].first, y = positions[pid][i].second;
            if (x == -1) { if (pid == 0) ++in.hand; continue; }
            if (x == -2) continue;
            bool revealed = false;
            for (const std::string& q : quests[pid]) if (q == names[pid][i]) { revealed = true; break; }
            if (pid == 0) in.our_all.emplace_back(x, y);
            if (revealed) {
                if (pid == 0) in.board_targets.emplace_back(x, y);
                else in.opp_targets.emplace_back(x, y);
            }
        }
    }
    return true;
}

struct Candidate {
    double score;
    int instant, popped, conn, line, d;
    std::string cmd;
};

bool cand_better(const Candidate& a, const Candidate& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.instant != b.instant) return a.instant > b.instant;
    if (a.popped != b.popped) return a.popped < b.popped;
    if (a.conn != b.conn) return a.conn > b.conn;
    if (a.line != b.line) return a.line < b.line;
    if (a.d != b.d) return a.d < b.d;
    return a.cmd > b.cmd;
}

std::string best_push(const Input& in, const std::string& last_push, int stale) {
    std::vector<Candidate> cands;
    int last_line = -1; bool last_horiz = false;
    if (last_push.size() >= 9 && last_push.compare(0, 5, "PUSH ") == 0) {
        std::string rest = last_push.substr(5);
        size_t sp = rest.find(' ');
        if (sp != std::string::npos) {
            last_line = std::atoi(rest.substr(0, sp).c_str());
            std::string dir = rest.substr(sp + 1);
            last_horiz = (dir == "RIGHT" || dir == "LEFT");
        }
    }
    for (int pass = 0; pass < 2; ++pass) {
        bool horizontal = (pass == 0);
        for (int line = 0; line < N; ++line) {
            for (int step = 0; step < 2; ++step) {
                int d = horizontal ? (step == 0 ? 1 : 3) : (step == 0 ? 0 : 2);
                if (last_line >= 0 && last_line == line && last_horiz == horizontal) continue;
                SimResult s = simulate(in.grid, in.tile, in.px, in.py,
                                       in.board_targets, in.hand, line, d);
                SimResult o = simulate(in.grid, in.tile, in.ox, in.oy,
                                       in.opp_targets, 0, line, d);
                double score = 1000.0 * s.instant - 100.0 * s.popped;
                for (const Pos& t : s.targets) {
                    int dist = bfs_dist(s.g, s.nx, s.ny, t.first, t.second);
                    if (dist <= 20) score += 1000 - 10 * dist;
                    else if (dist < 999) score += 200.0 / (dist + 1);
                    else score += 500.0 - 30.0 * (std::abs(s.nx - t.first) +
                                                  std::abs(s.ny - t.second));
                }
                score += 2.0 * reachable_count(s.g, s.nx, s.ny);
                for (const Pos& t : o.targets) {
                    int dist = bfs_dist(s.g, o.nx, o.ny, t.first, t.second);
                    if (dist <= 20) score -= 10;
                }
                for (const Pos& t : in.our_all) {
                    bool is_target = false;
                    for (const Pos& q : in.board_targets) if (q == t) { is_target = true; break; }
                    if (is_target) continue;
                    int dist = bfs_dist(s.g, s.nx, s.ny, t.first, t.second);
                    if (dist <= 20) score += 20;
                    else if (dist < 999) score += 20.0 / (dist + 1);
                }
                int conn = 0;
                for (int k = 0; k < 4; ++k) if (connected(s.g, s.nx, s.ny, k)) ++conn;
                score += conn;
                Candidate c;
                c.score = score; c.instant = s.instant; c.popped = s.popped;
                c.conn = conn; c.line = line; c.d = d;
                c.cmd = "PUSH " + std::to_string(line) + " " + DIRS[d];
                cands.push_back(c);
            }
        }
    }
    std::sort(cands.begin(), cands.end(), cand_better);
    if (stale >= 3 && cands.size() > 1) {
        for (const Candidate& c : cands) if (c.cmd != last_push) return c.cmd;
    }
    return cands[0].cmd;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string last_push;
    int stale = 0;
    Input in;
    while (read_input(in)) {
        if (in.turn_type == 1) {
            std::vector<std::string> moves;
            multi_bfs(in.grid, in.px, in.py, in.board_targets, moves);
            if (moves.empty() && !in.board_targets.empty())
                nearest_path(in.grid, in.px, in.py, in.board_targets, moves);
            if (moves.empty() && in.hand) {
                std::vector<Pos> edge;
                for (int x = 0; x < N; ++x) { edge.emplace_back(x, 0); edge.emplace_back(x, N - 1); }
                for (int y = 0; y < N; ++y) { edge.emplace_back(0, y); edge.emplace_back(N - 1, y); }
                nearest_path(in.grid, in.px, in.py, edge, moves);
            }
            if (moves.empty()) {
                std::cout << "PASS" << std::endl;
            } else {
                std::cout << "MOVE";
                for (const std::string& m : moves) std::cout << ' ' << m;
                std::cout << std::endl;
            }
        } else {
            std::string cmd = best_push(in, last_push, stale);
            if (cmd == last_push) ++stale; else stale = 0;
            last_push = cmd;
            std::cout << cmd << std::endl;
        }
        std::cout << std::flush;
    }
    return 0;
}
