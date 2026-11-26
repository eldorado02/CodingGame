// Fireworks (Hanabi) — C++ (submitted), v1 safe conventions. Port of solutions/python/bot.py.
// Input: events since our last turn, then the teammates' full hands (trailing
// block of "p:CARD:L:COLOR-N"); hint results follow the SAY event. Played or
// discarded cards are replaced in the same slot while the deck lasts.
// Turn: play a known playable card > hint a playable card (attribute not yet
// known, flags only playable cards) > discard (useless, unhinted, A) > safe hint.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const vector<string> COLORS = {"WHITE", "RED", "BLUE", "GREEN", "YELLOW"};
const string SLOTS = "ABCDE";
constexpr int TEAM = 3;  // players per round
constexpr int MAX_INFO = 12, DECK = 50, HAND = 5;
constexpr int NONE = -1;
const string UNKNOWN = "?";

vector<string> split(const string &text, char sep) {
    vector<string> parts;
    string cur;
    for (char ch : text) {
        if (ch == sep) { parts.push_back(cur); cur.clear(); }
        else cur += ch;
    }
    parts.push_back(cur);
    return parts;
}

bool isDigits(const string &s) { return !s.empty() && all_of(s.begin(), s.end(), [](char c) { return c >= '0' && c <= '9'; }); }

int colourIndex(const string &name) {
    auto it = find(COLORS.begin(), COLORS.end(), name);
    return it == COLORS.end() ? NONE : int(it - COLORS.begin());
}

// "COLOR-N" -> (colour, level text) split at the first '-'.
pair<string, string> partition(const string &value) {
    size_t dash = value.find('-');
    if (dash == string::npos) return {value, ""};
    return {value.substr(0, dash), value.substr(dash + 1)};
}

struct Card { int colour, level; };
struct Knowledge { int colour = NONE, level = NONE; };

struct Round {
    int me = 0, deck = 0;
    array<int, 5> fireworks{};
    vector<string> mySlots;
    map<pair<int, string>, Knowledge> knowledge;
    map<int, map<string, Card>> hands;

    Round() { reset(0); }

    void reset(int player) {
        me = player;
        fireworks.fill(0);
        deck = DECK - TEAM * HAND;
        mySlots.clear();
        for (char s : SLOTS) mySlots.push_back(string(1, s));
        knowledge.clear();
        hands.clear();
    }

    void forget(int player, const string &slot) {
        knowledge.erase({player, slot});
        if (deck > 0) --deck;
        else if (player == me) {
            auto it = find(mySlots.begin(), mySlots.end(), slot);
            if (it != mySlots.end()) mySlots.erase(it);
        }
    }

    void learn(int player, const string &slot, const string &value) {
        auto [colour, level] = partition(value);
        Knowledge &k = knowledge[{player, slot}];
        if (colour != UNKNOWN && colourIndex(colour) != NONE) k.colour = colourIndex(colour);
        if (level != UNKNOWN && isDigits(level)) k.level = stoi(level);
    }

    // Full card line of a teammate: fills player, slot and card.
    bool visibleCard(const string &line, int &player, string &slot, Card &card) const {
        vector<string> parts = split(line, ':');
        if (parts.size() != 4 || !isDigits(parts[0]) || parts[1] != "CARD" || stoi(parts[0]) == me) return false;
        auto [colour, level] = partition(parts[3]);
        if (colourIndex(colour) == NONE || !isDigits(level)) return false;
        player = stoi(parts[0]);
        slot = parts[2];
        card = {colourIndex(colour), stoi(level)};
        return true;
    }

    void apply(vector<string> events) {
        int lastNew = NONE;
        for (int k = 0; k < (int)events.size(); ++k) {
            vector<string> parts = split(events[k], ':');
            if (parts.size() >= 2 && parts[1] == "NEWGAME" && isDigits(parts[0])) lastNew = k;
        }
        if (lastNew != NONE) {
            reset(stoi(split(events[lastNew], ':')[0]));
            events.erase(events.begin(), events.begin() + lastNew + 1);
        }
        int player = 0;
        string slot;
        Card card{0, 0};
        int tail = (int)events.size();
        while (tail > 0 && visibleCard(events[tail - 1], player, slot, card)) --tail;
        hands.clear();
        for (int k = tail; k < (int)events.size(); ++k) {
            visibleCard(events[k], player, slot, card);
            hands[player][slot] = card;
        }
        int hinted = NONE;
        for (int k = 0; k < tail; ++k) {
            vector<string> parts = split(events[k], ':');
            if (parts.size() < 2 || !isDigits(parts[0])) continue;
            int who = stoi(parts[0]);
            const string &kind = parts[1];
            if ((kind == "PLAY" || kind == "ERROR" || kind == "DISCARD") && parts.size() >= 3) {
                if (kind == "PLAY" && parts.size() >= 4) {
                    auto [colour, level] = partition(parts[3]);
                    if (colourIndex(colour) != NONE && isDigits(level))
                        fireworks[colourIndex(colour)] = max(fireworks[colourIndex(colour)], stoi(level));
                }
                forget(who, parts[2]);
            } else if ((kind == "SAYLEVEL" || kind == "SAYCOLOR") && parts.size() >= 3 && isDigits(parts[2])) {
                hinted = stoi(parts[2]);
            } else if (kind == "CARD" && parts.size() >= 4 && who == hinted) {
                learn(who, parts[2], parts[3]);
            }
        }
    }

    Knowledge known(int player, const string &slot) const {
        auto it = knowledge.find({player, slot});
        return it == knowledge.end() ? Knowledge{} : it->second;
    }

    vector<int> possibleColours(const string &slot) const {
        int colour = known(me, slot).colour;
        if (colour != NONE) return {colour};
        return {0, 1, 2, 3, 4};
    }

    bool knownPlayable(const string &slot) const {
        int level = known(me, slot).level;
        if (level == NONE) return false;
        for (int c : possibleColours(slot))
            if (fireworks[c] != level - 1) return false;
        return true;
    }

    bool knownUseless(const string &slot) const {
        int level = known(me, slot).level;
        if (level == NONE) return false;
        for (int c : possibleColours(slot))
            if (fireworks[c] < level) return false;
        return true;
    }

    bool playable(const Card &c) const { return fireworks[c.colour] == c.level - 1; }

    // Level then colour hint flagging only playable cards; never repeats an
    // attribute the player already knows (the boss plays a card it knows fully).
    string hintFor(int player) const {
        auto hit = hands.find(player);
        if (hit == hands.end()) return "";
        for (const auto &[slot, card] : hit->second) {
            Knowledge k = known(player, slot);
            if (!playable(card) || (k.colour != NONE && k.level != NONE)) continue;
            for (int index : {1, 0}) {
                if ((index == 1 ? k.level : k.colour) != NONE) continue;
                bool safe = true;
                for (const auto &entry : hit->second) {
                    const Card &other = entry.second;
                    bool flagged = index == 1 ? other.level == card.level : other.colour == card.colour;
                    if (flagged && !playable(other)) safe = false;
                }
                if (safe)
                    return "SAY:" + to_string(player) + ":" + (index == 1 ? to_string(card.level) : COLORS[card.colour]);
            }
        }
        return "";
    }

    vector<int> teammates() const {
        vector<int> out;
        for (int k = 1; k < TEAM; ++k) out.push_back((me + k) % TEAM);
        return out;
    }

    string harmlessHint() const {
        for (int player : teammates()) {
            auto hit = hands.find(player);
            if (hit == hands.end()) continue;
            for (const auto &entry : hit->second)
                if (fireworks[entry.second.colour] >= entry.second.level)
                    return "SAY:" + to_string(player) + ":" + COLORS[entry.second.colour];
        }
        for (int player : teammates()) {
            auto hit = hands.find(player);
            if (hit != hands.end() && !hit->second.empty())
                return "SAY:" + to_string(player) + ":" + to_string(hit->second.begin()->second.level);
        }
        return "SAY:" + to_string(teammates()[0]) + ":1";
    }

    string decide(int info) const {
        for (const string &slot : mySlots)
            if (knownPlayable(slot)) return "PLAY:" + slot;
        if (info > 0)
            for (int player : teammates()) {
                string hint = hintFor(player);
                if (!hint.empty()) return hint;
            }
        if (info < MAX_INFO && !mySlots.empty()) {
            for (const string &slot : mySlots)
                if (knownUseless(slot)) return "DISCARD:" + slot;
            for (const string &slot : mySlots)
                if (!knowledge.count({me, slot})) return "DISCARD:" + slot;
            return "DISCARD:" + mySlots[0];
        }
        return harmlessHint();
    }
};

int main() {
    ios::sync_with_stdio(false);
    Round game;
    string line;
    while (getline(cin, line)) {
        istringstream head(line);
        int mistakes, info;
        if (!(head >> mistakes >> info)) return 0;
        if (!getline(cin, line)) return 0;
        int count = stoi(line);
        vector<string> events(max(0, count));
        for (string &e : events) {
            getline(cin, e);
            e.erase(e.find_last_not_of(" \t\r\n") + 1);
            e.erase(0, e.find_first_not_of(" \t\r\n") == string::npos ? e.size() : e.find_first_not_of(" \t\r\n"));
        }
        game.apply(events);
        cout << game.decide(info) << endl;
    }
}
