// Shobu — évaluation 1 ply + meilleure réponse adverse + position bord.
#include <algorithm>
#include <array>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

constexpr int WIN = 1000000000;
constexpr int BEAM = 24;
const int DIRS[8][2] = {{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}};

struct Move {
    int px, py, ax, ay, dx, dy;
    bool operator<(const Move& o) const {
        if (px != o.px) return px < o.px;
        if (py != o.py) return py < o.py;
        if (ax != o.ax) return ax < o.ax;
        if (ay != o.ay) return ay < o.ay;
        if (dx != o.dx) return dx < o.dx;
        return dy < o.dy;
    }
    bool operator==(const Move& o) const {
        return px==o.px && py==o.py && ax==o.ax && ay==o.ay && dx==o.dx && dy==o.dy;
    }
};

using Cells = array<int, 64>;

int board_of(int x, int y) { return 2 * (y / 4) + x / 4; }
bool is_dark(int b) { return b % 2 == 0; }
bool is_home(int b, int color) { return color == 0 ? b >= 2 : b < 2; }

bool same_board(int x, int y, int ux, int uy, int k) {
    int nx = x + ux * k, ny = y + uy * k;
    return nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && nx / 4 == x / 4 && ny / 4 == y / 4;
}

// Identique à Board.legalMoves() du referee.
vector<Move> gen_moves(const Cells& c, int me) {
    vector<Move> out;
    int op = 1 - me;
    for (int py = 0; py < 8; ++py) {
        for (int px = 0; px < 8; ++px) {
            if (c[py*8+px] != me || !is_home(board_of(px,py), me)) continue;
            bool passive_dark = is_dark(board_of(px,py));
            for (auto& d : DIRS) {
                int ux = d[0], uy = d[1];
                for (int k = 1; k <= 2; ++k) {
                    if (!same_board(px,py,ux,uy,k) || c[(py+uy*k)*8 + px+ux*k] != -1) break;
                    for (int ay = 0; ay < 8; ++ay) {
                        for (int ax = 0; ax < 8; ++ax) {
                            if (c[ay*8+ax] != me || is_dark(board_of(ax,ay)) == passive_dark) continue;
                            if (!same_board(ax,ay,ux,uy,k)) continue;
                            int n = 0; bool ok = true;
                            for (int j = 1; j <= k; ++j) {
                                int v = c[(ay+uy*j)*8 + ax+ux*j];
                                if (v == me) { ok = false; break; }
                                if (v == op) ++n;
                            }
                            if (!ok || n > 1) continue;
                            if (n == 1 && same_board(ax,ay,ux,uy,k+1)) {
                                if (c[(ay+uy*(k+1))*8 + ax+ux*(k+1)] != -1) continue;
                            }
                            out.push_back({px,py,ax,ay,ux*k,uy*k});
                        }
                    }
                }
            }
        }
    }
    return out;
}

Cells apply_move(const Cells& cells, int me, const Move& m) {
    Cells g = cells;
    int op = 1 - me;
    int k = max(abs(m.dx), abs(m.dy));
    int ux = (m.dx > 0) - (m.dx < 0);
    int uy = (m.dy > 0) - (m.dy < 0);
    g[m.py*8+m.px] = -1;
    g[(m.py+m.dy)*8 + m.px+m.dx] = me;
    for (int j = 1; j <= k; ++j) {
        int x = m.ax + ux*j, y = m.ay + uy*j;
        if (g[y*8+x] == op) {
            g[y*8+x] = -1;
            int tx = m.ax + ux*(k+1), ty = m.ay + uy*(k+1);
            if (same_board(m.ax,m.ay,ux,uy,k+1)) g[ty*8+tx] = op;
            break;
        }
    }
    g[m.ay*8+m.ax] = -1;
    g[(m.ay+m.dy)*8 + m.ax+m.dx] = me;
    return g;
}

int stones(const Cells& g, int b, int color) {
    int bx = (b % 2) * 4, by = (b / 2) * 4, n = 0;
    for (int y = by; y < by+4; ++y)
        for (int x = bx; x < bx+4; ++x)
            if (g[y*8+x] == color) ++n;
    return n;
}

int evaluate(const Cells& g, int me) {
    int op = 1 - me;
    int om[4], mm[4];
    int mn_o = 99, mn_m = 99;
    for (int b = 0; b < 4; ++b) {
        om[b] = stones(g, b, op);
        mm[b] = stones(g, b, me);
        mn_o = min(mn_o, om[b]);
        mn_m = min(mn_m, mm[b]);
    }
    if (mn_o == 0) return WIN;
    if (mn_m == 0) return -WIN;
    int score = 40 * (accumulate(mm, mm+4, 0) - accumulate(om, om+4, 0));
    score += 300 * ((4 - mn_o) * (4 - mn_o));
    score -= 250 * ((4 - mn_m) * (4 - mn_m));
    for (int b = 0; b < 4; ++b) score += 15 * (mm[b] - om[b]);
    int our_edge = 0, op_edge = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int c = g[y*8+x];
            if (c == -1) continue;
            int lx = x % 4, ly = y % 4;
            if (lx == 0 || lx == 3 || ly == 0 || ly == 3) {
                if (c == me) ++our_edge; else ++op_edge;
            }
        }
    }
    score += 25 * (op_edge - our_edge);
    return score;
}

Move choose(const Cells& g, int me) {
    vector<Move> moves = gen_moves(g, me);
    struct Scored { int s; Move m; Cells ng; };
    vector<Scored> scored;
    for (const auto& m : moves) {
        Cells ng = apply_move(g, me, m);
        scored.push_back({evaluate(ng, me), m, ng});
    }
    sort(scored.begin(), scored.end(), [](const Scored& a, const Scored& b) {
        if (a.s != b.s) return a.s > b.s;
        return a.m < b.m;
    });
    Move best{};
    int best_score = -2 * WIN;
    bool have = false;
    for (int idx = 0; idx < BEAM && idx < (int)scored.size(); ++idx) {
        int s = scored[idx].s;
        Move m = scored[idx].m;
        Cells ng = scored[idx].ng;
        if (s >= WIN) return m;
        vector<Move> replies = gen_moves(ng, 1 - me);
        if (replies.empty()) return m;
        int worst = WIN;
        // Les 24 premières réponses, dans l'ordre de génération du referee.
        for (int ri = 0; ri < BEAM && ri < (int)replies.size(); ++ri) {
            Cells rg = apply_move(ng, 1 - me, replies[ri]);
            worst = min(worst, evaluate(rg, me));
        }
        if (!have || worst > best_score || (worst == best_score && m < best)) {
            best_score = worst;
            best = m;
            have = true;
        }
    }
    return have ? best : moves[0];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int round_no, my_color;
    while (cin >> round_no >> my_color) {
        Cells cells{};
        for (int y = 0; y < 8; ++y) {
            string row;
            cin >> row;
            for (int x = 0; x < 8; ++x) {
                cells[y*8+x] = row[x] == 'b' ? 0 : (row[x] == 'w' ? 1 : -1);
            }
        }
        string last;
        cin >> last;
        if (last != "-" && last != "SWAP") {
            string tmp;
            for (int i = 0; i < 5; ++i) cin >> tmp;
        }
        int n;
        cin >> n;
        vector<Move> actions;
        bool can_swap = false;
        for (int i = 0; i < n; ++i) {
            string a;
            cin >> a;
            if (a == "SWAP") {
                can_swap = true;
            } else {
                Move m{};
                // a contient le premier entier, il reste 5 à lire
                m.px = stoi(a);
                cin >> m.py >> m.ax >> m.ay >> m.dx >> m.dy;
                actions.push_back(m);
            }
        }
        if (can_swap) {
            cout << "SWAP" << '\n' << flush;
            continue;
        }
        Move m = choose(cells, my_color);
        bool found = false;
        for (const auto& a : actions) if (a == m) { found = true; break; }
        if (!found && !actions.empty()) m = actions[0];
        cout << m.px << ' ' << m.py << ' ' << m.ax << ' ' << m.ay << ' '
             << m.dx << ' ' << m.dy << '\n' << flush;
    }
    return 0;
}
