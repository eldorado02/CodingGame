// Start-up! — C++ (submitted), v2 fast-market economy. Port of solutions/python/bot.py.
// The unfilled market goes to whoever sells first: hire the maximum every
// turn, sellers from 10 features on, half the developers on maintenance
// before that, 1 in DEVELOP_EVERY developing after; SAFETY_TURNS payrolls kept.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

constexpr int SELL_FEATURES = 10;        // sales need at least 10 features
constexpr int DEV_TARGET = 12;
constexpr int DEVELOP_EVERY = 6;         // once selling, 1 developer in 6 adds features
constexpr int EMPLOYEES_PER_MANAGER = 6; // past 4 some misbehave; speed is worth it
constexpr int HIRES_PER_MANAGER = 2;
constexpr int MAX_MANAGERS = 10;
constexpr int EMPLOYEE_PAY = 10, MANAGER_PAY = 20;
constexpr int SAFETY_TURNS = 3;          // payrolls kept in cash (income counted)
constexpr int FULL_SHARE = 1000;         // market share is in thousandths
constexpr int UNFILLED_PER_PLAYER = 30;  // 3 % of the market per player each turn
constexpr int NONE = -1;

struct State { long long id, playerCount, turn, income, cash, devs, sellers, managers, features, tests, bugs; };
struct Player { long long id, share, reputation; };
struct Staffing { long long devHire, sellHire, managerHire; };

long long payroll(long long employees, long long managers) { return EMPLOYEE_PAY * employees + MANAGER_PAY * managers; }

bool affordable(const State &s, long long employees, long long managers) {
    return s.cash + s.income * SAFETY_TURNS >= payroll(employees, managers) * SAFETY_TURNS;
}

Staffing staffing(const State &s, bool ready) {
    long long employees = s.devs + s.sellers, managers = s.managers;
    long long cap = max(0LL, min(HIRES_PER_MANAGER * managers, EMPLOYEES_PER_MANAGER * managers - employees));
    long long devHire = min(cap, max(0LL, DEV_TARGET - s.devs));
    long long sellHire = ready ? cap - devHire : 0;
    while (devHire + sellHire > 0 && !affordable(s, employees + devHire + sellHire, managers)) {
        if (sellHire > 0) --sellHire;
        else --devHire;
    }
    long long staff = employees + devHire + sellHire;
    long long managerHire = managers < MAX_MANAGERS && affordable(s, staff, managers + 1);
    return {devHire, sellHire, managerHire};
}

// Developers assigned to fixing bugs and writing tests.
long long maintenance(const State &s, long long devs) {
    if (s.features < SELL_FEATURES) return devs / 2;
    long long developing = max(0LL, min(devs / DEVELOP_EVERY, devs - s.bugs));
    return devs - developing;
}

// (competitive sellers, target id or NONE).
pair<long long, long long> fight(const Player &me, const vector<Player> &players, long long sellers) {
    long long shares = 0, rivals = 0;
    vector<Player> eligible;
    for (const Player &p : players) {
        shares += p.share;
        if (p.id == me.id) continue;
        ++rivals;
        if (p.share > 0 && p.reputation <= me.reputation) eligible.push_back(p);
    }
    if (eligible.empty() || FULL_SHARE - shares >= UNFILLED_PER_PLAYER * (long long)players.size()) return {0, NONE};
    long long target = NONE;
    if ((long long)eligible.size() < rivals) {
        Player best = eligible[0];
        for (const Player &p : eligible)
            if (p.share > best.share) best = p;
        target = best.id;
    }
    return {sellers / 2, target};
}

void decide(const State &s, const vector<Player> &players) {
    Player me{s.id, 0, 0};
    for (const Player &p : players)
        if (p.id == s.id) { me = p; break; }
    Staffing hire = staffing(s, s.features >= SELL_FEATURES);
    long long devs = s.devs + hire.devHire, sellers = s.sellers + hire.sellHire;
    auto [competitive, target] = fight(me, players, sellers);
    cout << hire.devHire << ' ' << hire.sellHire << ' ' << hire.managerHire << ' ' << maintenance(s, devs) << ' '
         << competitive;
    if (target != NONE) cout << ' ' << target;
    cout << endl;
}

int main() {
    ios::sync_with_stdio(false);
    State s;
    while (cin >> s.id >> s.playerCount >> s.turn >> s.income >> s.cash >> s.devs >> s.sellers >> s.managers >> s.features >>
           s.tests >> s.bugs) {
        vector<Player> players(max(0LL, s.playerCount));
        for (Player &p : players) cin >> p.id >> p.share >> p.reputation;
        if (!cin) return 0;
        decide(s, players);
    }
}
