// Dice Shogi 5x5 — C++ (submitted), v1 greedy. Port of solutions/python/bot.py.
#pragma GCC optimize("O3,unroll-loops")
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifndef DICE
#define DICE 1
#endif

typedef vector<vector<string>> Board;
const int ORTH[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
const int DIAG[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
const int GOLD[6][2] = {{0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0}, {0, 1}};
const int SILVER[5][2] = {{0, -1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

int val(const string &k) {
    if (k == "R") return 9; if (k == "B") return 8; if (k == "G") return 6;
    if (k == "S") return 5; if (k == "P") return 1; if (k == "+R") return 11;
    if (k == "+B") return 10; if (k == "+S") return 6; if (k == "+P") return 6;
    return 0;
}

vector<string> parseRow(const string &s) {
    vector<string> out;
    size_t i = 0;
    while (i < s.size() && out.size() < 5) {
        if (s[i] == '+' && i + 1 < s.size()) { out.push_back(s.substr(i, 2)); i += 2; }
        else { out.push_back(string(1, s[i])); ++i; }
    }
    while (out.size() < 5) out.push_back(".");
    return out;
}

int owner(const string &t) {
    char c = t.back();
    if (c == '.') return -1;
    return isupper((unsigned char)c) ? 0 : 1;
}

string kindOf(const string &t) {
    string r = t;
    for (auto &c : r) c = (char)toupper((unsigned char)c);
    return r;
}

void attacks(const Board &b, int side, bool att[5][5]) {
    for (int y = 0; y < 5; ++y) for (int x = 0; x < 5; ++x) att[y][x] = false;
    int fwd = side == 0 ? -1 : 1;
    for (int y = 0; y < 5; ++y) for (int x = 0; x < 5; ++x) {
        const string &t = b[y][x];
        if (owner(t) != side) continue;
        string k = kindOf(t);
        vector<pair<int, int>> steps, slides;
        auto add = [](vector<pair<int, int>> &v, const int (*d)[2], int n) { for (int i = 0; i < n; ++i) v.push_back({d[i][0], d[i][1]}); };
        if (k == "K") { add(steps, ORTH, 4); add(steps, DIAG, 4); }
        else if (k == "G" || k == "+S" || k == "+P") add(steps, GOLD, 6);
        else if (k == "S") add(steps, SILVER, 5);
        else if (k == "P") steps.push_back({0, -1});
        else if (k == "R") add(slides, ORTH, 4);
        else if (k == "B") add(slides, DIAG, 4);
        else if (k == "+R") { add(slides, ORTH, 4); add(steps, DIAG, 4); }
        else if (k == "+B") { add(slides, DIAG, 4); add(steps, ORTH, 4); }
        for (auto &d : steps) {
            int nx = x + d.first, ny = y + d.second * fwd;
            if (nx >= 0 && nx < 5 && ny >= 0 && ny < 5) att[ny][nx] = true;
        }
        for (auto &d : slides) {
            int nx = x + d.first, ny = y + d.second;
            while (nx >= 0 && nx < 5 && ny >= 0 && ny < 5) {
                att[ny][nx] = true;
                if (b[ny][nx] != ".") break;
                nx += d.first; ny += d.second;
            }
        }
    }
}

bool sq(char c, char r, int &x, int &y) {
    if (c < '1' || c > '5' || r < '1' || r > '5') return false;
    x = 5 - (c - '0'); y = (r - '0') - 1;
    return true;
}

bool score(const Board &b, int me, bool att[5][5], const string &a, int &out) {
    int x, y, sx, sy;
    if (a.size() >= 4 && a[1] == '*') {
        if (!sq(a[2], a[3], x, y)) return false;
        string k(1, (char)toupper((unsigned char)a[0]));
        out = att[y][x] ? -10 * val(k) : 0;
        return true;
    }
    if (a.size() < 4) return false;
    if (!sq(a[0], a[1], sx, sy) || !sq(a[2], a[3], x, y)) return false;
    const string &mover = b[sy][sx], &target = b[y][x];
    int sc = 0;
    if (target != "." && owner(target) != me) sc += 10 * val(kindOf(target));
    if (a.back() == '+') sc += 4;
    string k = kindOf(mover);
    if (k == "K") sc -= 1;
    else if (att[y][x]) sc -= 10 * val(k);
    out = sc;
    return true;
}

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line;
    if (!getline(cin, line)) return 0;
    int me = trim(line) == "1" ? 1 : 0;
    if (!getline(cin, line)) return 0;
    string sz = trim(line);
    int n = (!sz.empty() && isdigit((unsigned char)sz[0])) ? atoi(sz.c_str()) : 5;
    while (true) {
        Board b;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            b.push_back(parseRow(trim(line)));
        }
        for (int i = 0; i < 3 + DICE; ++i) if (!getline(cin, line)) return 0;
        if (!getline(cin, line)) return 0;
        int c = atoi(line.c_str());
        bool att[5][5];
        attacks(b, 1 - me, att);
        string best;
        bool have = false;
        int bestV = 0;
        for (int i = 0; i < c; ++i) {
            if (!getline(cin, line)) return 0;
            string t = trim(line);
            size_t sp = t.find_first_of(" \t");
            if (sp != string::npos) t = t.substr(0, sp);
            if (t.empty()) continue;
            int v;
            if (score(b, me, att, t, v) && (!have || v > bestV)) { have = true; bestV = v; best = t; }
        }
        cout << (have ? best : "random") << endl;
    }
}
