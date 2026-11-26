#!/usr/bin/env python3
"""Fireworks (Hanabi) — Python reference of the differential fuzz, v1 safe conventions.

Input (probe 2026-11-26): each turn the events since our last turn, then the
full hands of the other players ("p:CARD:L:COLOR-N"). Hint results follow the
SAY event ("q:CARD:L:?-N" / "q:CARD:L:COLOR-?"): they are knowledge lines. A
played / discarded card is replaced in the same slot while the deck lasts
(50 cards, 15 dealt).
Our turn, first match wins:
1. play a card we know is playable;
2. hint (next teammate first) a playable card they do not know, with the
   level, else the colour, only when the hint flags no unplayable card;
3. discard (info below the maximum): a known useless card, else the first
   slot without any hint, else slot A;
4. a harmless hint (no discard allowed).
"""
import sys

COLORS = ("WHITE", "RED", "BLUE", "GREEN", "YELLOW")
SLOTS = "ABCDE"
TEAM = 3                    # players per round
MAX_INFO = 12
DECK = 50
HAND = 5
UNKNOWN = "?"


class Round:
    def __init__(self, me):
        self.reset(me)

    def reset(self, me):
        self.me = me
        self.fireworks = {c: 0 for c in COLORS}
        self.deck = DECK - TEAM * HAND
        self.my_slots = list(SLOTS)
        self.knowledge = {}         # (player, slot) -> [colour or None, level or None]
        self.hands = {}             # player -> {slot: (colour, level)}

    def forget(self, player, slot):
        self.knowledge.pop((player, slot), None)
        if self.deck > 0:
            self.deck -= 1
        elif player == self.me and slot in self.my_slots:
            self.my_slots.remove(slot)

    def learn(self, player, slot, value):
        colour, _, level = value.partition("-")
        known = self.knowledge.setdefault((player, slot), [None, None])
        if colour != UNKNOWN and colour in COLORS:
            known[0] = colour
        if level != UNKNOWN and level.isdigit():
            known[1] = int(level)

    def visible_card(self, line):
        """(player, slot, colour, level) of a full card line of a teammate, else None."""
        parts = line.split(":")
        if len(parts) != 4 or not parts[0].isdigit() or parts[1] != "CARD" or int(parts[0]) == self.me:
            return None
        colour, _, level = parts[3].partition("-")
        if colour not in COLORS or not level.isdigit():
            return None
        return int(parts[0]), parts[2], colour, int(level)

    def apply(self, events):
        """Events first, then the teammates' hands: the trailing block of full
        card lines (a full knowledge line just before it is the same card).
        A NEWGAME line starts a new round: the events before it are dropped."""
        last_new = -1
        for k, line in enumerate(events):
            parts = line.split(":")
            if len(parts) >= 2 and parts[1] == "NEWGAME" and parts[0].isdigit():
                last_new = k
        if last_new >= 0:
            self.reset(int(events[last_new].split(":")[0]))
            events = events[last_new + 1:]
        tail = len(events)
        while tail > 0 and self.visible_card(events[tail - 1]):
            tail -= 1
        self.hands = {}
        for line in events[tail:]:
            player, slot, colour, level = self.visible_card(line)
            self.hands.setdefault(player, {})[slot] = (colour, level)
        hinted = None
        for line in events[:tail]:
            parts = line.split(":")
            if len(parts) < 2 or not parts[0].isdigit():
                continue
            player, kind = int(parts[0]), parts[1]
            if kind in ("PLAY", "ERROR", "DISCARD") and len(parts) >= 3:
                if kind == "PLAY" and len(parts) >= 4:
                    colour, _, level = parts[3].partition("-")
                    if colour in COLORS and level.isdigit():
                        self.fireworks[colour] = max(self.fireworks[colour], int(level))
                self.forget(player, parts[2])
            elif kind in ("SAYLEVEL", "SAYCOLOR") and len(parts) >= 3 and parts[2].isdigit():
                hinted = int(parts[2])
            elif kind == "CARD" and len(parts) >= 4 and player == hinted:
                self.learn(player, parts[2], parts[3])

    # ---- our cards
    def possible_colours(self, slot):
        colour = self.knowledge.get((self.me, slot), [None, None])[0]
        return [colour] if colour else list(COLORS)

    def known_playable(self, slot):
        level = self.knowledge.get((self.me, slot), [None, None])[1]
        return level is not None and all(self.fireworks[c] == level - 1 for c in self.possible_colours(slot))

    def known_useless(self, slot):
        level = self.knowledge.get((self.me, slot), [None, None])[1]
        return level is not None and all(self.fireworks[c] >= level for c in self.possible_colours(slot))

    # ---- teammates
    def playable(self, card):
        return self.fireworks[card[0]] == card[1] - 1

    def knows_playable(self, player, slot):
        colour, level = self.knowledge.get((player, slot), [None, None])
        return colour is not None and level is not None

    def hint_for(self, player):
        """Level (index 1) then colour (index 0) hint flagging only playable cards;
        an attribute the player already knows is never repeated (the boss
        plays a card once it knows both)."""
        hand = self.hands.get(player, {})
        for slot in sorted(hand):
            card = hand[slot]
            if not self.playable(card) or self.knows_playable(player, slot):
                continue
            known = self.knowledge.get((player, slot), [None, None])
            for index in (1, 0):
                if known[index] is not None:
                    continue
                flagged = [c for c in hand.values() if c[index] == card[index]]
                if all(self.playable(c) for c in flagged):
                    return "SAY:%d:%s" % (player, card[index])
        return None

    def teammates(self):
        return [(self.me + k) % TEAM for k in range(1, TEAM)]

    def harmless_hint(self):
        for player in self.teammates():
            hand = self.hands.get(player, {})
            for slot in sorted(hand):
                colour, level = hand[slot]
                if self.fireworks[colour] >= level:
                    return "SAY:%d:%s" % (player, colour)
        for player in self.teammates():
            hand = self.hands.get(player, {})
            if hand:
                return "SAY:%d:%d" % (player, hand[sorted(hand)[0]][1])
        return "SAY:%d:1" % self.teammates()[0]

    def decide(self, info):
        for slot in self.my_slots:
            if self.known_playable(slot):
                return "PLAY:" + slot
        if info > 0:
            for player in self.teammates():
                hint = self.hint_for(player)
                if hint:
                    return hint
        if info < MAX_INFO and self.my_slots:
            useless = [s for s in self.my_slots if self.known_useless(s)]
            blank = [s for s in self.my_slots if (self.me, s) not in self.knowledge]
            return "DISCARD:" + (useless or blank or self.my_slots)[0]
        return self.harmless_hint()


def main():
    read = sys.stdin.readline
    game = Round(0)
    while True:
        head = read().split()
        if len(head) < 2:
            return
        info = int(head[1])
        events = [read().strip() for _ in range(int(read()))]
        game.apply(events)
        print(game.decide(info), flush=True)


if __name__ == "__main__":
    main()
