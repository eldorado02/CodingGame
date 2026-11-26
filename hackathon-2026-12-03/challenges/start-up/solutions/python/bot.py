#!/usr/bin/env python3
"""Start-up! — Python reference of the differential fuzz, v2 fast-market economy.

The unfilled market goes to whoever sells first: the top bots reach 80 %
share by turn ~12 (v1, which waited for 4 tests per feature, never sold).
So, like them:
- hire the maximum every turn (2 per manager) and one manager per turn,
  developers first (DEV_TARGET), sellers as soon as we have 10 features;
- before 10 features half of the developers maintain (fixes are free while
  our share is 0), afterwards one in DEVELOP_EVERY keeps developing and the
  others fix bugs (reputation = 100 * features / (3 * bugs + fixed bugs));
- keep SAFETY_TURNS payrolls of cash (income counted).
Sellers fight competitors (focused on the biggest one we out-reputate) once
the unfilled market is too small.
"""
import sys

SELL_FEATURES = 10          # sales need at least 10 features
DEV_TARGET = 12
DEVELOP_EVERY = 6           # once selling, 1 developer in 6 adds features
EMPLOYEES_PER_MANAGER = 6   # past 4 some misbehave; speed is worth it
HIRES_PER_MANAGER = 2
MAX_MANAGERS = 10
EMPLOYEE_PAY = 10
MANAGER_PAY = 20
SAFETY_TURNS = 3            # payrolls kept in cash (income counted)
FULL_SHARE = 1000           # market share is in thousandths
UNFILLED_PER_PLAYER = 30    # 3 % of the market per player each turn


def payroll(employees, managers):
    return EMPLOYEE_PAY * employees + MANAGER_PAY * managers


def affordable(cash, income, employees, managers):
    return cash + income * SAFETY_TURNS >= payroll(employees, managers) * SAFETY_TURNS


def staffing(s, ready):
    """(devs to hire, sellers to hire, managers to hire)."""
    employees = s["devs"] + s["sellers"]
    managers = s["managers"]
    cap = max(0, min(HIRES_PER_MANAGER * managers, EMPLOYEES_PER_MANAGER * managers - employees))
    dev_hire = min(cap, max(0, DEV_TARGET - s["devs"]))
    sell_hire = cap - dev_hire if ready else 0
    while dev_hire + sell_hire > 0 and not affordable(s["cash"], s["income"], employees + dev_hire + sell_hire, managers):
        if sell_hire > 0:
            sell_hire -= 1
        else:
            dev_hire -= 1
    staff = employees + dev_hire + sell_hire
    manager_hire = 1 if managers < MAX_MANAGERS and affordable(s["cash"], s["income"], staff, managers + 1) else 0
    return dev_hire, sell_hire, manager_hire


def maintenance(s, devs):
    """Developers assigned to fixing bugs and writing tests."""
    if s["features"] < SELL_FEATURES:
        return devs // 2
    developing = max(0, min(devs // DEVELOP_EVERY, devs - s["bugs"]))
    return devs - developing


def fight(s, me, players, sellers):
    """(competitive sellers, target id or -1)."""
    shares = sum(p[1] for p in players)
    rivals = [p for p in players if p[0] != me[0]]
    eligible = [p for p in rivals if p[1] > 0 and p[2] <= me[2]]
    if not eligible or FULL_SHARE - shares >= UNFILLED_PER_PLAYER * len(players):
        return 0, -1
    target = -1
    if len(eligible) < len(rivals):
        best = eligible[0]
        for p in eligible[1:]:
            if p[1] > best[1]:
                best = p
        target = best[0]
    return sellers // 2, target


def decide(s, players):
    me = next((p for p in players if p[0] == s["id"]), (s["id"], 0, 0))
    dev_hire, sell_hire, manager_hire = staffing(s, s["features"] >= SELL_FEATURES)
    devs = s["devs"] + dev_hire
    sellers = s["sellers"] + sell_hire
    competitive, target = fight(s, me, players, sellers)
    values = [dev_hire, sell_hire, manager_hire, maintenance(s, devs), competitive]
    if target >= 0:
        values.append(target)
    return " ".join(map(str, values))


FIELDS = ("id", "playerCount", "turn", "income", "cash", "devs", "sellers", "managers", "features", "tests", "bugs")


def read_tokens():
    """Integers as they arrive (reading all stdin at once would block the game)."""
    for line in iter(sys.stdin.readline, ""):
        for t in line.split():
            yield int(t)


def main():
    tokens = read_tokens()
    while True:
        try:
            s = {name: next(tokens) for name in FIELDS}
            players = [tuple(next(tokens) for _ in range(3)) for _ in range(s["playerCount"])]
        except StopIteration:
            return
        print(decide(s, players), flush=True)


if __name__ == "__main__":
    main()
