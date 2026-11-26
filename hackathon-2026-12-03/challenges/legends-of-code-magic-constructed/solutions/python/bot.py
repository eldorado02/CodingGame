#!/usr/bin/env python3
"""LOCM Constructed — Python reference of the differential fuzz, v1 greedy.

Constructed turn (playerMana == 0): 15 distinct cards twice each, best
value per mana first, at most ITEM_KINDS items and EXPENSIVE_KINDS cards
costing EXPENSIVE_COST or more. `CHOOSE` takes the card number (base id).
Battle turn:
1. summon the hand subset of creatures with the most value that fits the
   mana and the free slots (hand <= 8: all subsets), each on the emptier lane;
2. play items, best value first: red / damaging blue on the enemy creature
   they kill (else the strongest), green on our strongest creature, other
   blue items on the opponent;
3. per lane, kill the enemy guards first (smallest guard first, smallest
   attacker that kills it), then every attacker left hits the opponent.
"""
import sys

CREATURE, GREEN, RED, BLUE = 0, 1, 2, 3
HAND, MY_BOARD, ENEMY_BOARD = 0, 1, -1
LANES = 2
LANE_SIZE = 3
DECK_SIZE = 30
COPIES = 2
ITEM_KINDS = 3
EXPENSIVE_COST = 7
EXPENSIVE_KINDS = 2
ABILITY_VALUE = {"B": 1, "C": 1, "D": 1, "G": 3, "L": 4, "W": 3}
AREA_TARGET = 0
EFFICIENCY_SCALE = 100      # integer value / mana ratios


class Card:
    __slots__ = ("number", "id", "location", "type", "cost", "attack", "defense", "abilities",
                 "my_hp", "opp_hp", "draw", "area", "lane", "can_attack")

    def __init__(self, fields):
        (self.number, self.id, self.location, self.type, self.cost, self.attack, self.defense) = map(int, fields[:7])
        self.abilities = set(ch for ch in fields[7] if ch in ABILITY_VALUE)
        self.my_hp, self.opp_hp, self.draw, self.area, self.lane = map(int, fields[8:13])
        self.can_attack = self.location == MY_BOARD


def value(card):
    """Rough strength of a card (bodies and effects), whatever its cost."""
    abilities = sum(ABILITY_VALUE[a] for a in card.abilities)
    effects = 3 * card.draw + card.my_hp - card.opp_hp
    if card.type == CREATURE:
        body = 2 * card.attack + 2 * card.defense + abilities
    else:
        body = 2 * (abs(card.attack) + abs(card.defense)) + 2 * abilities
    total = body + effects
    return total if card.area == AREA_TARGET else 2 * total


def efficiency(card):
    """Value per mana, as a floored integer (identical in every language)."""
    return value(card) * EFFICIENCY_SCALE // (card.cost + 1)


def construct(cards):
    ranked = sorted(cards, key=lambda c: (-efficiency(c), c.number))
    chosen, items, expensive = [], 0, 0
    for card in ranked:
        if len(chosen) * COPIES >= DECK_SIZE:
            break
        if card.number in chosen:
            continue
        if card.type != CREATURE and items >= ITEM_KINDS:
            continue
        if card.cost >= EXPENSIVE_COST and expensive >= EXPENSIVE_KINDS:
            continue
        chosen.append(card.number)
        items += card.type != CREATURE
        expensive += card.cost >= EXPENSIVE_COST
    return ";".join("CHOOSE %d" % n for n in chosen for _ in range(COPIES)) or "PASS"


class Battle:
    def __init__(self, cards, mana):
        self.mana = mana
        self.hand = sorted((c for c in cards if c.location == HAND), key=lambda c: c.id)
        self.mine = sorted((c for c in cards if c.location == MY_BOARD), key=lambda c: c.id)
        self.enemy = sorted((c for c in cards if c.location == ENEMY_BOARD), key=lambda c: c.id)
        self.actions = []

    def lane_count(self, lane):
        return sum(1 for c in self.mine if c.lane == lane)

    def summon(self):
        creatures = [c for c in self.hand if c.type == CREATURE and c.cost <= self.mana]
        free = LANES * LANE_SIZE - len(self.mine)
        best_mask, best_value = 0, 0
        for mask in range(1, 1 << len(creatures)):
            picked = [c for i, c in enumerate(creatures) if mask >> i & 1]
            if len(picked) > free or sum(c.cost for c in picked) > self.mana:
                continue
            total = sum(value(c) for c in picked)
            if total > best_value:
                best_mask, best_value = mask, total
        for i, card in enumerate(creatures):
            if best_mask >> i & 1:
                lane = 0 if self.lane_count(0) <= self.lane_count(1) else 1
                if self.lane_count(lane) >= LANE_SIZE:
                    continue
                card.lane, card.location = lane, MY_BOARD
                card.can_attack = "C" in card.abilities
                self.mine.append(card)
                self.mana -= card.cost
                self.actions.append("SUMMON %d %d" % (card.id, lane))

    def damage(self, target, amount):
        """Deals damage (Ward absorbs it); removes the creature when it dies."""
        if amount <= 0:
            return
        if "W" in target.abilities:
            target.abilities.discard("W")
            return
        target.defense -= amount
        if target.defense <= 0:
            for side in (self.mine, self.enemy):
                if target in side:
                    side.remove(target)

    def killable_or_strongest(self, item):
        if not self.enemy:
            return None
        kills = [c for c in self.enemy if "W" not in c.abilities and c.defense <= -item.defense]
        pool = kills or self.enemy
        best = pool[0]
        for c in pool[1:]:
            if c.attack + c.defense > best.attack + best.defense:
                best = c
        return best

    def strongest_mine(self):
        best = None
        for c in self.mine:
            if best is None or c.attack > best.attack:
                best = c
        return best

    def use_items(self):
        items = sorted((c for c in self.hand if c.type != CREATURE), key=lambda c: (-value(c), c.id))
        for item in items:
            if item.cost > self.mana:
                continue
            if item.type == GREEN:
                target = self.strongest_mine()
                if target is None:
                    continue
                target.attack += item.attack
                target.defense += item.defense
                target.abilities |= item.abilities
            elif item.type == RED or (item.type == BLUE and item.defense < 0 and self.enemy):
                target = self.killable_or_strongest(item)
                if target is None:
                    continue
                target.attack = max(0, target.attack + item.attack)
                target.abilities -= item.abilities
                self.damage(target, -item.defense)
            else:
                target = None
            self.mana -= item.cost
            self.actions.append("USE %d %d" % (item.id, target.id if target else -1))

    def attack_lane(self, lane):
        attackers = [c for c in self.mine if c.lane == lane and c.can_attack and c.attack > 0]
        while attackers:
            guards = sorted((c for c in self.enemy if c.lane == lane and "G" in c.abilities),
                            key=lambda c: (c.defense, c.id))
            if not guards:
                break
            guard = guards[0]
            killers = [a for a in attackers if a.attack >= guard.defense or "L" in a.abilities]
            attacker = min(killers, key=lambda a: (a.attack, a.id)) if killers \
                else max(attackers, key=lambda a: (a.attack, -a.id))
            attackers.remove(attacker)
            self.actions.append("ATTACK %d %d" % (attacker.id, guard.id))
            if "L" in attacker.abilities and "W" not in guard.abilities:
                self.damage(guard, guard.defense)
            else:
                self.damage(guard, attacker.attack)
        for attacker in attackers:
            self.actions.append("ATTACK %d -1" % attacker.id)

    def play(self):
        self.summon()
        self.use_items()
        for lane in range(LANES):
            self.attack_lane(lane)
        return ";".join(self.actions) or "PASS"


def main():
    read = sys.stdin.readline
    while True:
        players = []
        for _ in range(2):
            line = read()
            if not line:
                return
            players.append([int(v) for v in line.split()])
        _, action_count = (int(v) for v in read().split()[:2])
        for _ in range(action_count):
            read()
        cards = [Card(read().split()) for _ in range(int(read()))]
        mana = players[0][1]
        if mana == 0:
            print(construct(cards), flush=True)
        else:
            print(Battle(cards, mana).play(), flush=True)


if __name__ == "__main__":
    main()
