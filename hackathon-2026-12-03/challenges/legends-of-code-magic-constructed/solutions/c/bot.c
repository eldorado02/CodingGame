/* LOCM Constructed — C port of cpp/bot.cpp (same deck, same battle rules).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CREATURE = 0, GREEN = 1, RED = 2, BLUE = 3 };
enum { HAND = 0, MY_BOARD = 1, ENEMY_BOARD = -1 };
enum { LANES = 2, LANE_SIZE = 3, DECK_SIZE = 30, COPIES = 2 };
enum { ITEM_KINDS = 3, EXPENSIVE_COST = 7, EXPENSIVE_KINDS = 2, AREA_TARGET = 0 };
enum { EFFICIENCY_SCALE = 100 };  /* integer value / mana ratios */
enum { CHARGE = 1 << 1, GUARD = 1 << 3, LETHAL = 1 << 4, WARD = 1 << 5 };
enum { MAX_CARDS = 160, LINE_SIZE = 256, OUT_SIZE = 8192, NONE = -1 };
static const char *ABILITY_LETTERS = "BCDGLW";
static const int ABILITY_VALUE[] = {1, 1, 1, 3, 4, 3};

typedef struct {
    int number, id, location, type, cost, attack, defense, abilities, my_hp, opp_hp, draw, area, lane;
    bool can_attack, dead;
} Card;

static Card cards[MAX_CARDS];
static int card_count, mana;
static int hand[MAX_CARDS], mine[MAX_CARDS], enemy[MAX_CARDS];  /* by instance id, summons appended */
static int n_hand, n_mine, n_enemy;
static char out[OUT_SIZE];

static void append(const char *action) {
    if (out[0]) strncat(out, ";", OUT_SIZE - strlen(out) - 1);
    strncat(out, action, OUT_SIZE - strlen(out) - 1);
}

static int parse_abilities(const char *text) {
    int mask = 0;
    for (const char *p = text; *p; ++p)
        for (int i = 0; ABILITY_LETTERS[i]; ++i)
            if (*p == ABILITY_LETTERS[i]) mask |= 1 << i;
    return mask;
}

static int floor_div(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }

/* Rough strength of a card (bodies and effects), whatever its cost. */
static int value(const Card *c) {
    int abilities = 0;
    for (int i = 0; ABILITY_LETTERS[i]; ++i)
        if (c->abilities >> i & 1) abilities += ABILITY_VALUE[i];
    int effects = 3 * c->draw + c->my_hp - c->opp_hp;
    int body = c->type == CREATURE ? 2 * c->attack + 2 * c->defense + abilities
                                   : 2 * (abs(c->attack) + abs(c->defense)) + 2 * abilities;
    int total = body + effects;
    return c->area == AREA_TARGET ? total : 2 * total;
}

/* Value per mana, as a floored integer (identical in every language). */
static int efficiency(const Card *c) { return floor_div(value(c) * EFFICIENCY_SCALE, c->cost + 1); }

/* Stable insertion sort of card indexes with a "comes first" predicate. */
static void sort_indexes(int *idx, int n, bool (*first)(int, int)) {
    for (int i = 1; i < n; ++i) {
        int x = idx[i], j = i - 1;
        while (j >= 0 && first(x, idx[j])) { idx[j + 1] = idx[j]; --j; }
        idx[j + 1] = x;
    }
}

static bool by_efficiency(int a, int b) {
    int ea = efficiency(&cards[a]), eb = efficiency(&cards[b]);
    return ea != eb ? ea > eb : cards[a].number < cards[b].number;
}
static bool by_id(int a, int b) { return cards[a].id < cards[b].id; }
static bool by_value(int a, int b) {
    int va = value(&cards[a]), vb = value(&cards[b]);
    return va != vb ? va > vb : cards[a].id < cards[b].id;
}

static void construct(void) {
    int order[MAX_CARDS], chosen[DECK_SIZE], n_chosen = 0, items = 0, expensive = 0;
    for (int i = 0; i < card_count; ++i) order[i] = i;
    sort_indexes(order, card_count, by_efficiency);
    for (int k = 0; k < card_count && n_chosen * COPIES < DECK_SIZE; ++k) {
        const Card *c = &cards[order[k]];
        bool seen = false;
        for (int j = 0; j < n_chosen; ++j) seen |= chosen[j] == c->number;
        if (seen || (c->type != CREATURE && items >= ITEM_KINDS) || (c->cost >= EXPENSIVE_COST && expensive >= EXPENSIVE_KINDS))
            continue;
        chosen[n_chosen++] = c->number;
        items += c->type != CREATURE;
        expensive += c->cost >= EXPENSIVE_COST;
    }
    char action[LINE_SIZE];
    for (int j = 0; j < n_chosen; ++j)
        for (int k = 0; k < COPIES; ++k) {
            snprintf(action, sizeof action, "CHOOSE %d", chosen[j]);
            append(action);
        }
}

static int lane_count(int lane) {
    int n = 0;
    for (int i = 0; i < n_mine; ++i) n += !cards[mine[i]].dead && cards[mine[i]].lane == lane;
    return n;
}

static void summon(void) {
    int creatures[MAX_CARDS], n = 0, alive = 0;
    for (int i = 0; i < n_hand; ++i)
        if (cards[hand[i]].type == CREATURE && cards[hand[i]].cost <= mana) creatures[n++] = hand[i];
    for (int i = 0; i < n_mine; ++i) alive += !cards[mine[i]].dead;
    int free_slots = LANES * LANE_SIZE - alive, best_mask = 0, best_value = 0;
    for (int mask = 1; mask < 1 << n; ++mask) {
        int count = 0, cost = 0, total = 0;
        for (int k = 0; k < n; ++k)
            if (mask >> k & 1) { ++count; cost += cards[creatures[k]].cost; total += value(&cards[creatures[k]]); }
        if (count > free_slots || cost > mana) continue;
        if (total > best_value) { best_mask = mask; best_value = total; }
    }
    char action[LINE_SIZE];
    for (int k = 0; k < n; ++k) {
        if (!(best_mask >> k & 1)) continue;
        Card *c = &cards[creatures[k]];
        int lane = lane_count(0) <= lane_count(1) ? 0 : 1;
        if (lane_count(lane) >= LANE_SIZE) continue;
        c->lane = lane;
        c->location = MY_BOARD;
        c->can_attack = c->abilities & CHARGE;
        mine[n_mine++] = creatures[k];
        mana -= c->cost;
        snprintf(action, sizeof action, "SUMMON %d %d", c->id, lane);
        append(action);
    }
}

/* Deals damage (Ward absorbs it); the creature dies at 0 defense. */
static void damage(Card *target, int amount) {
    if (amount <= 0) return;
    if (target->abilities & WARD) { target->abilities &= ~WARD; return; }
    target->defense -= amount;
    if (target->defense <= 0) target->dead = true;
}

static int killable_or_strongest(const Card *item) {
    bool any_kill = false;
    int best = NONE;
    for (int i = 0; i < n_enemy; ++i) {
        const Card *c = &cards[enemy[i]];
        if (!c->dead && !(c->abilities & WARD) && c->defense <= -item->defense) any_kill = true;
    }
    for (int i = 0; i < n_enemy; ++i) {
        const Card *c = &cards[enemy[i]];
        if (c->dead) continue;
        if (any_kill && ((c->abilities & WARD) || c->defense > -item->defense)) continue;
        if (best == NONE || c->attack + c->defense > cards[best].attack + cards[best].defense) best = enemy[i];
    }
    return best;
}

static int strongest_mine(void) {
    int best = NONE;
    for (int i = 0; i < n_mine; ++i)
        if (!cards[mine[i]].dead && (best == NONE || cards[mine[i]].attack > cards[best].attack)) best = mine[i];
    return best;
}

static bool any_enemy(void) {
    for (int i = 0; i < n_enemy; ++i)
        if (!cards[enemy[i]].dead) return true;
    return false;
}

static void use_items(void) {
    int items[MAX_CARDS], n = 0;
    for (int i = 0; i < n_hand; ++i)
        if (cards[hand[i]].type != CREATURE) items[n++] = hand[i];
    sort_indexes(items, n, by_value);
    char action[LINE_SIZE];
    for (int k = 0; k < n; ++k) {
        const Card *item = &cards[items[k]];
        if (item->cost > mana) continue;
        int target = NONE;
        if (item->type == GREEN) {
            target = strongest_mine();
            if (target == NONE) continue;
            cards[target].attack += item->attack;
            cards[target].defense += item->defense;
            cards[target].abilities |= item->abilities;
        } else if (item->type == RED || (item->type == BLUE && item->defense < 0 && any_enemy())) {
            target = killable_or_strongest(item);
            if (target == NONE) continue;
            int attack = cards[target].attack + item->attack;
            cards[target].attack = attack > 0 ? attack : 0;
            cards[target].abilities &= ~item->abilities;
            damage(&cards[target], -item->defense);
        }
        mana -= item->cost;
        snprintf(action, sizeof action, "USE %d %d", item->id, target == NONE ? -1 : cards[target].id);
        append(action);
    }
}

static bool smaller_attacker(const Card *a, const Card *b) { return a->attack < b->attack || (a->attack == b->attack && a->id < b->id); }
static bool bigger_attacker(const Card *a, const Card *b) { return a->attack > b->attack || (a->attack == b->attack && a->id < b->id); }

static void attack_lane(int lane) {
    int attackers[MAX_CARDS], n = 0;
    char action[LINE_SIZE];
    for (int i = 0; i < n_mine; ++i) {
        const Card *c = &cards[mine[i]];
        if (!c->dead && c->lane == lane && c->can_attack && c->attack > 0) attackers[n++] = mine[i];
    }
    while (n > 0) {
        int guard = NONE;
        for (int i = 0; i < n_enemy; ++i) {
            const Card *c = &cards[enemy[i]];
            if (c->dead || c->lane != lane || !(c->abilities & GUARD)) continue;
            if (guard == NONE || c->defense < cards[guard].defense || (c->defense == cards[guard].defense && c->id < cards[guard].id))
                guard = enemy[i];
        }
        if (guard == NONE) break;
        Card *g = &cards[guard];
        int pick = -1;
        for (int k = 0; k < n; ++k) { /* smallest attacker that kills it */
            const Card *a = &cards[attackers[k]];
            bool kills = a->attack >= g->defense || (a->abilities & LETHAL);
            if (kills && (pick < 0 || smaller_attacker(a, &cards[attackers[pick]]))) pick = k;
        }
        if (pick < 0) /* none kills it: the strongest one */
            for (int k = 0; k < n; ++k)
                if (pick < 0 || bigger_attacker(&cards[attackers[k]], &cards[attackers[pick]])) pick = k;
        const Card *a = &cards[attackers[pick]];
        for (int k = pick; k + 1 < n; ++k) attackers[k] = attackers[k + 1];
        --n;
        snprintf(action, sizeof action, "ATTACK %d %d", a->id, g->id);
        append(action);
        bool lethal = (a->abilities & LETHAL) && !(g->abilities & WARD);
        damage(g, lethal ? g->defense : a->attack);
    }
    for (int k = 0; k < n; ++k) {
        snprintf(action, sizeof action, "ATTACK %d -1", cards[attackers[k]].id);
        append(action);
    }
}

static void battle(void) {
    int order[MAX_CARDS];
    n_hand = n_mine = n_enemy = 0;
    for (int i = 0; i < card_count; ++i) order[i] = i;
    sort_indexes(order, card_count, by_id);
    for (int k = 0; k < card_count; ++k) {
        Card *c = &cards[order[k]];
        c->can_attack = c->location == MY_BOARD;
        c->dead = false;
        if (c->location == HAND) hand[n_hand++] = order[k];
        else if (c->location == MY_BOARD) mine[n_mine++] = order[k];
        else enemy[n_enemy++] = order[k];
    }
    summon();
    use_items();
    for (int lane = 0; lane < LANES; ++lane) attack_lane(lane);
}

int main(void) {
    char line[LINE_SIZE];
    for (;;) {
        int stats[2][4], opponent_hand, action_count;
        for (int p = 0; p < 2; ++p)
            if (scanf("%d %d %d %d", &stats[p][0], &stats[p][1], &stats[p][2], &stats[p][3]) != 4) return 0;
        if (scanf("%d %d", &opponent_hand, &action_count) != 2) return 0;
        if (!fgets(line, sizeof line, stdin)) return 0; /* end of the counts line */
        for (int i = 0; i < action_count; ++i)
            if (!fgets(line, sizeof line, stdin)) return 0;
        if (scanf("%d", &card_count) != 1) return 0;
        if (card_count > MAX_CARDS) card_count = MAX_CARDS;
        for (int i = 0; i < card_count; ++i) {
            Card *c = &cards[i];
            char abilities[LINE_SIZE];
            if (scanf("%d %d %d %d %d %d %d %255s %d %d %d %d %d", &c->number, &c->id, &c->location, &c->type, &c->cost,
                      &c->attack, &c->defense, abilities, &c->my_hp, &c->opp_hp, &c->draw, &c->area, &c->lane) != 13)
                return 0;
            c->abilities = parse_abilities(abilities);
        }
        mana = stats[0][1];
        out[0] = '\0';
        if (mana == 0) construct();
        else battle();
        printf("%s\n", out[0] ? out : "PASS");
        fflush(stdout);
    }
}
