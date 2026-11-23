// Nine Men's Morris — C++ (submitted), v1 alpha-beta. Port of solutions/python/bot.py.
// Mills = aligned triples derived from the adjacency given at init.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int STONES = 9, EMPTY = -1;
constexpr long long FIRST_TURN_NODES = 15000, TURN_NODES = 15000;
constexpr int TURN_MS = 35;
constexpr int STONE_VALUE = 100, MILL_VALUE = 10;
constexpr int MAX_FIELDS = 24, MAX_MOVES = 24 * 24;

struct Move { int src, dst, take; };  // src = -1 placement, take = -1 no mill
bool operator<(const Move &a, const Move &b) {
    return a.src != b.src ? a.src < b.src : a.dst != b.dst ? a.dst < b.dst : a.take < b.take;
}
struct State { int owner[MAX_FIELDS]; int hand[2]; int side; };

int F = 0;
vector<string> names;
map<string, int> fieldIndex;
vector<vector<int>> adj;
vector<array<int, 3>> mills;
vector<vector<int>> millsOf;

bool aligned(int a, int b, int c) {
    const string &na = names[a], &nb = names[b], &nc = names[c];
    return (na[0] == nb[0] && nb[0] == nc[0]) || (na.substr(1) == nb.substr(1) && nb.substr(1) == nc.substr(1));
}

void buildMills() {
    set<array<int, 3>> found;
    for (int b = 0; b < F; ++b)
        for (int a : adj[b])
            for (int c : adj[b])
                if (a < c && aligned(a, b, c)) found.insert({a, b, c});
    mills.assign(found.begin(), found.end());
    millsOf.assign(F, {});
    for (int k = 0; k < (int)mills.size(); ++k)
        for (int i : mills[k]) millsOf[i].push_back(k);
}

bool inMill(const int *owner, int i) {
    int p = owner[i];
    for (int k : millsOf[i]) {
        const auto &m = mills[k];
        if (owner[m[0]] == p && owner[m[1]] == p && owner[m[2]] == p) return true;
    }
    return false;
}

vector<int> takeTargets(const int *owner, int enemy) {
    vector<int> stones, free;
    for (int i = 0; i < F; ++i) if (owner[i] == enemy) stones.push_back(i);
    for (int i : stones) if (!inMill(owner, i)) free.push_back(i);
    return free.empty() ? stones : free;
}

int genMoves(const State &s, Move *out) {
    int p = s.side, n = 0;
    vector<pair<int, int>> steps;
    if (s.hand[p] > 0) {
        for (int d = 0; d < F; ++d) if (s.owner[d] == EMPTY) steps.push_back({-1, d});
    } else {
        vector<int> mine;
        for (int i = 0; i < F; ++i) if (s.owner[i] == p) mine.push_back(i);
        bool flying = mine.size() == 3;
        for (int src : mine) {
            if (flying) { for (int d = 0; d < F; ++d) if (s.owner[d] == EMPTY) steps.push_back({src, d}); }
            else for (int d : adj[src]) if (s.owner[d] == EMPTY) steps.push_back({src, d});
        }
    }
    int after[MAX_FIELDS];
    for (auto &st : steps) {
        copy(s.owner, s.owner + F, after);
        if (st.first >= 0) after[st.first] = EMPTY;
        after[st.second] = p;
        if (inMill(after, st.second)) {
            for (int t : takeTargets(after, 1 - p)) if (n < MAX_MOVES) out[n++] = {st.first, st.second, t};
        } else if (n < MAX_MOVES) out[n++] = {st.first, st.second, -1};
    }
    return n;
}

State applyMove(const State &s, const Move &m) {
    State t = s;
    if (m.src >= 0) t.owner[m.src] = EMPTY;
    else --t.hand[s.side];
    t.owner[m.dst] = s.side;
    if (m.take >= 0) t.owner[m.take] = EMPTY;
    t.side = 1 - s.side;
    return t;
}

int material(const State &s, int p) {
    int n = s.hand[p];
    for (int i = 0; i < F; ++i) n += s.owner[i] == p;
    return n;
}

int sideScore(const State &s, int p) {
    int millCount = 0, mobility = 0;
    for (const auto &m : mills) millCount += s.owner[m[0]] == p && s.owner[m[1]] == p && s.owner[m[2]] == p;
    for (int i = 0; i < F; ++i)
        if (s.owner[i] == p) for (int d : adj[i]) mobility += s.owner[d] == EMPTY;
    return STONE_VALUE * material(s, p) + MILL_VALUE * millCount + mobility;
}

inline int evaluate(const State &s) { return sideScore(s, s.side) - sideScore(s, 1 - s.side); }

void orderMoves(Move *mv, int n) {  // stable, captures first
    for (int i = 1; i < n; ++i) {
        Move x = mv[i];
        int j = i - 1;
        while (j >= 0 && (mv[j].take < 0 && x.take >= 0)) { mv[j + 1] = mv[j]; --j; }
        mv[j + 1] = x;
    }
}

// ---------------------------------------------------------------- ENGINE
constexpr int WIN = 1000000;
constexpr int INF = 1 << 30;
constexpr int MAX_DEPTH = 64;
long long NODE_LIMIT = TURN_NODES;
long long nodes = 0;
bool aborted = false;
int lastDepth = 0;
chrono::steady_clock::time_point t0;

int negamax(const State &s, int depth, int alpha, int beta, int ply) {
    if (++nodes > NODE_LIMIT) { aborted = true; return 0; }
    if ((nodes & 255) == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() > TURN_MS) {
        cerr << "TIMEGUARD" << endl; aborted = true; return 0;
    }
    if (material(s, s.side) < 3) return -(WIN - ply);
    Move mv[MAX_MOVES];
    int n = genMoves(s, mv);
    if (!n) return -(WIN - ply);
    if (depth == 0) return evaluate(s);
    orderMoves(mv, n);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        int v = -negamax(applyMove(s, mv[i]), depth - 1, -beta, -alpha, ply + 1);
        if (aborted) return 0;
        if (v > best) best = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    return best;
}

Move search(const State &root, vector<Move> mv) {
    int n = (int)mv.size();
    orderMoves(mv.data(), n);
    Move best = mv[0];
    lastDepth = 0; nodes = 0; aborted = false;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        int alpha = -INF, bestV = -INF, curIdx = 0;
        for (int i = 0; i < n; ++i) {
            int v = -negamax(applyMove(root, mv[i]), depth - 1, -INF, -alpha, 1);
            if (aborted) break;
            if (v > bestV) { bestV = v; curIdx = i; }
            if (v > alpha) alpha = v;
        }
        if (aborted) break;
        best = mv[curIdx];
        lastDepth = depth;
        for (int i = curIdx; i > 0; --i) mv[i] = mv[i - 1];  // PV first
        mv[0] = best;
        if (bestV >= WIN - MAX_DEPTH || bestV <= -(WIN - MAX_DEPTH)) break;
    }
    return best;
}

// ---------------------------------------------------------------- I/O
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

vector<string> split(const string &s, char sep) {
    vector<string> out;
    string cur;
    for (char c : s) { if (c == sep) { out.push_back(trim(cur)); cur.clear(); } else cur += c; }
    out.push_back(trim(cur));
    return out;
}

bool parseCommand(const string &text, Move &m) {
    vector<string> parts = split(text, ';');
    if (parts.size() < 2) return false;
    vector<int> f;
    for (size_t i = 1; i < parts.size(); ++i) {
        auto it = fieldIndex.find(parts[i]);
        if (it == fieldIndex.end()) return false;
        f.push_back(it->second);
    }
    const string &kind = parts[0];
    if (kind == "PLACE" && f.size() == 1) { m = {-1, f[0], -1}; return true; }
    if (kind == "PLACE&TAKE" && f.size() == 2) { m = {-1, f[0], f[1]}; return true; }
    if (kind == "MOVE" && f.size() == 2) { m = {f[0], f[1], -1}; return true; }
    if (kind == "MOVE&TAKE" && f.size() == 3) { m = {f[0], f[1], f[2]}; return true; }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long cliNodes = -1;
    if (const char *e = getenv("CG_NODES")) cliNodes = atoll(e);
    string line;
    if (!getline(cin, line)) return 0;
    int me = atoi(line.c_str());
    if (!getline(cin, line)) return 0;
    int count = atoi(line.c_str());
    map<string, vector<string>> adjacency;
    for (int i = 0; i < count; ++i) {
        if (!getline(cin, line)) return 0;
        size_t colon = line.find(':');
        string name = trim(line.substr(0, colon));
        names.push_back(name);
        if (colon != string::npos)
            for (const string &x : split(line.substr(colon + 1), ';')) if (!x.empty()) adjacency[name].push_back(x);
    }
    F = min((int)names.size(), MAX_FIELDS);
    names.resize(F);
    for (int i = 0; i < F; ++i) fieldIndex[names[i]] = i;
    adj.assign(F, {});
    for (int i = 0; i < F; ++i) {
        for (const string &x : adjacency[names[i]]) if (fieldIndex.count(x)) adj[i].push_back(fieldIndex[x]);
        sort(adj[i].begin(), adj[i].end());
    }
    buildMills();
    int placed[2] = {0, 0};
    bool first = true;
    while (getline(cin, line)) {
        t0 = chrono::steady_clock::now();
        if (trim(line).rfind("PLACE", 0) == 0) ++placed[1 - me];
        State s;
        for (int i = 0; i < F; ++i) s.owner[i] = EMPTY;
        if (!getline(cin, line)) return 0;
        for (const string &item : split(line, ';')) {
            size_t colon = item.find(':');
            if (colon == string::npos) continue;
            string name = trim(item.substr(0, colon)), state = trim(item.substr(colon + 1));
            if (fieldIndex.count(name) && (state == "0" || state == "1")) s.owner[fieldIndex[name]] = state[0] - '0';
        }
        if (!getline(cin, line)) return 0;
        int n = atoi(line.c_str());
        vector<string> commands;
        vector<Move> legal;
        map<Move, string> texts;
        for (int i = 0; i < n; ++i) {
            if (!getline(cin, line)) return 0;
            string c = trim(line);
            commands.push_back(c);
            Move m;
            if (parseCommand(c, m) && !texts.count(m)) { legal.push_back(m); texts[m] = c; }
        }
        if (legal.empty()) { cout << (commands.empty() ? "PLACE;A1" : commands[0]) << endl; continue; }
        s.hand[0] = STONES - placed[0];
        s.hand[1] = STONES - placed[1];
        s.side = me;
        NODE_LIMIT = cliNodes > 0 ? cliNodes : (first ? FIRST_TURN_NODES : TURN_NODES);
        first = false;
        Move m = search(s, legal);
        if (m.src < 0) ++placed[me];
        cerr << "d=" << lastDepth << " n=" << nodes << " ms="
             << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() << endl;
        cout << texts[m] << endl;
    }
}
