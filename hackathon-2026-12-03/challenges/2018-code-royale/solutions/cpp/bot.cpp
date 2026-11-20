// Code Royale Wood 3 — barracks KNIGHT, TRAIN en boucle.
#include <cmath>
#include <iostream>
#include <vector>

namespace {
struct Site {
    int id, x, y, r;
    int stype = -1, owner = -1, p1 = -1, p2 = -1;
};

double dist(int ax, int ay, int bx, int by) {
    const double dx = ax - bx, dy = ay - by;
    return std::sqrt(dx * dx + dy * dy);
}
}  // namespace

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<Site> sites(n);
    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> sites[i].id >> sites[i].x >> sites[i].y >> sites[i].r)) return 0;
    }

    for (;;) {
        int gold, touched;
        if (!(std::cin >> gold >> touched)) return 0;
        for (int i = 0; i < n; ++i) {
            int sid, i1, i2, st, ow, p1, p2;
            if (!(std::cin >> sid >> i1 >> i2 >> st >> ow >> p1 >> p2)) return 0;
            for (auto& s : sites) {
                if (s.id == sid) {
                    s.stype = st;
                    s.owner = ow;
                    s.p1 = p1;
                    s.p2 = p2;
                    break;
                }
            }
        }

        int nu;
        if (!(std::cin >> nu)) return 0;
        int mx = 200, my = 200, ex = -1, ey = -1;
        for (int i = 0; i < nu; ++i) {
            int x, y, owner, utype, hp;
            if (!(std::cin >> x >> y >> owner >> utype >> hp)) return 0;
            if (utype == -1) {
                if (owner == 0) { mx = x; my = y; }
                else { ex = x; ey = y; }
            }
        }
        if (ex < 0) { ex = 1920 - mx; ey = 1000 - my; }

        // 1. Détruire un barracks ennemi proche par contact.
        const Site* destroy = nullptr;
        double best_d = 250.0;
        for (const auto& s : sites) {
            if (s.stype == 2 && s.owner == 1) {
                const double d = dist(mx, my, s.x, s.y);
                if (d < best_d) { best_d = d; destroy = &s; }
            }
        }
        if (destroy) {
            std::cout << "MOVE " << destroy->x << ' ' << destroy->y << '\n';
        } else {
            // 2. Meilleur site libre pour un barracks KNIGHT.
            int best_sid = -1;
            double best_score = 1e18;
            for (const auto& s : sites) {
                if (s.owner == 1) continue;
                if (s.stype == 2 && s.p1 > 0) continue;
                if (s.stype == 2 && s.p2 == 0) continue;
                const double de = dist(s.x, s.y, ex, ey);
                const double dm = dist(s.x, s.y, mx, my);
                double score = de + 0.6 * dm;
                if (de < 140) score += 10000;
                if (score < best_score) { best_score = score; best_sid = s.id; }
            }
            if (best_sid < 0) std::cout << "WAIT\n";
            else std::cout << "BUILD " << best_sid << " BARRACKS-KNIGHT\n";
        }

        // 3. Entraîner les barracks KNIGHT prêts et abordables.
        std::vector<int> train;
        int left = gold;
        for (const auto& s : sites) {
            if (s.stype == 2 && s.owner == 0 && s.p1 == 0 && s.p2 == 0 && left >= 80) {
                train.push_back(s.id);
                left -= 80;
            }
        }
        std::cout << "TRAIN";
        for (int id : train) std::cout << ' ' << id;
        std::cout << '\n' << std::flush;
    }
    return 0;
}
