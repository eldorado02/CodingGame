// Chess (Chess960) — C++ (submitted), v1 one-ply with hanging pieces. Port of solutions/python/bot.py.
// Each legal UCI move (from the `moves` input) is played on the FEN board
// (castling = king onto its own rook, en passant, promotion) and scored:
// material + centrality / pawn advance - worst loss the opponent can inflict
// next (attacked piece, minus the cheapest attacker when defended) + check.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int CHECK_BONUS = 30;
constexpr int KING_ATTACKER = 10000;  // a king only takes an undefended piece
constexpr int CENTER_WEIGHT = 4;      // per step closer to the centre for knights and bishops
constexpr int PAWN_ADVANCE = 3;       // per rank advanced
constexpr int NO_ATTACKER = 1 << 30;
const string INPUTS = "fen moves";
constexpr int KNIGHT[8][2] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
constexpr int KING[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
constexpr int DIAGONALS[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
constexpr int LINES[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

using Board = array<array<char, 8>, 8>;  // [file][rank], '.' empty

int value(char piece) {
    switch (tolower(piece)) {
        case 'p': return 100;
        case 'n': return 320;
        case 'b': return 330;
        case 'r': return 500;
        case 'q': return 900;
        default: return 0;
    }
}

bool isWhite(char piece) { return isupper((unsigned char)piece); }
bool inside(int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; }

Board parseBoard(const string &field) {
    Board b;
    for (auto &file : b) file.fill('.');
    int rank = 7, file = 0;
    for (char ch : field) {
        if (ch == '/') { --rank; file = 0; }
        else if (isdigit((unsigned char)ch)) file += ch - '0';
        else {
            if (inside(file, rank)) b[file][rank] = ch;
            ++file;
        }
    }
    return b;
}

Board play(Board b, const string &move, bool white) {
    int sf = move[0] - 'a', sr = move[1] - '1', df = move[2] - 'a', dr = move[3] - '1';
    if (!inside(sf, sr) || !inside(df, dr) || b[sf][sr] == '.') return b;
    char piece = b[sf][sr], target = b[df][dr];
    b[sf][sr] = '.';
    if (tolower(piece) == 'k' && target != '.' && isWhite(target) == white && tolower(target) == 'r') {
        b[df][dr] = '.';  // castling: king onto its own rook
        bool kingSide = df > sf;
        b[kingSide ? 6 : 2][dr] = piece;
        b[kingSide ? 5 : 3][dr] = target;
        return b;
    }
    if (tolower(piece) == 'p' && sf != df && target == '.') b[df][sr] = '.';  // en passant
    if (move.size() > 4) piece = white ? toupper((unsigned char)move[4]) : tolower((unsigned char)move[4]);
    b[df][dr] = piece;
    return b;
}

// Cheapest attacker of `white` on (f, r) and whether there is any.
int cheapestAttacker(const Board &b, int f, int r, bool white) {
    int best = NO_ATTACKER;
    int pawnRank = white ? r - 1 : r + 1;
    for (int df : {-1, 1}) {
        int pf = f + df;
        if (inside(pf, pawnRank) && b[pf][pawnRank] != '.' && isWhite(b[pf][pawnRank]) == white && tolower(b[pf][pawnRank]) == 'p')
            best = min(best, value('p'));
    }
    auto scan = [&](const int (*steps)[2], int count, const char *kinds, bool slide) {
        for (int i = 0; i < count; ++i) {
            int nf = f + steps[i][0], nr = r + steps[i][1];
            while (inside(nf, nr)) {
                char p = b[nf][nr];
                if (p != '.') {
                    char k = tolower(p);
                    if (isWhite(p) == white && (k == kinds[0] || k == kinds[1])) best = min(best, k == 'k' ? KING_ATTACKER : value(p));
                    break;
                }
                if (!slide) break;
                nf += steps[i][0];
                nr += steps[i][1];
            }
        }
    };
    scan(KNIGHT, 8, "nn", false);
    scan(KING, 8, "kk", false);
    scan(DIAGONALS, 4, "bq", true);
    scan(LINES, 4, "rq", true);
    return best;
}

int evaluate(const Board &b, bool white) {
    int score = 0, worstLoss = 0;
    for (int f = 0; f < 8; ++f)
        for (int r = 0; r < 8; ++r) {
            char p = b[f][r];
            if (p == '.') continue;
            bool mine = isWhite(p) == white;
            char k = tolower(p);
            int v = value(p);
            if (k == 'n' || k == 'b') v += CENTER_WEIGHT * (6 - (abs(2 * f - 7) + abs(2 * r - 7)) / 2);
            else if (k == 'p') v += PAWN_ADVANCE * (isWhite(p) ? r : 7 - r);
            score += mine ? v : -v;
            if (mine && k != 'k') {
                int hit = cheapestAttacker(b, f, r, !white);
                if (hit != NO_ATTACKER) {
                    bool defended = cheapestAttacker(b, f, r, white) != NO_ATTACKER;
                    worstLoss = max(worstLoss, defended ? value(p) - hit : value(p));
                }
            }
            if (!mine && k == 'k' && cheapestAttacker(b, f, r, white) != NO_ATTACKER) score += CHECK_BONUS;
        }
    return score - worstLoss;
}

string choose(const string &fen, const vector<string> &moves) {
    istringstream fields(fen);
    string boardField, color = "w";
    fields >> boardField >> color;
    Board board = parseBoard(boardField);
    bool white = color == "w";
    int best = -1, bestScore = 0;
    for (int i = 0; i < (int)moves.size(); ++i) {
        if (moves[i].size() < 4) continue;
        int score = evaluate(play(board, moves[i], white), white);
        if (best < 0 || score > bestScore) { best = i; bestScore = score; }
    }
    if (best >= 0) return moves[best];
    return moves.empty() ? "random" : moves[0];
}

int main() {
    ios::sync_with_stdio(false);
    string line;
    if (!getline(cin, line) || line.find_first_not_of(" \r\n") == string::npos) return 0;
    int constants = stoi(line);
    for (int i = 0; i < constants; ++i) getline(cin, line);
    cout << INPUTS << endl;
    while (getline(cin, line)) {
        string fen = line;
        fen.erase(fen.find_last_not_of(" \r\n") + 1);
        if (fen.empty()) return 0;
        if (!getline(cin, line)) return 0;
        vector<string> moves(max(0, stoi(line)));
        for (string &m : moves) {
            getline(cin, m);
            m.erase(m.find_last_not_of(" \r\n") + 1);
            m.erase(0, m.find_first_not_of(" \t"));
        }
        cout << choose(fen, moves) << endl;
    }
}
