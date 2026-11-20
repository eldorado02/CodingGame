// Botters of the Galaxy Wood 6 — HULK, ATTACK héros puis tour.
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
struct Entity {
    int id, x, y, health;
    std::string kind;
};

long long d2(int ax, int ay, int bx, int by) {
    const long long dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

const Entity* best_of(const std::vector<Entity>& v, const std::string& kind,
                      int mx, int my) {
    const Entity* best = nullptr;
    for (const auto& e : v) {
        if (e.kind != kind) continue;
        if (!best || d2(mx, my, e.x, e.y) < d2(mx, my, best->x, best->y)) best = &e;
    }
    return best;
}
}  // namespace

int main() {
    std::string line;
    auto next_line = [&]() -> bool { return static_cast<bool>(std::getline(std::cin, line)); };

    if (!next_line()) return 0;
    const int team = std::stoi(line);
    if (!next_line()) return 0;
    const int bushes = std::stoi(line);
    for (int i = 0; i < bushes; ++i) if (!next_line()) return 0;
    if (!next_line()) return 0;
    const int items = std::stoi(line);
    for (int i = 0; i < items; ++i) if (!next_line()) return 0;

    int mx = 0, my = 0;
    while (next_line()) {
        // Une ligne par champ : gold, gold adverse, roundType, entityCount.
        const int gold = std::stoi(line);
        (void)gold;
        if (!next_line()) return 0;
        const int opp_gold = std::stoi(line);
        (void)opp_gold;
        if (!next_line()) return 0;
        const int round_type = std::stoi(line);
        if (!next_line()) return 0;
        const int count = std::stoi(line);

        std::vector<Entity> foes;
        for (int i = 0; i < count; ++i) {
            if (!next_line()) return 0;
            std::istringstream iss(line);
            Entity en{};
            int eteam = 0, range = 0, maxhp = 0, shield = 0, dmg = 0;
            int speed = 0, stun = 0, goldval = 0;
            if (!(iss >> en.id >> eteam >> en.kind >> en.x >> en.y >> range
                      >> en.health >> maxhp >> shield >> dmg >> speed >> stun >> goldval)) return 0;
            // Les champs supplémentaires (héros) sont ignorés : la ligne entière
            // a été lue, il n'y a donc pas de désalignement possible.
            if (en.kind == "HERO" && eteam == team) { mx = en.x; my = en.y; }
            if (eteam == team || en.health <= 0) continue;
            foes.push_back(en);
        }

        // La phase de pick contient aussi des entités : lues ci-dessus.
        if (round_type < 0) {
            std::cout << "HULK\n" << std::flush;
            continue;
        }

        const Entity* target = best_of(foes, "HERO", mx, my);
        if (!target) target = best_of(foes, "TOWER", mx, my);
        if (!target) {
            for (const auto& e : foes) {
                if (e.kind == "HERO" || e.kind == "TOWER") continue;
                if (!target || d2(mx, my, e.x, e.y) < d2(mx, my, target->x, target->y)) target = &e;
            }
        }
        for (int h = 0; h < round_type; ++h) {
            if (target) std::cout << "ATTACK " << target->id << '\n';
            else std::cout << "WAIT\n";
        }
        std::cout << std::flush;
    }
    return 0;
}
