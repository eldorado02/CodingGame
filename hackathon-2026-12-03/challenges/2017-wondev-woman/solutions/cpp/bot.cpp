// Wondev Woman — arbitre version 2 (2 unités, PUSH&BUILD possible).
//
// Protocole (stub officiel, vérifié sur des test games réels) :
//   init  : size, unitsPerPlayer (= 2 ici, malgré l'énoncé archivé qui dit 1)
//   tour  : size lignes de grille ('.' = trou, '0'..'3' = hauteur),
//           unitsPerPlayer lignes "x y" pour nous, autant pour l'adversaire,
//           puis legalActions, puis legalActions lignes
//           "atype index dir1 dir2" (atype = MOVE&BUILD ou PUSH&BUILD).
//   sortie : UNE seule ligne. Le referee ne lit que outputs[0] : on joue la
//            meilleure action parmi TOUTES les unités, une seule par tour.
//
// Décision : marcher sur une case de hauteur 3 marque 1 point (referee :
// `if (targetHeight == FINAL_HEIGHT - 1) result.scorePoint = true`). On
// construit donc des tremplins 2 -> 3 à côté de notre unité, on reste adjacent
// aux cases de hauteur 3, et on détruit (3 -> 4) celles dont l'adversaire
// pourrait profiter.

#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

// Ordre identique au Python : N, NE, E, SE, S, SW, W, NW.
const std::array<std::pair<int, int>, 8> kDelta = {{
    {0, -1}, {1, -1}, {1, 0}, {1, 1},
    {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
}};

int delta_index(const std::string& dir) {
    static const std::array<std::string, 8> names = {
        "N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    for (int i = 0; i < 8; ++i) {
        if (names[i] == dir) return i;
    }
    return 0;
}

// Hauteur d'une case : -1 si trou / hors plateau.
int height(const std::vector<std::vector<int>>& grid, int x, int y) {
    if (y < 0 || y >= static_cast<int>(grid.size()) ||
        x < 0 || x >= static_cast<int>(grid[0].size())) {
        return -1;
    }
    return grid[y][x];
}

struct Action {
    std::string type;
    int index;
    int dir1;
    int dir2;
};

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size = 0;
    int units_per_player = 0;
    if (!(std::cin >> size >> units_per_player)) return 0;

    while (true) {
        std::vector<std::vector<int>> grid(size, std::vector<int>(size));
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                char c = 0;
                if (!(std::cin >> c)) return 0;
                grid[y][x] = (c == '.') ? -1 : (c - '0');
            }
        }

        std::vector<std::pair<int, int>> ours(units_per_player);
        std::vector<std::pair<int, int>> foes(units_per_player);
        for (auto& p : ours) {
            if (!(std::cin >> p.first >> p.second)) return 0;
        }
        for (auto& p : foes) {
            if (!(std::cin >> p.first >> p.second)) return 0;
        }

        int legal_count = 0;
        if (!(std::cin >> legal_count)) return 0;
        std::vector<Action> actions;
        actions.reserve(legal_count);
        for (int i = 0; i < legal_count; ++i) {
            Action a;
            std::string d1, d2;
            if (!(std::cin >> a.type >> a.index >> d1 >> d2)) return 0;
            a.dir1 = delta_index(d1);
            a.dir2 = delta_index(d2);
            actions.push_back(std::move(a));
        }

        // score > token : départage déterministe en cas d'égalité.
        std::optional<std::pair<int, std::string>> best;
        for (const auto& a : actions) {
            if (a.index < 0 || a.index >= units_per_player) continue;
            const auto [ux, uy] = ours[a.index];
            const auto [dx, dy] = kDelta[a.dir1];
            const int mx = ux + dx;
            const int my = uy + dy;
            const int h_dest = height(grid, mx, my);

            const auto [bx, by] = kDelta[a.dir2];
            const int cx = mx + bx;
            const int cy = my + by;
            const int h_build = height(grid, cx, cy);

            int score = 0;
            if (h_dest == 3) {
                score += 1000000;  // marche sur une case niveau 3 = 1 point
            } else if (h_dest >= 0) {
                score += 400 * h_dest;
            }
            if (h_dest >= 0) {
                for (const auto& [ex, ey] : kDelta) {
                    if (height(grid, mx + ex, my + ey) == 3) {
                        score += 1500;  // adjacent à une case de score
                        break;
                    }
                }
            }
            if (h_build == 3) {
                bool near_foe = false;
                for (const auto& [fx, fy] : foes) {
                    if (std::abs(cx - fx) <= 1 && std::abs(cy - fy) <= 1) {
                        near_foe = true;
                        break;
                    }
                }
                score += near_foe ? 50000 : -40;  // détruire un tremplin adverse
            } else if (h_build == 2) {
                const bool adjacent = (std::abs(cx - mx) + std::abs(cy - my) <= 1);
                score += adjacent ? 3000 : 500;   // créer une case de score
            } else if (h_build == 1) {
                score += 100;
            } else if (h_build == 0) {
                score += 10;
            }

            // On reconstruit les noms de directions pour la sortie.
            static const std::array<std::string, 8> names = {
                "N", "NE", "E", "SE", "S", "SW", "W", "NW"};
            const std::string token = a.type + " " + std::to_string(a.index) +
                                      " " + names[a.dir1] + " " + names[a.dir2];
            // Même départage que Python : stricement supérieur, donc la
            // première action légale (ordre du referee) gagne une égalité.
            if (!best || score > best->first) {
                best = std::make_pair(score, token);
            }
        }

        // `WAIT` est invalide dans cette version : concéder proprement.
        std::cout << (best ? best->second : "ACCEPT-DEFEAT") << '\n' << std::flush;
    }
    return 0;
}
