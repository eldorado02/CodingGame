// 6 nimmt! — C++ (submitted), v1 expected-penalty greedy. Port of solutions/python/bot.py.
// Card cost = expected cows: Binomial(others, gap / unseen) cards land in the
// gap before ours; the row is taken when they fill it. Below every row: the
// cheapest row. Ties: cost, gap, card. Line pick: fewest cows, lowest index.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

const string PHASE_PLAY = "CHOOSE_CARD_TO_PLAY";
constexpr int DECK_SIZE = 104;
constexpr int HAND_SIZE = 10;  // a full hand means a new round: forget the seen cards
constexpr int ROW_LIMIT = 5;   // the sixth card of a row takes the five others
constexpr int ROWS = 4;
constexpr int DISQUALIFIED_SCORE = -999;

using Row = vector<int>;

int cows(int card) {
    if (card == 55) return 7;
    if (card % 11 == 0) return 5;
    if (card % 10 == 0) return 3;
    if (card % 5 == 0) return 2;
    return 1;
}

int rowCows(const Row &row) {
    int total = 0;
    for (int c : row) total += cows(c);
    return total;
}

// Index of the row the card goes on, or -1 when it is below every row.
int targetRow(const vector<Row> &rows, int card) {
    int best = -1;
    for (int i = 0; i < (int)rows.size(); ++i)
        if (rows[i].back() < card && (best < 0 || rows[i].back() > rows[best].back())) best = i;
    return best;
}

long long binomial(int n, int k) {
    long long value = 1;
    for (int i = 0; i < k; ++i) value = value * (n - i) / (i + 1);
    return value;
}

// P(Binomial(draws, p) >= count), products written out for identical floats.
double atLeast(int count, int draws, double p) {
    if (count <= 0) return 1.0;
    double total = 0.0;
    for (int j = count; j <= draws; ++j) {
        double term = 1.0;
        for (int i = 0; i < j; ++i) term *= p;
        for (int i = 0; i < draws - j; ++i) term *= 1.0 - p;
        total += (double)binomial(draws, j) * term;
    }
    return total;
}

int gapUnseen(const vector<bool> &seen, int low, int high) {
    int n = 0;
    for (int c = low + 1; c < high; ++c) n += !seen[c];
    return n;
}

// (expected cows, gap) of playing this card now.
pair<double, int> playCost(const vector<Row> &rows, const vector<bool> &seen, int unseen, int others, int card) {
    int r = targetRow(rows, card);
    if (r < 0) {
        int cheapest = rowCows(rows[0]);
        for (const Row &row : rows) cheapest = min(cheapest, rowCows(row));
        return {(double)cheapest, 0};
    }
    int gap = gapUnseen(seen, rows[r].back(), card);
    double p = unseen ? (double)gap / unseen : 0.0;
    int slots = ROW_LIMIT - (int)rows[r].size();
    return {atLeast(slots, others, p) * rowCows(rows[r]), gap};
}

int chooseCard(const vector<Row> &rows, vector<int> hand, const vector<bool> &seen, int others) {
    int unseen = 0;
    for (int c = 1; c <= DECK_SIZE; ++c) unseen += !seen[c];
    sort(hand.begin(), hand.end());
    int best = hand[0];
    tuple<double, int, int> bestKey;
    bool first = true;
    for (int card : hand) {
        auto [cost, gap] = playCost(rows, seen, unseen, others, card);
        tuple<double, int, int> key{cost, gap, card};
        if (first || key < bestKey) { best = card; bestKey = key; first = false; }
    }
    return best;
}

int chooseRow(const vector<Row> &rows) {
    int best = 0;
    for (int i = 1; i < (int)rows.size(); ++i)
        if (rowCows(rows[i]) < rowCows(rows[best])) best = i;
    return best;
}

vector<int> readInts(istream &in) {
    string line;
    getline(in, line);
    istringstream ss(line);
    vector<int> out;
    int v;
    while (ss >> v) out.push_back(v);
    return out;
}

int main() {
    ios::sync_with_stdio(false);
    vector<int> header = readInts(cin);
    if (header.empty()) return 0;
    int playerCount = header[0];
    vector<bool> seen(DECK_SIZE + 2, false);
    // The real referee sends the phase as the LAST line of a turn (the statement
    // lists it first): last cards, rows, scores, hand size, hand, phase.
    while (cin.peek() != EOF) {
        vector<int> lastPlayed = readInts(cin);
        vector<Row> rows;
        for (int i = 0; i < ROWS; ++i) {
            readInts(cin);
            Row row = readInts(cin);
            rows.push_back(row.empty() ? Row{0} : row);
        }
        vector<int> scores = readInts(cin);
        readInts(cin);
        vector<int> hand = readInts(cin);
        string phase;
        getline(cin, phase);
        phase.erase(phase.find_last_not_of(" \t\r\n") + 1);
        if (phase == PHASE_PLAY && (int)hand.size() == HAND_SIZE) seen.assign(DECK_SIZE + 2, false);
        auto markSeen = [&](int card) { if (card >= 1 && card <= DECK_SIZE) seen[card] = true; };
        for (int c : lastPlayed) markSeen(c);
        for (int c : hand) markSeen(c);
        for (const Row &row : rows) for (int c : row) markSeen(c);
        int active = 0;
        for (int i = 0; i < (int)scores.size() && i < playerCount; ++i) active += scores[i] != DISQUALIFIED_SCORE;
        int others = max(0, active - 1);
        if (phase == PHASE_PLAY && !hand.empty()) cout << "PLAY " << chooseCard(rows, hand, seen, others) << endl;
        else cout << "PICK " << chooseRow(rows) << endl;
    }
}
