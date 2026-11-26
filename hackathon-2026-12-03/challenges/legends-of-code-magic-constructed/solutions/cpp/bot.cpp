// LOCM Constructed — C++ (submitted), v1 greedy. Port of solutions/python/bot.py.
// Constructed: 15 distinct cards twice, best value per mana first (few items,
// few expensive cards). Battle: best-value creature subset, items (removal on
// what they kill, buffs on our strongest), kill lane guards, then face.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int CREATURE = 0, GREEN = 1, RED = 2, BLUE = 3;
constexpr int HAND = 0, MY_BOARD = 1, ENEMY_BOARD = -1;
constexpr int LANES = 2, LANE_SIZE = 3;
constexpr int DECK_SIZE = 30, COPIES = 2;
constexpr int ITEM_KINDS = 3, EXPENSIVE_COST = 7, EXPENSIVE_KINDS = 2;
constexpr int AREA_TARGET = 0;
constexpr int EFFICIENCY_SCALE = 100;  // integer value / mana ratios
constexpr const char *ABILITY_LETTERS = "BCDGLW";
constexpr int ABILITY_VALUE[] = {1, 1, 1, 3, 4, 3};
constexpr int CHARGE = 1 << 1, GUARD = 1 << 3, LETHAL = 1 << 4, WARD = 1 << 5;

struct Card {
    int number, id, location, type, cost, attack, defense, abilities, myHp, oppHp, draw, area, lane;
    bool canAttack, dead;
};

int parseAbilities(const string &text) {
    int mask = 0;
    for (char ch : text)
        for (int i = 0; ABILITY_LETTERS[i]; ++i)
            if (ch == ABILITY_LETTERS[i]) mask |= 1 << i;
    return mask;
}

int floorDiv(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }

// Rough strength of a card (bodies and effects), whatever its cost.
int value(const Card &c) {
    int abilities = 0;
    for (int i = 0; ABILITY_LETTERS[i]; ++i)
        if (c.abilities >> i & 1) abilities += ABILITY_VALUE[i];
    int effects = 3 * c.draw + c.myHp - c.oppHp;
    int body = c.type == CREATURE ? 2 * c.attack + 2 * c.defense + abilities
                                  : 2 * (abs(c.attack) + abs(c.defense)) + 2 * abilities;
    int total = body + effects;
    return c.area == AREA_TARGET ? total : 2 * total;
}

// Value per mana, as a floored integer (identical in every language).
int efficiency(const Card &c) { return floorDiv(value(c) * EFFICIENCY_SCALE, c.cost + 1); }

string construct(vector<Card> cards) {
    stable_sort(cards.begin(), cards.end(), [](const Card &a, const Card &b) {
        int ea = efficiency(a), eb = efficiency(b);
        return ea != eb ? ea > eb : a.number < b.number;
    });
    vector<int> chosen;
    int items = 0, expensive = 0;
    for (const Card &c : cards) {
        if ((int)chosen.size() * COPIES >= DECK_SIZE) break;
        if (find(chosen.begin(), chosen.end(), c.number) != chosen.end()) continue;
        if (c.type != CREATURE && items >= ITEM_KINDS) continue;
        if (c.cost >= EXPENSIVE_COST && expensive >= EXPENSIVE_KINDS) continue;
        chosen.push_back(c.number);
        items += c.type != CREATURE;
        expensive += c.cost >= EXPENSIVE_COST;
    }
    string out;
    for (int n : chosen)
        for (int k = 0; k < COPIES; ++k) out += (out.empty() ? "" : ";") + string("CHOOSE ") + to_string(n);
    return out.empty() ? "PASS" : out;
}

struct Battle {
    vector<Card> cards;
    int mana;
    vector<int> hand, mine, enemy;  // indexes into cards, by instance id (summons appended)
    vector<string> actions;

    Battle(vector<Card> all, int mana_) : cards(move(all)), mana(mana_) {
        vector<int> order(cards.size());
        for (int i = 0; i < (int)order.size(); ++i) order[i] = i;
        stable_sort(order.begin(), order.end(), [&](int a, int b) { return cards[a].id < cards[b].id; });
        for (int i : order) {
            Card &c = cards[i];
            c.canAttack = c.location == MY_BOARD;
            c.dead = false;
            (c.location == HAND ? hand : c.location == MY_BOARD ? mine : enemy).push_back(i);
        }
    }

    int laneCount(int lane) const {
        int n = 0;
        for (int i : mine) n += !cards[i].dead && cards[i].lane == lane;
        return n;
    }

    int aliveMine() const {
        int n = 0;
        for (int i : mine) n += !cards[i].dead;
        return n;
    }

    void summon() {
        vector<int> creatures;
        for (int i : hand)
            if (cards[i].type == CREATURE && cards[i].cost <= mana) creatures.push_back(i);
        int freeSlots = LANES * LANE_SIZE - aliveMine();
        int bestMask = 0, bestValue = 0;
        for (int mask = 1; mask < 1 << creatures.size(); ++mask) {
            int count = 0, cost = 0, total = 0;
            for (int k = 0; k < (int)creatures.size(); ++k)
                if (mask >> k & 1) { ++count; cost += cards[creatures[k]].cost; total += value(cards[creatures[k]]); }
            if (count > freeSlots || cost > mana) continue;
            if (total > bestValue) { bestMask = mask; bestValue = total; }
        }
        for (int k = 0; k < (int)creatures.size(); ++k) {
            if (!(bestMask >> k & 1)) continue;
            Card &c = cards[creatures[k]];
            int lane = laneCount(0) <= laneCount(1) ? 0 : 1;
            if (laneCount(lane) >= LANE_SIZE) continue;
            c.lane = lane;
            c.location = MY_BOARD;
            c.canAttack = c.abilities & CHARGE;
            mine.push_back(creatures[k]);
            mana -= c.cost;
            actions.push_back("SUMMON " + to_string(c.id) + " " + to_string(lane));
        }
    }

    // Deals damage (Ward absorbs it); the creature dies at 0 defense.
    void damage(Card &target, int amount) {
        if (amount <= 0) return;
        if (target.abilities & WARD) { target.abilities &= ~WARD; return; }
        target.defense -= amount;
        if (target.defense <= 0) target.dead = true;
    }

    int killableOrStrongest(const Card &item) const {
        int best = -1;
        bool anyKill = false;
        for (int i : enemy)
            if (!cards[i].dead && !(cards[i].abilities & WARD) && cards[i].defense <= -item.defense) anyKill = true;
        for (int i : enemy) {
            const Card &c = cards[i];
            if (c.dead) continue;
            if (anyKill && ((c.abilities & WARD) || c.defense > -item.defense)) continue;
            if (best < 0 || c.attack + c.defense > cards[best].attack + cards[best].defense) best = i;
        }
        return best;
    }

    int strongestMine() const {
        int best = -1;
        for (int i : mine)
            if (!cards[i].dead && (best < 0 || cards[i].attack > cards[best].attack)) best = i;
        return best;
    }

    bool anyEnemy() const {
        for (int i : enemy)
            if (!cards[i].dead) return true;
        return false;
    }

    void useItems() {
        vector<int> items;
        for (int i : hand)
            if (cards[i].type != CREATURE) items.push_back(i);
        stable_sort(items.begin(), items.end(), [&](int a, int b) {
            int va = value(cards[a]), vb = value(cards[b]);
            return va != vb ? va > vb : cards[a].id < cards[b].id;
        });
        for (int i : items) {
            const Card &item = cards[i];
            if (item.cost > mana) continue;
            int target = -1;
            if (item.type == GREEN) {
                target = strongestMine();
                if (target < 0) continue;
                cards[target].attack += item.attack;
                cards[target].defense += item.defense;
                cards[target].abilities |= item.abilities;
            } else if (item.type == RED || (item.type == BLUE && item.defense < 0 && anyEnemy())) {
                target = killableOrStrongest(item);
                if (target < 0) continue;
                cards[target].attack = max(0, cards[target].attack + item.attack);
                cards[target].abilities &= ~item.abilities;
                damage(cards[target], -item.defense);
            }
            mana -= item.cost;
            actions.push_back("USE " + to_string(item.id) + " " + to_string(target < 0 ? -1 : cards[target].id));
        }
    }

    void attackLane(int lane) {
        vector<int> attackers;
        for (int i : mine)
            if (!cards[i].dead && cards[i].lane == lane && cards[i].canAttack && cards[i].attack > 0) attackers.push_back(i);
        while (!attackers.empty()) {
            int guard = -1;
            for (int i : enemy) {
                const Card &c = cards[i];
                if (c.dead || c.lane != lane || !(c.abilities & GUARD)) continue;
                if (guard < 0 || c.defense < cards[guard].defense || (c.defense == cards[guard].defense && c.id < cards[guard].id))
                    guard = i;
            }
            if (guard < 0) break;
            Card &g = cards[guard];
            int pick = -1;
            for (int i : attackers) {  // smallest attacker that kills it
                const Card &a = cards[i];
                bool kills = a.attack >= g.defense || (a.abilities & LETHAL);
                if (kills && (pick < 0 || a.attack < cards[pick].attack || (a.attack == cards[pick].attack && a.id < cards[pick].id)))
                    pick = i;
            }
            if (pick < 0)  // none kills it: the strongest one
                for (int i : attackers) {
                    const Card &a = cards[i];
                    if (pick < 0 || a.attack > cards[pick].attack || (a.attack == cards[pick].attack && a.id < cards[pick].id))
                        pick = i;
                }
            attackers.erase(find(attackers.begin(), attackers.end(), pick));
            const Card &a = cards[pick];
            actions.push_back("ATTACK " + to_string(a.id) + " " + to_string(g.id));
            bool lethal = (a.abilities & LETHAL) && !(g.abilities & WARD);
            damage(g, lethal ? g.defense : a.attack);
        }
        for (int i : attackers) actions.push_back("ATTACK " + to_string(cards[i].id) + " -1");
    }

    string play() {
        summon();
        useItems();
        for (int lane = 0; lane < LANES; ++lane) attackLane(lane);
        string out;
        for (const string &a : actions) out += (out.empty() ? "" : ";") + a;
        return out.empty() ? "PASS" : out;
    }
};

int main() {
    ios::sync_with_stdio(false);
    string line;
    while (true) {
        int stats[2][4];
        for (auto &p : stats)
            for (int &v : p)
                if (!(cin >> v)) return 0;
        int opponentHand, actionCount;
        cin >> opponentHand >> actionCount;
        getline(cin, line);
        for (int i = 0; i < actionCount; ++i) getline(cin, line);
        int count;
        cin >> count;
        vector<Card> cards(count);
        for (Card &c : cards) {
            string abilities;
            cin >> c.number >> c.id >> c.location >> c.type >> c.cost >> c.attack >> c.defense >> abilities >> c.myHp >>
                c.oppHp >> c.draw >> c.area >> c.lane;
            c.abilities = parseAbilities(abilities);
        }
        if (!cin) return 0;
        int mana = stats[0][1];
        cout << (mana == 0 ? construct(cards) : Battle(cards, mana).play()) << endl;
    }
}
