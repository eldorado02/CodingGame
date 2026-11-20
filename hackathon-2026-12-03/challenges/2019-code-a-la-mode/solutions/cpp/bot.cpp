// Code a la Mode — plat BLUEBERRIES + ICE_CREAM puis livraison.
#include <array>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct Pt { int x, y; };
std::optional<Pt> dish, window, berries, icecream;
std::vector<Pt> tables;

bool is_dish(const std::string& s) { return s.rfind("DISH", 0) == 0; }
bool contains(const std::string& s, const char* food) {
    return s.find(food) != std::string::npos;
}
void contents(const std::string& s, bool& bb, bool& ic) {
    bb = ic = false;
    if (!is_dish(s)) return;
    bb = contains(s, "BLUEBERRIES");
    ic = contains(s, "ICE_CREAM");
}
void emit(const std::optional<Pt>& p) {
    if (!p) std::cout << "WAIT\n";
    else std::cout << "USE " << p->x << ' ' << p->y << '\n';
}
}  // namespace

int main() {
    std::string line;
    std::getline(std::cin, line);
    const int nall = std::stoi(line);
    for (int i = 0; i < nall; ++i) std::getline(std::cin, line);
    for (int y = 0; y < 7; ++y) {
        std::getline(std::cin, line);
        for (int x = 0; x < 11; ++x) {
            Pt p{x, y};
            switch (line[x]) {
                case 'D': if (!dish) dish = p; break;
                case 'W': if (!window) window = p; break;
                case 'B': if (!berries) berries = p; break;
                case 'I': if (!icecream) icecream = p; break;
                case '#': tables.push_back(p); break;
                default: break;
            }
        }
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;  // tours restants
        std::getline(std::cin, line);  // notre joueur
        std::string hx, hy, held;
        std::stringstream(line) >> hx >> hy >> held;
        std::getline(std::cin, line);  // partenaire
        int nt;
        std::getline(std::cin, line);
        std::stringstream(line) >> nt;
        std::vector<Pt> pos(nt);
        std::vector<std::string> names(nt);
        for (int i = 0; i < nt; ++i) {
            std::getline(std::cin, line);
            std::stringstream(line) >> pos[i].x >> pos[i].y >> names[i];
        }
        std::getline(std::cin, line);  // four
        int nc;
        std::getline(std::cin, line);
        std::stringstream(line) >> nc;
        for (int i = 0; i < nc; ++i) std::getline(std::cin, line);

        bool bb, ic;
        contents(held, bb, ic);
        std::optional<Pt> target;
        if (is_dish(held) && bb && ic) {
            target = window;
        } else if (is_dish(held)) {
            const bool mbb = !bb, mic = !ic;
            for (int i = 0; i < nt && !target; ++i)
                if (mbb && names[i] == "BLUEBERRIES") target = pos[i];
            for (int i = 0; i < nt && !target; ++i)
                if (mic && names[i] == "ICE_CREAM") target = pos[i];
            if (!target) target = mbb ? berries : icecream;
        } else if (held == "BLUEBERRIES" || held == "ICE_CREAM") {
            target = dish;
        } else {
            for (int i = 0; i < nt && !target; ++i) {
                bool a, b;
                contents(names[i], a, b);
                if (is_dish(names[i]) && a && b) target = pos[i];
            }
            for (int i = 0; i < nt && !target; ++i)
                if (is_dish(names[i])) target = pos[i];
            for (int i = 0; i < nt && !target; ++i)
                if (names[i] == "BLUEBERRIES" || names[i] == "ICE_CREAM") target = pos[i];
            if (!target) target = dish;
        }
        emit(target);
        std::cout << std::flush;
    }
    return 0;
}
