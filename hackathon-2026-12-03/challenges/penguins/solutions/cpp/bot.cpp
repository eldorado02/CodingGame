// Penguins — C++ (submitted), v2 greedy + mobility. Port of solutions/python/bot.py.
// Rows 1, 3, ... (index 0, 2, ...) are the narrow shifted rows; see step().
#pragma GCC optimize("O3,unroll-loops")
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int SOURCE_WEIGHT = 10;   // fish eaten now outweigh fish reachable later
constexpr int MOBILITY_WEIGHT = 1;  // a penguin with few rays left gets trapped
typedef pair<int, int> Cell;        // (row, x)

// Neighbour in direction d (0 E, 1 W, 2 NE, 3 NW, 4 SE, 5 SW).
Cell step(Cell c, int d) {
    int r = c.first, x = c.second;
    bool narrow = r % 2 == 0;
    if (d == 0) return {r, x + 1};
    if (d == 1) return {r, x - 1};
    int dr = (d == 2 || d == 3) ? 1 : -1;
    bool right = d == 2 || d == 4;
    if (narrow) return {r + dr, right ? x + 1 : x};
    return {r + dr, right ? x : x - 1};
}

bool isIce(const vector<string> &b, const set<Cell> &blocked, Cell c) {
    int r = c.first, x = c.second;
    return r >= 0 && r < (int)b.size() && x >= 0 && x < (int)b[r].size() && isdigit((unsigned char)b[r][x])
        && b[r][x] != '0' && !blocked.count(c);
}

int mobility(const vector<string> &b, const set<Cell> &blocked, Cell from) {
    int n = 0;
    for (int d = 0; d < 6; ++d)
        for (Cell c = step(from, d); isIce(b, blocked, c); c = step(c, d)) ++n;
    return n;
}

bool parsePos(const string &pos, Cell &c) {
    if (pos.size() < 2 || !isalpha((unsigned char)pos[0])) return false;
    for (size_t i = 1; i < pos.size(); ++i) if (!isdigit((unsigned char)pos[i])) return false;
    c = {atoi(pos.c_str() + 1) - 1, toupper((unsigned char)pos[0]) - 'A'};
    return true;
}

int fishAt(const vector<string> &b, Cell c) {
    int r = c.first, x = c.second;
    if (r < 0 || r >= (int)b.size() || x < 0 || x >= (int)b[r].size() || !isdigit((unsigned char)b[r][x])) return -1;
    return b[r][x] - '0';
}

int score(vector<string> b, const map<string, Cell> &mine, const vector<Cell> &occupied, const string &action) {
    vector<string> parts;
    istringstream in(action);
    for (string t; in >> t;) parts.push_back(t);
    Cell dst;
    if (parts.empty() || !parsePos(parts.back(), dst)) return -1;
    int value = fishAt(b, dst);
    set<Cell> blocked(occupied.begin(), occupied.end());
    blocked.insert(dst);
    auto it = parts.size() >= 3 ? mine.find(parts[1]) : mine.end();
    if (it != mine.end()) {
        Cell src = it->second;
        value += SOURCE_WEIGHT * max(0, fishAt(b, src));
        blocked.erase(src);
        if (src.first >= 0 && src.first < (int)b.size() && src.second >= 0 && src.second < (int)b[src.first].size())
            b[src.first][src.second] = '0';
    }
    blocked.erase(dst);
    return value + MOBILITY_WEIGHT * mobility(b, blocked, dst);
}

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line, me;
    if (!getline(cin, line)) return 0;
    int players = atoi(line.c_str());
    if (!getline(cin, line)) return 0;
    me = trim(line);
    if (!getline(cin, line)) return 0;
    while (getline(cin, line)) {
        int rows = atoi(line.c_str());
        vector<string> board;
        for (int i = 0; i < rows; ++i) { if (!getline(cin, line)) return 0; board.push_back(trim(line)); }
        for (int i = 0; i < players; ++i) if (!getline(cin, line)) return 0;
        if (!getline(cin, line)) return 0;
        int count = atoi(line.c_str());
        map<string, Cell> mine;  // penguin ids are unique per player only
        vector<Cell> occupied;
        for (int i = 0; i < count; ++i) {
            if (!getline(cin, line)) return 0;
            istringstream in(line);
            string id, owner, pos;
            Cell c;
            if ((in >> id >> owner >> pos) && parsePos(pos, c)) {
                occupied.push_back(c);
                if (owner == me) mine[id] = c;
            }
        }
        if (!getline(cin, line)) return 0;
        int actions = atoi(line.c_str()), bestV = -2;
        string best;
        for (int i = 0; i < actions; ++i) {
            if (!getline(cin, line)) return 0;
            string action = trim(line);
            int v = action.empty() ? -2 : score(board, mine, occupied, action);
            if (v > bestV) { bestV = v; best = action; }
        }
        cout << (best.empty() ? "MOVE 0 A1" : best) << endl;
    }
}
