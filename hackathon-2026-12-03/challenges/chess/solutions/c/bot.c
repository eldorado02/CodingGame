/* Chess (Chess960) — C port of cpp/bot.cpp (same play, same score, same tie-break).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { CHECK_BONUS = 30 };
enum { KING_ATTACKER = 10000 };  /* a king only takes an undefended piece */
enum { CENTER_WEIGHT = 4 };      /* per step closer to the centre for knights and bishops */
enum { PAWN_ADVANCE = 3 };       /* per rank advanced */
enum { NO_ATTACKER = 1 << 30, LINE_SIZE = 256, MAX_MOVES = 512, MOVE_SIZE = 16 };
static const char *INPUTS = "fen moves";
static const int KNIGHT[8][2] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
static const int KING[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
static const int DIAGONALS[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
static const int LINES[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

typedef char Board[8][8]; /* [file][rank], '.' empty */

static int value(char piece) {
    switch (tolower((unsigned char)piece)) {
        case 'p': return 100;
        case 'n': return 320;
        case 'b': return 330;
        case 'r': return 500;
        case 'q': return 900;
        default: return 0;
    }
}

static bool is_white(char piece) { return isupper((unsigned char)piece); }
static bool inside(int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; }
static int min_i(int a, int b) { return a < b ? a : b; }

static void parse_board(const char *field, Board b) {
    memset(b, '.', sizeof(Board));
    int rank = 7, file = 0;
    for (const char *p = field; *p && *p != ' '; ++p) {
        if (*p == '/') { --rank; file = 0; }
        else if (isdigit((unsigned char)*p)) file += *p - '0';
        else {
            if (inside(file, rank)) b[file][rank] = *p;
            ++file;
        }
    }
}

static void play(Board b, const char *move, bool white) {
    int sf = move[0] - 'a', sr = move[1] - '1', df = move[2] - 'a', dr = move[3] - '1';
    if (!inside(sf, sr) || !inside(df, dr) || b[sf][sr] == '.') return;
    char piece = b[sf][sr], target = b[df][dr];
    b[sf][sr] = '.';
    if (tolower((unsigned char)piece) == 'k' && target != '.' && is_white(target) == white && tolower((unsigned char)target) == 'r') {
        b[df][dr] = '.'; /* castling: king onto its own rook */
        bool king_side = df > sf;
        b[king_side ? 6 : 2][dr] = piece;
        b[king_side ? 5 : 3][dr] = target;
        return;
    }
    if (tolower((unsigned char)piece) == 'p' && sf != df && target == '.') b[df][sr] = '.'; /* en passant */
    if (strlen(move) > 4) piece = (char)(white ? toupper((unsigned char)move[4]) : tolower((unsigned char)move[4]));
    b[df][dr] = piece;
}

static int scan(Board b, int f, int r, bool white, const int (*steps)[2], int count, const char *kinds, bool slide) {
    int best = NO_ATTACKER;
    for (int i = 0; i < count; ++i) {
        int nf = f + steps[i][0], nr = r + steps[i][1];
        while (inside(nf, nr)) {
            char p = b[nf][nr];
            if (p != '.') {
                char k = (char)tolower((unsigned char)p);
                if (is_white(p) == white && (k == kinds[0] || k == kinds[1])) best = min_i(best, k == 'k' ? KING_ATTACKER : value(p));
                break;
            }
            if (!slide) break;
            nf += steps[i][0];
            nr += steps[i][1];
        }
    }
    return best;
}

/* Cheapest attacker of `white` on (f, r), NO_ATTACKER when none. */
static int cheapest_attacker(Board b, int f, int r, bool white) {
    int best = NO_ATTACKER, pawn_rank = white ? r - 1 : r + 1;
    for (int df = -1; df <= 1; df += 2) {
        int pf = f + df;
        if (inside(pf, pawn_rank) && b[pf][pawn_rank] != '.' && is_white(b[pf][pawn_rank]) == white &&
            tolower((unsigned char)b[pf][pawn_rank]) == 'p')
            best = min_i(best, value('p'));
    }
    best = min_i(best, scan(b, f, r, white, KNIGHT, 8, "nn", false));
    best = min_i(best, scan(b, f, r, white, KING, 8, "kk", false));
    best = min_i(best, scan(b, f, r, white, DIAGONALS, 4, "bq", true));
    best = min_i(best, scan(b, f, r, white, LINES, 4, "rq", true));
    return best;
}

static int evaluate(Board b, bool white) {
    int score = 0, worst_loss = 0;
    for (int f = 0; f < 8; ++f)
        for (int r = 0; r < 8; ++r) {
            char p = b[f][r];
            if (p == '.') continue;
            bool mine = is_white(p) == white;
            char k = (char)tolower((unsigned char)p);
            int v = value(p);
            if (k == 'n' || k == 'b') v += CENTER_WEIGHT * (6 - (abs(2 * f - 7) + abs(2 * r - 7)) / 2);
            else if (k == 'p') v += PAWN_ADVANCE * (is_white(p) ? r : 7 - r);
            score += mine ? v : -v;
            if (mine && k != 'k') {
                int hit = cheapest_attacker(b, f, r, !white);
                if (hit != NO_ATTACKER) {
                    bool defended = cheapest_attacker(b, f, r, white) != NO_ATTACKER;
                    int loss = defended ? value(p) - hit : value(p);
                    if (loss > worst_loss) worst_loss = loss;
                }
            }
            if (!mine && k == 'k' && cheapest_attacker(b, f, r, white) != NO_ATTACKER) score += CHECK_BONUS;
        }
    return score - worst_loss;
}

static void strip(char *s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) s[--n] = '\0';
    size_t lead = strspn(s, " \t");
    memmove(s, s + lead, strlen(s + lead) + 1);
}

int main(void) {
    static char moves[MAX_MOVES][MOVE_SIZE];
    char line[LINE_SIZE], fen[LINE_SIZE];
    if (!fgets(line, sizeof line, stdin)) return 0;
    int constants = atoi(line);
    for (int i = 0; i < constants; ++i)
        if (!fgets(line, sizeof line, stdin)) return 0;
    printf("%s\n", INPUTS);
    fflush(stdout);
    while (fgets(fen, sizeof fen, stdin)) {
        strip(fen);
        if (!fen[0] || !fgets(line, sizeof line, stdin)) return 0;
        int count = atoi(line);
        if (count < 0) count = 0;
        if (count > MAX_MOVES) count = MAX_MOVES;
        for (int i = 0; i < count; ++i) {
            if (!fgets(line, sizeof line, stdin)) line[0] = '\0';
            strip(line);
            snprintf(moves[i], MOVE_SIZE, "%.15s", line);
        }
        char *color = strchr(fen, ' ');
        bool white = !color || (color[1] == 'w' && (color[2] == ' ' || color[2] == '\0'));
        Board board;
        parse_board(fen, board);
        int best = -1, best_score = 0;
        for (int i = 0; i < count; ++i) {
            if (strlen(moves[i]) < 4) continue;
            Board after;
            memcpy(after, board, sizeof after);
            play(after, moves[i], white);
            int score = evaluate(after, white);
            if (best < 0 || score > best_score) { best = i; best_score = score; }
        }
        printf("%s\n", best >= 0 ? moves[best] : (count ? moves[0] : "random"));
        fflush(stdout);
    }
    return 0;
}
