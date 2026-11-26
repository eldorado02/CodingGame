// Poker (Texas hold'em) — C++ (submitted), v2 Monte Carlo equity. Port of solutions/python/bot.py.
// Equity from SIMULATIONS deals of the unknown cards (deterministic xorshift32
// seeded by the round), 7-card evaluator (category then kickers). All-in at
// ALL_IN_PERCENT, minimum raise at RAISE_PERCENT, check when free, call when
// the equity beats the pot odds, else fold; short stack pushes at PUSH_PERCENT.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const string RANKS = "23456789TJQKA", SUITS = "HDSC";
enum Category { HIGH_CARD, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL_HOUSE, QUADS, STRAIGHT_FLUSH };
constexpr long long ALL_IN_PERCENT = 80, RAISE_PERCENT = 62, PUSH_PERCENT = 50, SHORT_STACK_BLINDS = 10;
constexpr int BOARD_SIZE = 5, DECK = 52;
constexpr uint64_t SEED_SALT = 2654435761ULL;
constexpr long long BLIND_CAP = 1000000000LL;  // blinds keep doubling: stay in 64 bits
int simulations = 400;

// xorshift32: identical sequence in every language.
struct Rng {
    uint32_t state;
    explicit Rng(uint64_t seed) : state((uint32_t)seed ? (uint32_t)seed : 1u) {}
    uint32_t next() {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return state = x;
    }
};

vector<int> parse(const string &cards) {  // card index = rank * 4 + suit
    vector<int> out;
    stringstream ss(cards);
    string c;
    while (getline(ss, c, '_'))
        if (c.size() == 2 && RANKS.find(c[0]) != string::npos && SUITS.find(c[1]) != string::npos)
            out.push_back((int)RANKS.find(c[0]) * 4 + (int)SUITS.find(c[1]));
    return out;
}

// Highest straight top rank among the present ranks (wheel = 3), or -1.
int straightTop(const bool *present) {
    for (int top = 12; top > 3; --top) {
        bool run = true;
        for (int k = 0; k < 5; ++k) run = run && present[top - k];
        if (run) return top;
    }
    if (present[12] && present[0] && present[1] && present[2] && present[3]) return 3;
    return -1;
}

long long encode(int category, const vector<int> &kickers) {
    long long value = category;
    for (int k = 0; k < 5; ++k) value = value * 16 + (k < (int)kickers.size() ? kickers[k] : 0);
    return value;
}

// Comparable value of the best 5-card hand among the cards.
long long evaluate(const vector<int> &cards) {
    int counts[13] = {};
    bool bySuit[4][13] = {};
    int suitCount[4] = {};
    for (int c : cards) {
        ++counts[c / 4];
        if (!bySuit[c % 4][c / 4]) { bySuit[c % 4][c / 4] = true; ++suitCount[c % 4]; }
    }
    for (int s = 0; s < 4; ++s)
        if (suitCount[s] >= 5) {
            int top = straightTop(bySuit[s]);
            if (top >= 0) return encode(STRAIGHT_FLUSH, {top});
        }
    vector<int> ranks, quads, trips, pairs;
    for (int r = 12; r >= 0; --r) {
        if (!counts[r]) continue;
        ranks.push_back(r);
        if (counts[r] == 4) quads.push_back(r);
        if (counts[r] == 3) trips.push_back(r);
        if (counts[r] == 2) pairs.push_back(r);
    }
    auto others = [&](const vector<int> &skip, int n) {
        vector<int> out;
        for (int r : ranks)
            if (find(skip.begin(), skip.end(), r) == skip.end() && (int)out.size() < n) out.push_back(r);
        return out;
    };
    if (!quads.empty()) {
        vector<int> k = {quads[0]}, rest = others({quads[0]}, 1);
        k.insert(k.end(), rest.begin(), rest.end());
        return encode(QUADS, k);
    }
    if (!trips.empty() && (trips.size() > 1 || !pairs.empty())) {
        int second = -1;
        for (size_t i = 1; i < trips.size(); ++i) second = max(second, trips[i]);
        for (int p : pairs) second = max(second, p);
        return encode(FULL_HOUSE, {trips[0], second});
    }
    for (int s = 0; s < 4; ++s)
        if (suitCount[s] >= 5) {
            vector<int> k;
            for (int r = 12; r >= 0 && (int)k.size() < 5; --r)
                if (bySuit[s][r]) k.push_back(r);
            return encode(FLUSH, k);
        }
    bool present[13];
    for (int r = 0; r < 13; ++r) present[r] = counts[r] > 0;
    int top = straightTop(present);
    if (top >= 0) return encode(STRAIGHT, {top});
    if (!trips.empty()) {
        vector<int> k = {trips[0]}, rest = others({trips[0]}, 2);
        k.insert(k.end(), rest.begin(), rest.end());
        return encode(TRIPS, k);
    }
    if (pairs.size() >= 2) {
        vector<int> k = {pairs[0], pairs[1]}, rest = others({pairs[0], pairs[1]}, 1);
        k.insert(k.end(), rest.begin(), rest.end());
        return encode(TWO_PAIR, k);
    }
    if (!pairs.empty()) {
        vector<int> k = {pairs[0]}, rest = others({pairs[0]}, 3);
        k.insert(k.end(), rest.begin(), rest.end());
        return encode(PAIR, k);
    }
    return encode(HIGH_CARD, others({}, 5));
}

// Twice the wins + ties over twice the simulations.
pair<long long, long long> equity(const vector<int> &hole, const vector<int> &board, int opponents, uint64_t seed) {
    Rng rng(seed);
    vector<int> deck;
    for (int c = 0; c < DECK; ++c)
        if (find(hole.begin(), hole.end(), c) == hole.end() && find(board.begin(), board.end(), c) == board.end()) deck.push_back(c);
    int missing = BOARD_SIZE - (int)board.size(), need = missing + 2 * opponents;
    long long score = 0;
    for (int sim = 0; sim < simulations; ++sim) {
        for (int k = 0; k < need; ++k) swap(deck[k], deck[k + rng.next() % (deck.size() - k)]);
        vector<int> fullBoard = board;
        fullBoard.insert(fullBoard.end(), deck.begin(), deck.begin() + missing);
        vector<int> mine = hole;
        mine.insert(mine.end(), fullBoard.begin(), fullBoard.end());
        long long ours = evaluate(mine), best = 0;
        for (int o = 0; o < opponents; ++o) {
            vector<int> theirs(deck.begin() + missing + 2 * o, deck.begin() + missing + 2 * o + 2);
            theirs.insert(theirs.end(), fullBoard.begin(), fullBoard.end());
            best = max(best, evaluate(theirs));
        }
        score += ours > best ? 2 : ours == best ? 1 : 0;
    }
    return {score, 2LL * simulations};
}

long long bigBlind(long long start, long long multiplier, long long perLevel, long long hand) {
    long long blind = start;
    for (long long i = 0; i < (hand - 1) / max(perLevel, 1LL); ++i) blind = min(blind * multiplier, BLIND_CAP);
    return blind;
}

string decide(long long score, long long total, const vector<string> &possible, long long toCall, long long pot, long long stack,
              long long blind) {
    map<string, string> names;
    for (const string &p : possible) {
        string head = p.substr(0, p.find('_'));
        if (!names.count(head)) names[head] = p;
    }
    auto spaced = [](string s) { replace(s.begin(), s.end(), '_', ' '); return s; };
    bool push = stack <= SHORT_STACK_BLINDS * blind && score * 100 >= PUSH_PERCENT * total;
    if (names.count("ALL-IN") && (score * 100 >= ALL_IN_PERCENT * total || push)) return "ALL-IN";
    if (score * 100 >= RAISE_PERCENT * total && names.count("BET")) return spaced(names["BET"]);
    if (names.count("CHECK")) return "CHECK";
    if (names.count("CALL") && score * (pot + toCall) > total * toCall) return "CALL";
    if (names.count("FOLD")) return "FOLD";
    return possible.empty() ? "FOLD" : spaced(possible[0]);
}

int main() {
    ios::sync_with_stdio(false);
    if (const char *e = getenv("CG_NODES")) simulations = atoi(e);
    vector<long long> init(8);
    for (long long &v : init)
        if (!(cin >> v)) return 0;
    long long big = init[1], perLevel = init[2], multiplier = init[3];
    int players = (int)init[6], me = (int)init[7];
    string line;
    long long round, hand;
    while (cin >> round >> hand) {
        vector<pair<long long, long long>> seats(players);
        for (auto &s : seats) cin >> s.first >> s.second;
        string boardText, holeText;
        cin >> boardText >> holeText;
        for (int block = 0; block < 2; ++block) {  // past actions, then showdowns
            int count;
            cin >> count;
            getline(cin, line);
            for (int i = 0; i < count; ++i) getline(cin, line);
        }
        int count;
        cin >> count;
        vector<string> possible(max(0, count));
        for (string &p : possible) cin >> p;
        if (!cin) return 0;
        long long top = 0, pot = 0;
        int opponents = 0;
        for (int k = 0; k < players; ++k) {
            top = max(top, seats[k].second);
            pot += seats[k].second;
            if (k != me && seats[k].first + seats[k].second > 0) ++opponents;
        }
        vector<int> hole = parse(holeText), board = parse(boardText);
        pair<long long, long long> eq{0, 1};
        if (hole.size() == 2) eq = equity(hole, board, max(1, opponents), (uint64_t)round * SEED_SALT + (uint64_t)hand);
        cout << decide(eq.first, eq.second, possible, top - seats[me].second, pot, seats[me].first, bigBlind(big, multiplier, perLevel, hand))
             << endl;
    }
}
