// Yinsh — C++ (submitted), v1 one-ply greedy on the legal actions. Port of solutions/python/bot.py.
// Input rows = columns a..k (numbers increasing); lines along (0,1), (1,0),
// (1,1). Each legal action is simulated (move: marker left behind, markers
// in between flipped; "xA-BxC": row A..B and ring C removed) and scored:
// RING_VALUE * rings removed + 5-cell windows of one colour (+m^2 / -t^2)
// + marker difference + ring mobility difference. STEAL is never played.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int COLUMNS = 11, NUMBERS = 12;  // numbers 1..11
constexpr int COLUMN_START[COLUMNS] = {2, 1, 1, 1, 1, 2, 2, 3, 4, 5, 7};
constexpr int COLUMN_SIZE[COLUMNS] = {4, 7, 8, 9, 10, 9, 10, 9, 8, 7, 4};
constexpr char EMPTY = '.', MY_RING = 'R', MY_MARKER = 'S', THEIR_RING = 'r', THEIR_MARKER = 's';
constexpr int AXES[3][2] = {{0, 1}, {1, 0}, {1, 1}};
constexpr int DIRECTIONS[6][2] = {{0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1}};
constexpr int ROW = 5;  // markers in a winning row
constexpr int RING_VALUE = 1000;
constexpr int MAX_CELL_TEXT = 3;  // "k11"
const string STEAL = "STEAL";

struct Cell { int col, num; };
using Board = char[COLUMNS][NUMBERS];

bool valid(int col, int num) {
    return col >= 0 && col < COLUMNS && num >= COLUMN_START[col] && num < COLUMN_START[col] + COLUMN_SIZE[col];
}

vector<vector<Cell>> buildWindows() {  // every line of ROW consecutive valid cells
    vector<vector<Cell>> out;
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num)
            for (const auto &axis : AXES) {
                vector<Cell> cells;
                bool ok = true;
                for (int k = 0; k < ROW; ++k) {
                    Cell c{col + k * axis[0], num + k * axis[1]};
                    ok = ok && valid(c.col, c.num);
                    cells.push_back(c);
                }
                if (ok) out.push_back(cells);
            }
    return out;
}
const vector<vector<Cell>> WINDOWS = buildWindows();

// "f10" -> (5, 10); nothing when it is not a board cell.
optional<Cell> parseCell(const string &text) {
    if (text.size() < 2 || text.size() > (size_t)MAX_CELL_TEXT) return nullopt;
    for (size_t i = 1; i < text.size(); ++i)
        if (!isdigit((unsigned char)text[i])) return nullopt;
    Cell c{tolower((unsigned char)text[0]) - 'a', atoi(text.c_str() + 1)};
    if (!valid(c.col, c.num)) return nullopt;
    return c;
}

int sign(int v) { return (v > 0) - (v < 0); }

// Cells from a to b inclusive when they share an axis, else empty.
vector<Cell> segment(Cell a, Cell b) {
    int dc = b.col - a.col, dn = b.num - a.num;
    if (!(dc == 0 || dn == 0 || dc == dn)) return {};
    int steps = max(abs(dc), abs(dn));
    vector<Cell> line;
    for (int k = 0; k <= steps; ++k) line.push_back({a.col + k * sign(dc), a.num + k * sign(dn)});
    return line;
}

char flipped(char v) { return v == MY_MARKER ? THEIR_MARKER : v == THEIR_MARKER ? MY_MARKER : v; }

void applyMove(Board b, Cell src, Cell dst) {
    vector<Cell> line = segment(src, dst);
    b[dst.col][dst.num] = b[src.col][src.num];
    b[src.col][src.num] = MY_MARKER;
    for (int k = 1; k + 1 < (int)line.size(); ++k) b[line[k].col][line[k].num] = flipped(b[line[k].col][line[k].num]);
}

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

// "xA-BxC": clears the markers A..B and the ring C; returns 1 if a ring went.
int applyRemoval(Board b, const string &text) {
    vector<string> parts = split(text.substr(1), 'x');
    vector<string> ends = split(parts[0], '-');
    if (ends.size() == 2) {
        optional<Cell> a = parseCell(ends[0]), z = parseCell(ends[1]);
        if (a && z)
            for (const Cell &c : segment(*a, *z))
                if (b[c.col][c.num] == MY_MARKER || b[c.col][c.num] == THEIR_MARKER) b[c.col][c.num] = EMPTY;
    }
    optional<Cell> ring = parts.size() > 1 ? parseCell(parts[1]) : nullopt;
    if (ring && (b[ring->col][ring->num] == MY_RING || b[ring->col][ring->num] == THEIR_RING)) {
        b[ring->col][ring->num] = EMPTY;
        return 1;
    }
    return 0;
}

// Plays our action on the board; returns the rings it removed.
int simulate(Board b, const string &action) {
    int removed = 0;
    for (const string &step : split(action, ';')) {
        size_t dash = step.find('-');
        if (!step.empty() && step[0] == 'x') {
            removed += applyRemoval(b, step);
        } else if (dash != string::npos) {
            optional<Cell> src = parseCell(step.substr(0, dash)), dst = parseCell(step.substr(dash + 1));
            if (src && dst) applyMove(b, *src, *dst);
        } else if (optional<Cell> c = parseCell(step); c && b[c->col][c->num] == EMPTY) {
            b[c->col][c->num] = MY_RING;
        }
    }
    return removed;
}

int mobility(const Board b, char ring) {  // destinations of every ring of this colour
    int total = 0;
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num) {
            if (!valid(col, num) || b[col][num] != ring) continue;
            for (const auto &d : DIRECTIONS) {
                int c = col + d[0], n = num + d[1];
                bool jumped = false;
                while (valid(c, n) && b[c][n] != MY_RING && b[c][n] != THEIR_RING) {
                    if (b[c][n] == EMPTY) {
                        ++total;
                        if (jumped) break;
                    } else {
                        jumped = true;
                    }
                    c += d[0];
                    n += d[1];
                }
            }
        }
    return total;
}

int evaluate(const Board b) {
    int score = 0;
    for (const auto &cells : WINDOWS) {
        int mine = 0, theirs = 0;
        for (const Cell &c : cells) {
            mine += b[c.col][c.num] == MY_MARKER;
            theirs += b[c.col][c.num] == THEIR_MARKER;
        }
        if (theirs == 0) score += mine * mine;
        else if (mine == 0) score -= theirs * theirs;
    }
    for (int col = 0; col < COLUMNS; ++col)
        for (int num = 1; num < NUMBERS; ++num)
            if (valid(col, num)) score += (b[col][num] == MY_MARKER) - (b[col][num] == THEIR_MARKER);
    return score + mobility(b, MY_RING) - mobility(b, THEIR_RING);
}

bool isSteal(const string &action) {
    if (action.size() != STEAL.size()) return false;
    for (size_t i = 0; i < action.size(); ++i)
        if (toupper((unsigned char)action[i]) != STEAL[i]) return false;
    return true;
}

string choose(const Board board, const vector<string> &actions) {
    int best = -1;
    long long bestScore = 0;
    for (int i = 0; i < (int)actions.size(); ++i) {
        if (isSteal(actions[i])) continue;
        Board after;
        for (int col = 0; col < COLUMNS; ++col)
            for (int num = 0; num < NUMBERS; ++num) after[col][num] = board[col][num];
        long long score = (long long)RING_VALUE * simulate(after, actions[i]) + evaluate(after);
        if (best < 0 || score > bestScore) { best = i; bestScore = score; }
    }
    if (best >= 0) return actions[best];
    return actions.empty() ? STEAL : actions[0];
}

int main() {
    ios::sync_with_stdio(false);
    string token;
    if (!(cin >> token)) return 0;
    cout << "yes" << endl;
    int count;
    while (cin >> count) {
        Board board;
        for (auto &column : board)
            for (char &v : column) v = EMPTY;
        for (int col = 0; col < count; ++col) {
            string row;
            cin >> row;
            if (col >= COLUMNS) continue;
            for (int k = 0; k < COLUMN_SIZE[col] && k < (int)row.size(); ++k) board[col][COLUMN_START[col] + k] = row[k];
        }
        int actionCount;
        cin >> actionCount;
        vector<string> actions(max(0, actionCount));
        for (string &a : actions) cin >> a;
        if (!cin) return 0;
        cout << choose(board, actions) << endl;
    }
}
