/* 6 nimmt! — C port of cpp/bot.cpp (same cost model, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHASE_PLAY "CHOOSE_CARD_TO_PLAY"
enum { DECK_SIZE = 104 };
enum { HAND_SIZE = 10 };  /* a full hand means a new round: forget the seen cards */
enum { ROW_LIMIT = 5 };   /* the sixth card of a row takes the five others */
enum { ROWS = 4, MAX_ROW = 8, MAX_LIST = 128, LINE_SIZE = 4096 };
enum { DISQUALIFIED_SCORE = -999 };

typedef struct { int cards[MAX_ROW]; int size; } Row;

static int cows(int card) {
    if (card == 55) return 7;
    if (card % 11 == 0) return 5;
    if (card % 10 == 0) return 3;
    if (card % 5 == 0) return 2;
    return 1;
}

static int row_cows(const Row *row) {
    int total = 0;
    for (int i = 0; i < row->size; ++i) total += cows(row->cards[i]);
    return total;
}

static int last_of(const Row *row) { return row->cards[row->size - 1]; }

/* Index of the row the card goes on, or -1 when it is below every row. */
static int target_row(const Row *rows, int card) {
    int best = -1;
    for (int i = 0; i < ROWS; ++i)
        if (last_of(&rows[i]) < card && (best < 0 || last_of(&rows[i]) > last_of(&rows[best]))) best = i;
    return best;
}

static long long binomial(int n, int k) {
    long long value = 1;
    for (int i = 0; i < k; ++i) value = value * (n - i) / (i + 1);
    return value;
}

/* P(Binomial(draws, p) >= count), products written out for identical floats. */
static double at_least(int count, int draws, double p) {
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

static int gap_unseen(const bool *seen, int low, int high) {
    int n = 0;
    for (int c = low + 1; c < high; ++c) n += !seen[c];
    return n;
}

/* Expected cows of playing this card now; *gap receives the unseen gap. */
static double play_cost(const Row *rows, const bool *seen, int unseen, int others, int card, int *gap) {
    int r = target_row(rows, card);
    *gap = 0;
    if (r < 0) {
        int cheapest = row_cows(&rows[0]);
        for (int i = 1; i < ROWS; ++i)
            if (row_cows(&rows[i]) < cheapest) cheapest = row_cows(&rows[i]);
        return (double)cheapest;
    }
    *gap = gap_unseen(seen, last_of(&rows[r]), card);
    double p = unseen ? (double)*gap / unseen : 0.0;
    int slots = ROW_LIMIT - rows[r].size;
    return at_least(slots, others, p) * row_cows(&rows[r]);
}

static int compare_ints(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static int choose_card(const Row *rows, int *hand, int hand_size, const bool *seen, int others) {
    int unseen = 0;
    for (int c = 1; c <= DECK_SIZE; ++c) unseen += !seen[c];
    qsort(hand, hand_size, sizeof *hand, compare_ints);
    int best = hand[0], best_gap = 0;
    double best_cost = 0.0;
    for (int i = 0; i < hand_size; ++i) {
        int gap;
        double cost = play_cost(rows, seen, unseen, others, hand[i], &gap);
        bool better = i == 0 || cost < best_cost || (cost == best_cost && gap < best_gap);
        if (better) { best = hand[i]; best_cost = cost; best_gap = gap; }
    }
    return best;
}

static int choose_row(const Row *rows) {
    int best = 0;
    for (int i = 1; i < ROWS; ++i)
        if (row_cows(&rows[i]) < row_cows(&rows[best])) best = i;
    return best;
}

/* Reads one line of integers; returns their count, -1 at end of input. */
static int read_ints(int *out, int max) {
    char line[LINE_SIZE];
    if (!fgets(line, sizeof line, stdin)) return -1;
    int n = 0;
    char *p = line, *end;
    while (n < max) {
        long v = strtol(p, &end, 10);
        if (end == p) break;
        out[n++] = (int)v;
        p = end;
    }
    return n;
}

static void mark_seen(bool *seen, const int *cards, int n) {
    for (int i = 0; i < n; ++i)
        if (cards[i] >= 1 && cards[i] <= DECK_SIZE) seen[cards[i]] = true;
}

int main(void) {
    int header[MAX_LIST], last_played[MAX_LIST], scores[MAX_LIST], hand[MAX_LIST], ignored[MAX_LIST];
    bool seen[DECK_SIZE + 2] = {false};
    Row rows[ROWS];
    char phase[LINE_SIZE];
    if (read_ints(header, MAX_LIST) < 1) return 0;
    int player_count = header[0];
    /* The real referee sends the phase as the LAST line of a turn (the statement
     * lists it first): last cards, rows, scores, hand size, hand, phase. */
    for (;;) {
        int n_last = read_ints(last_played, MAX_LIST);
        if (n_last < 0) return 0;
        for (int i = 0; i < ROWS; ++i) {
            read_ints(ignored, MAX_LIST);
            rows[i].size = read_ints(rows[i].cards, MAX_ROW);
            if (rows[i].size <= 0) { rows[i].cards[0] = 0; rows[i].size = 1; }
        }
        int n_scores = read_ints(scores, MAX_LIST);
        read_ints(ignored, MAX_LIST);
        int hand_size = read_ints(hand, MAX_LIST);
        if (hand_size < 0) hand_size = 0;
        if (!fgets(phase, sizeof phase, stdin)) phase[0] = '\0';
        phase[strcspn(phase, " \t\r\n")] = '\0';
        bool play = strcmp(phase, PHASE_PLAY) == 0;
        if (play && hand_size == HAND_SIZE) memset(seen, 0, sizeof seen);
        mark_seen(seen, last_played, n_last > 0 ? n_last : 0);
        mark_seen(seen, hand, hand_size);
        for (int i = 0; i < ROWS; ++i) mark_seen(seen, rows[i].cards, rows[i].size);
        int active = 0;
        for (int i = 0; i < n_scores && i < player_count; ++i) active += scores[i] != DISQUALIFIED_SCORE;
        int others = active > 1 ? active - 1 : 0;
        if (play && hand_size > 0) printf("PLAY %d\n", choose_card(rows, hand, hand_size, seen, others));
        else printf("PICK %d\n", choose_row(rows));
        fflush(stdout);
    }
    return 0;
}
