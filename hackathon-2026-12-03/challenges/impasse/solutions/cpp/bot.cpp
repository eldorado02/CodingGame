// Impasse — C++ (submitted), v1 greedy. Port of solutions/python/bot.py.
#pragma GCC optimize("O3,unroll-loops")
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// (col, row 1..8) pairs; false if malformed
bool parseCoords(const string &t, vector<pair<int, int>> &out) {
    out.clear();
    if (t.empty() || t.size() % 2) return false;
    for (size_t i = 0; i < t.size(); i += 2) {
        char c = t[i], r = t[i + 1];
        if (c < 'a' || c > 'h' || r < '1' || r > '8') return false;
        out.push_back({c - 'a', r - '0'});
    }
    return true;
}

int score(const vector<string> &board, char me, const vector<pair<int, int>> &co) {
    if (co.size() == 1) return 50;
    bool up = me == 'w';
    int nearR = up ? 1 : 8, farR = up ? 8 : 1;
    int c1 = co[0].first, r1 = co[0].second, c2 = co[1].first, r2 = co[1].second;
    char src = board[8 - r1][c1], dst = board[8 - r2][c2];
    char meU = up ? 'W' : 'B';
    int sc = 0, rows = abs(r2 - r1);
    if (src == meU) {
        if (dst == me) sc += 12 + (r2 == nearR ? 20 : 0);
        else sc += 3 * rows + (r2 == nearR ? 25 : 0);
    } else {
        sc += 3 * rows + (r2 == farR ? 8 : 0);
    }
    if (co.size() >= 3) sc += 2;
    return sc;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line;
    if (!getline(cin, line)) return 0;
    char me = 'w';
    for (char ch : line) if (ch != ' ') { me = (ch == 'b' || ch == 'B') ? 'b' : 'w'; break; }
    while (true) {
        vector<string> board(8);
        for (int y = 0; y < 8; ++y) {
            if (!getline(cin, line)) return 0;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line += "........";
            board[y] = line.substr(0, 8);
        }
        if (!getline(cin, line)) return 0;
        if (!getline(cin, line)) return 0;
        int n = atoi(line.c_str()), bestV = -1;
        string best;
        vector<pair<int, int>> co;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            size_t a = line.find_first_not_of(" \t");
            if (a == string::npos) continue;
            size_t b = line.find_first_of(" \t\r", a);
            string t = line.substr(a, b == string::npos ? string::npos : b - a);
            if (!parseCoords(t, co)) continue;
            int v = score(board, me, co);
            if (v > bestV) { bestV = v; best = t; }
        }
        cout << (best.empty() ? "random" : best) << endl;
    }
}
