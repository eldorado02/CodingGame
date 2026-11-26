/* Start-up! — C port of cpp/bot.cpp (same economy, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>

enum { SELL_FEATURES = 10 };         /* sales need at least 10 features */
enum { DEV_TARGET = 12 };
enum { DEVELOP_EVERY = 6 };          /* once selling, 1 developer in 6 adds features */
enum { EMPLOYEES_PER_MANAGER = 6 };  /* past 4 some misbehave; speed is worth it */
enum { HIRES_PER_MANAGER = 2, MAX_MANAGERS = 10 };
enum { EMPLOYEE_PAY = 10, MANAGER_PAY = 20 };
enum { SAFETY_TURNS = 3 };           /* payrolls kept in cash (income counted) */
enum { FULL_SHARE = 1000 };          /* market share is in thousandths */
enum { UNFILLED_PER_PLAYER = 30 };   /* 3 % of the market per player each turn */
enum { NONE = -1, MAX_PLAYERS = 16 };

typedef long long i64;
typedef struct { i64 id, player_count, turn, income, cash, devs, sellers, managers, features, tests, bugs; } State;
typedef struct { i64 id, share, reputation; } Player;

static i64 min_ll(i64 a, i64 b) { return a < b ? a : b; }
static i64 max_ll(i64 a, i64 b) { return a > b ? a : b; }

static i64 payroll(i64 employees, i64 managers) { return EMPLOYEE_PAY * employees + MANAGER_PAY * managers; }

static bool affordable(const State *s, i64 employees, i64 managers) {
    return s->cash + s->income * SAFETY_TURNS >= payroll(employees, managers) * SAFETY_TURNS;
}

static void staffing(const State *s, bool ready, i64 *dev_hire, i64 *sell_hire, i64 *manager_hire) {
    i64 employees = s->devs + s->sellers, managers = s->managers;
    i64 cap = max_ll(0, min_ll(HIRES_PER_MANAGER * managers, EMPLOYEES_PER_MANAGER * managers - employees));
    *dev_hire = min_ll(cap, max_ll(0, DEV_TARGET - s->devs));
    *sell_hire = ready ? cap - *dev_hire : 0;
    while (*dev_hire + *sell_hire > 0 && !affordable(s, employees + *dev_hire + *sell_hire, managers)) {
        if (*sell_hire > 0) --*sell_hire;
        else --*dev_hire;
    }
    i64 staff = employees + *dev_hire + *sell_hire;
    *manager_hire = managers < MAX_MANAGERS && affordable(s, staff, managers + 1);
}

/* Developers assigned to fixing bugs and writing tests. */
static i64 maintenance(const State *s, i64 devs) {
    if (s->features < SELL_FEATURES) return devs / 2;
    i64 developing = max_ll(0, min_ll(devs / DEVELOP_EVERY, devs - s->bugs));
    return devs - developing;
}

/* Competitive sellers; *target receives the focused rival id or NONE. */
static i64 fight(const Player *me, const Player *players, int count, i64 sellers, i64 *target) {
    i64 shares = 0;
    int rivals = 0, eligible = 0, best = NONE;
    for (int i = 0; i < count; ++i) {
        shares += players[i].share;
        if (players[i].id == me->id) continue;
        ++rivals;
        if (players[i].share > 0 && players[i].reputation <= me->reputation) {
            ++eligible;
            if (best == NONE || players[i].share > players[best].share) best = i;
        }
    }
    *target = NONE;
    if (!eligible || FULL_SHARE - shares >= (i64)UNFILLED_PER_PLAYER * count) return 0;
    if (eligible < rivals) *target = players[best].id;
    return sellers / 2;
}

static void decide(const State *s, const Player *players, int count) {
    Player me = {s->id, 0, 0};
    for (int i = 0; i < count; ++i)
        if (players[i].id == s->id) { me = players[i]; break; }
    i64 dev_hire, sell_hire, manager_hire, target;
    staffing(s, s->features >= SELL_FEATURES, &dev_hire, &sell_hire, &manager_hire);
    i64 devs = s->devs + dev_hire, sellers = s->sellers + sell_hire;
    i64 competitive = fight(&me, players, count, sellers, &target);
    printf("%lld %lld %lld %lld %lld", dev_hire, sell_hire, manager_hire, maintenance(s, devs), competitive);
    if (target != NONE) printf(" %lld", target);
    printf("\n");
    fflush(stdout);
}

int main(void) {
    State s;
    Player players[MAX_PLAYERS];
    while (scanf("%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", &s.id, &s.player_count, &s.turn, &s.income,
                 &s.cash, &s.devs, &s.sellers, &s.managers, &s.features, &s.tests, &s.bugs) == 11) {
        int count = (int)min_ll(max_ll(0, s.player_count), MAX_PLAYERS);
        for (int i = 0; i < count; ++i)
            if (scanf("%lld %lld %lld", &players[i].id, &players[i].share, &players[i].reputation) != 3) return 0;
        decide(&s, players, count);
    }
    return 0;
}
