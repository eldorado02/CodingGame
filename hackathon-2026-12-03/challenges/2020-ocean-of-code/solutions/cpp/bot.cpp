// Ocean of Code (Wood 3) — croyance adversaire + torpedo.
#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr int N = 15;
const char DIR_NAME[4] = {'N', 'E', 'S', 'W'};
const int DX[4] = {0, 1, 0, -1};
const int DY[4] = {-1, 0, 1, 0};

using GridBool = std::array<std::array<bool, N>, N>;
using GridInt = std::array<std::array<int, N>, N>;

struct Bot {
    GridBool water{}, visited{}, bel{};
    int prev_opp_life = -1;
    int last_x = -1, last_y = -1;
    int cnt = 0, sumx = 0, sumy = 0;
};

void clear_bel(Bot &b) {
    for (auto &row : b.bel) row.fill(false);
}

void reset_bel(Bot &b) {
    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x) b.bel[y][x] = b.water[y][x];
}

bool bel_empty(const Bot &b) {
    for (const auto &row : b.bel)
        for (bool v : row) if (v) return false;
    return true;
}

void reset_if_empty(Bot &b) {
    if (bel_empty(b)) reset_bel(b);
}

int sector_of(int x, int y) {
    return x / 5 + 1 + (y / 5) * 3;
}

GridInt bfs(const GridBool &water, int sx, int sy) {
    GridInt dist;
    for (auto &row : dist) row.fill(-1);
    int qx[N * N], qy[N * N], head = 0, tail = 0;
    dist[sy][sx] = 0;
    qx[tail] = sx; qy[tail] = sy; tail++;
    while (head < tail) {
        int x = qx[head], y = qy[head]; head++;
        for (int k = 0; k < 4; ++k) {
            int nx = x + DX[k], ny = y + DY[k];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!water[ny][nx] || dist[ny][nx] != -1) continue;
            dist[ny][nx] = dist[y][x] + 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }
    return dist;
}

void choose_start(const Bot &b, int &out_x, int &out_y) {
    int bx = -1, by = -1, bdeg = -1, bcenter = 1000000;
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            if (!b.water[y][x]) continue;
            int deg = 0;
            for (int k = 0; k < 4; ++k) {
                int nx = x + DX[k], ny = y + DY[k];
                if (nx >= 0 && nx < N && ny >= 0 && ny < N && b.water[ny][nx]) ++deg;
            }
            int center = std::abs(x - 7) + std::abs(y - 7);
            if (deg > bdeg ||
                (deg == bdeg && center < bcenter) ||
                (deg == bdeg && center == bcenter &&
                 (bx < 0 || x < bx || (x == bx && y < by)))) {
                bx = x; by = y; bdeg = deg; bcenter = center;
            }
        }
    }
    out_x = bx; out_y = by;
}

void compute_centroid(Bot &b) {
    b.cnt = b.sumx = b.sumy = 0;
    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x)
            if (b.bel[y][x]) { ++b.cnt; b.sumx += x; b.sumy += y; }
    if (b.cnt == 0) {
        reset_bel(b);
        for (int y = 0; y < N; ++y)
            for (int x = 0; x < N; ++x)
                if (b.bel[y][x]) { ++b.cnt; b.sumx += x; b.sumy += y; }
    }
}

bool best_dir(const Bot &b, int px, int py, bool ignore_visited,
              int &nx_out, int &ny_out, int &idx_out) {
    bool found = false;
    int best_free = -1, best_dist = 0, best_idx = 0, bnx = 0, bny = 0;
    for (int k = 0; k < 4; ++k) {
        int nx = px + DX[k], ny = py + DY[k];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N || !b.water[ny][nx]) continue;
        if (!ignore_visited && b.visited[ny][nx]) continue;
        int free = 0;
        for (int j = 0; j < 4; ++j) {
            int ax = nx + DX[j], ay = ny + DY[j];
            if (ax < 0 || ax >= N || ay < 0 || ay >= N || !b.water[ay][ax]) continue;
            bool blocked = ignore_visited ? (ax == px && ay == py) : b.visited[ay][ax];
            if (!blocked) ++free;
        }
        int cdist = std::abs(2 * b.cnt * nx - b.sumx) + std::abs(2 * b.cnt * ny - b.sumy);
        if (!found || free > best_free ||
            (free == best_free && cdist < best_dist) ||
            (free == best_free && cdist == best_dist && k < best_idx)) {
            found = true; best_free = free; best_dist = cdist; best_idx = k;
            bnx = nx; bny = ny;
        }
    }
    if (!found) return false;
    nx_out = bnx; ny_out = bny; idx_out = best_idx;
    return true;
}

std::vector<std::string> split_orders(const std::string &s) {
    std::vector<std::string> out;
    if (s == "NA") return out;
    size_t start = 0;
    while (true) {
        size_t pos = s.find('|', start);
        out.push_back(s.substr(start, pos == std::string::npos ? std::string::npos : pos - start));
        if (pos == std::string::npos) break;
        start = pos + 1;
    }
    return out;
}

std::vector<std::string> tokens(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string t;
    while (iss >> t) out.push_back(t);
    return out;
}
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Bot b;
    std::string line;
    if (!std::getline(std::cin, line)) return 0;
    auto head = tokens(line);
    if (head.size() == 3) {
        const int hh = std::stoi(head[1]);
        for (int y = 0; y < hh; ++y) {
            std::getline(std::cin, line);
            for (int x = 0; x < N; ++x) b.water[y][x] = line[x] == '.';
        }
        reset_bel(b);
        int sx, sy;
        choose_start(b, sx, sy);
        b.visited[sy][sx] = true;
        std::cout << sx << ' ' << sy << '\n' << std::flush;
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        auto f = tokens(line);
        if (f.size() != 8) break;
        const int px = std::stoi(f[0]), py = std::stoi(f[1]);
        const int opp_life = std::stoi(f[3]);
        const int cooldown = std::stoi(f[4]);
        std::getline(std::cin, line);  // sonar
        std::string orders;
        std::getline(std::cin, orders);

        if (b.last_x >= 0 && b.prev_opp_life >= 0) {
            const int delta = b.prev_opp_life - opp_life;
            const int tx = b.last_x, ty = b.last_y;
            if (delta == 2) {
                clear_bel(b);
                if (b.water[ty][tx]) b.bel[ty][tx] = true;
            } else if (delta == 1) {
                clear_bel(b);
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = tx + dx, ny = ty + dy;
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N && b.water[ny][nx])
                            b.bel[ny][nx] = true;
                    }
            } else if (delta == 0) {
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = tx + dx, ny = ty + dy;
                        if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                            b.bel[ny][nx] = false;
                    }
            }
        }
        b.last_x = b.last_y = -1;
        reset_if_empty(b);

        for (const auto &raw : split_orders(orders)) {
            auto tok = tokens(raw);
            if (!tok.empty()) {
                if (tok[0] == "MOVE" && tok.size() >= 2) {
                    int k = -1;
                    for (int j = 0; j < 4; ++j)
                        if (tok[1][0] == DIR_NAME[j]) { k = j; break; }
                    if (k >= 0) {
                        GridBool nb;
                        for (auto &row : nb) row.fill(false);
                        for (int y = 0; y < N; ++y)
                            for (int x = 0; x < N; ++x) {
                                if (!b.bel[y][x]) continue;
                                int nx = x + DX[k], ny = y + DY[k];
                                if (nx >= 0 && nx < N && ny >= 0 && ny < N && b.water[ny][nx])
                                    nb[ny][nx] = true;
                            }
                        b.bel = nb;
                    }
                } else if (tok[0] == "SURFACE" && tok.size() >= 2) {
                    const int s = std::stoi(tok[1]);
                    for (int y = 0; y < N; ++y)
                        for (int x = 0; x < N; ++x)
                            if (b.bel[y][x] && sector_of(x, y) != s) b.bel[y][x] = false;
                } else if (tok[0] == "TORPEDO" && tok.size() >= 3) {
                    const int ax = std::stoi(tok[1]), ay = std::stoi(tok[2]);
                    auto d = bfs(b.water, ax, ay);
                    for (int y = 0; y < N; ++y)
                        for (int x = 0; x < N; ++x)
                            if (b.bel[y][x] && (d[y][x] < 0 || d[y][x] > 4))
                                b.bel[y][x] = false;
                }
            }
            reset_if_empty(b);
        }

        compute_centroid(b);

        b.visited[py][px] = true;
        std::vector<std::string> actions;
        int final_x = px, final_y = py;
        int nx, ny, kidx;
        if (best_dir(b, px, py, false, nx, ny, kidx)) {
            actions.push_back(std::string("MOVE ") + DIR_NAME[kidx] + " TORPEDO");
            final_x = nx; final_y = ny;
            b.visited[ny][nx] = true;
        } else {
            for (auto &row : b.visited) row.fill(false);
            b.visited[py][px] = true;
            actions.push_back("SURFACE");
            if (best_dir(b, px, py, true, nx, ny, kidx)) {
                actions.push_back(std::string("MOVE ") + DIR_NAME[kidx] + " TORPEDO");
                final_x = nx; final_y = ny;
                b.visited[py][px] = true;
                b.visited[ny][nx] = true;
            }
        }

        if (cooldown == 0) {
            auto dstart = bfs(b.water, px, py);
            int best_score = -1, bx = -1, by = -1;
            for (int y = 0; y < N; ++y) {
                for (int x = 0; x < N; ++x) {
                    if (!b.water[y][x] || dstart[y][x] < 0 || dstart[y][x] > 4) continue;
                    if (std::abs(x - final_x) <= 1 && std::abs(y - final_y) <= 1) continue;
                    int score = b.bel[y][x] ? 2 : 0;
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) continue;
                            int ax = x + dx, ay = y + dy;
                            if (ax >= 0 && ax < N && ay >= 0 && ay < N && b.bel[ay][ax])
                                ++score;
                        }
                    if (score > best_score ||
                        (score == best_score && (bx < 0 || x < bx || (x == bx && y < by)))) {
                        best_score = score; bx = x; by = y;
                    }
                }
            }
            if (best_score > 0) {
                actions.push_back("TORPEDO " + std::to_string(bx) + " " + std::to_string(by));
                b.last_x = bx; b.last_y = by;
            }
        }

        for (size_t i = 0; i < actions.size(); ++i) {
            if (i) std::cout << '|';
            std::cout << actions[i];
        }
        std::cout << '\n' << std::flush;
        b.prev_opp_life = opp_life;
    }
    return 0;
}
