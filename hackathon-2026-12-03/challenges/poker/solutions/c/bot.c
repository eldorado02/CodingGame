/* Poker (Texas hold'em) — C port of cpp/bot.cpp (same equity, same decisions).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *RANKS = "23456789TJQKA";
static const char *SUITS = "HDSC";
enum { HIGH_CARD, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL_HOUSE, QUADS, STRAIGHT_FLUSH };
enum { ALL_IN_PERCENT = 80, RAISE_PERCENT = 62, PUSH_PERCENT = 50, SHORT_STACK_BLINDS = 10 };
enum { BOARD_SIZE = 5, DECK = 52, MAX_CARDS = 16, MAX_PLAYERS = 8, MAX_POSSIBLE = 32, WORD = 64, LINE_SIZE = 512 };
static const uint64_t SEED_SALT = 2654435761ULL;
static const long long BLIND_CAP = 1000000000LL; /* blinds keep doubling: stay in 64 bits */
static int simulations = 400;

static uint32_t rng_state;
static void rng_seed(uint64_t seed) { rng_state = (uint32_t)seed ? (uint32_t)seed : 1u; }
static uint32_t rng_next(void) { /* xorshift32: identical sequence in every language */
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rng_state = x;
}

static int parse(const char *text, int *out) { /* card index = rank * 4 + suit */
    int n = 0;
    const char *p = text;
    while (*p && n < MAX_CARDS) {
        const char *end = strchr(p, '_');
        size_t len = end ? (size_t)(end - p) : strlen(p);
        if (len == 2 && strchr(RANKS, p[0]) && strchr(SUITS, p[1]) && p[0] && p[1])
            out[n++] = (int)(strchr(RANKS, p[0]) - RANKS) * 4 + (int)(strchr(SUITS, p[1]) - SUITS);
        if (!end) break;
        p = end + 1;
    }
    return n;
}

static int straight_top(const bool *present) { /* highest straight top (wheel = 3), or -1 */
    for (int top = 12; top > 3; --top) {
        bool run = true;
        for (int k = 0; k < 5; ++k) run = run && present[top - k];
        if (run) return top;
    }
    return present[12] && present[0] && present[1] && present[2] && present[3] ? 3 : -1;
}

static long long encode(int category, const int *kickers, int n) {
    long long value = category;
    for (int k = 0; k < 5; ++k) value = value * 16 + (k < n ? kickers[k] : 0);
    return value;
}

/* Up to n ranks (descending) different from skip1 / skip2. */
static int others(const int *ranks, int count, int skip1, int skip2, int n, int *out) {
    int m = 0;
    for (int i = 0; i < count && m < n; ++i)
        if (ranks[i] != skip1 && ranks[i] != skip2) out[m++] = ranks[i];
    return m;
}

static long long evaluate(const int *cards, int n) { /* comparable value of the best 5-card hand */
    int counts[13] = {0}, suit_count[4] = {0};
    bool by_suit[4][13] = {{false}};
    for (int i = 0; i < n; ++i) {
        ++counts[cards[i] / 4];
        if (!by_suit[cards[i] % 4][cards[i] / 4]) { by_suit[cards[i] % 4][cards[i] / 4] = true; ++suit_count[cards[i] % 4]; }
    }
    for (int s = 0; s < 4; ++s)
        if (suit_count[s] >= 5) {
            int top = straight_top(by_suit[s]);
            if (top >= 0) return encode(STRAIGHT_FLUSH, &top, 1);
        }
    int ranks[13], quads[4], trips[4], pairs[7], nr = 0, nq = 0, nt = 0, np = 0, k[5];
    for (int r = 12; r >= 0; --r) {
        if (!counts[r]) continue;
        ranks[nr++] = r;
        if (counts[r] == 4) quads[nq++] = r;
        if (counts[r] == 3) trips[nt++] = r;
        if (counts[r] == 2) pairs[np++] = r;
    }
    if (nq) {
        k[0] = quads[0];
        return encode(QUADS, k, 1 + others(ranks, nr, quads[0], -1, 1, k + 1));
    }
    if (nt && (nt > 1 || np)) {
        int second = -1;
        for (int i = 1; i < nt; ++i) second = trips[i] > second ? trips[i] : second;
        for (int i = 0; i < np; ++i) second = pairs[i] > second ? pairs[i] : second;
        k[0] = trips[0];
        k[1] = second;
        return encode(FULL_HOUSE, k, 2);
    }
    for (int s = 0; s < 4; ++s)
        if (suit_count[s] >= 5) {
            int m = 0;
            for (int r = 12; r >= 0 && m < 5; --r)
                if (by_suit[s][r]) k[m++] = r;
            return encode(FLUSH, k, m);
        }
    bool present[13];
    for (int r = 0; r < 13; ++r) present[r] = counts[r] > 0;
    int top = straight_top(present);
    if (top >= 0) return encode(STRAIGHT, &top, 1);
    if (nt) {
        k[0] = trips[0];
        return encode(TRIPS, k, 1 + others(ranks, nr, trips[0], -1, 2, k + 1));
    }
    if (np >= 2) {
        k[0] = pairs[0];
        k[1] = pairs[1];
        return encode(TWO_PAIR, k, 2 + others(ranks, nr, pairs[0], pairs[1], 1, k + 2));
    }
    if (np) {
        k[0] = pairs[0];
        return encode(PAIR, k, 1 + others(ranks, nr, pairs[0], -1, 3, k + 1));
    }
    return encode(HIGH_CARD, k, others(ranks, nr, -1, -1, 5, k));
}

/* Twice the wins + ties; *total receives twice the simulations. */
static long long equity(const int *hole, const int *board, int nb, int opponents, uint64_t seed, long long *total) {
    int deck[DECK], n = 0, hand[MAX_CARDS];
    rng_seed(seed);
    for (int c = 0; c < DECK; ++c) {
        bool known = c == hole[0] || c == hole[1];
        for (int i = 0; i < nb; ++i) known |= c == board[i];
        if (!known) deck[n++] = c;
    }
    int missing = BOARD_SIZE - nb, need = missing + 2 * opponents;
    long long score = 0;
    for (int sim = 0; sim < simulations; ++sim) {
        for (int k = 0; k < need; ++k) {
            int j = k + (int)(rng_next() % (uint32_t)(n - k));
            int t = deck[k]; deck[k] = deck[j]; deck[j] = t;
        }
        int full[BOARD_SIZE];
        for (int i = 0; i < nb; ++i) full[i] = board[i];
        for (int i = 0; i < missing; ++i) full[nb + i] = deck[i];
        hand[0] = hole[0];
        hand[1] = hole[1];
        memcpy(hand + 2, full, sizeof full);
        long long ours = evaluate(hand, 7), best = 0;
        for (int o = 0; o < opponents; ++o) {
            hand[0] = deck[missing + 2 * o];
            hand[1] = deck[missing + 2 * o + 1];
            long long v = evaluate(hand, 7);
            if (v > best) best = v;
        }
        score += ours > best ? 2 : ours == best ? 1 : 0;
    }
    *total = 2LL * simulations;
    return score;
}

static long long big_blind(long long start, long long multiplier, long long per_level, long long hand) {
    long long blind = start;
    for (long long i = 0; i < (hand - 1) / (per_level > 1 ? per_level : 1); ++i)
        blind = blind * multiplier < BLIND_CAP ? blind * multiplier : BLIND_CAP;
    return blind;
}

static const char *find_action(char possible[][WORD], int n, const char *head) {
    size_t len = strlen(head);
    for (int i = 0; i < n; ++i)
        if (!strncmp(possible[i], head, len) && (possible[i][len] == '_' || possible[i][len] == '\0')) return possible[i];
    return NULL;
}

static void spaced(const char *action, char *out) {
    snprintf(out, WORD, "%s", action);
    for (char *p = out; *p; ++p)
        if (*p == '_') *p = ' ';
}

static void decide(long long score, long long total, char possible[][WORD], int n, long long to_call, long long pot, long long stack,
                   long long blind, char *out) {
    bool push = stack <= SHORT_STACK_BLINDS * blind && score * 100 >= PUSH_PERCENT * total;
    const char *bet = find_action(possible, n, "BET");
    if (find_action(possible, n, "ALL-IN") && (score * 100 >= ALL_IN_PERCENT * total || push)) { strcpy(out, "ALL-IN"); return; }
    if (score * 100 >= RAISE_PERCENT * total && bet) { spaced(bet, out); return; }
    if (find_action(possible, n, "CHECK")) { strcpy(out, "CHECK"); return; }
    if (find_action(possible, n, "CALL") && score * (pot + to_call) > total * to_call) { strcpy(out, "CALL"); return; }
    if (find_action(possible, n, "FOLD")) { strcpy(out, "FOLD"); return; }
    if (n) spaced(possible[0], out);
    else strcpy(out, "FOLD");
}

int main(void) {
    const char *env = getenv("CG_NODES");
    if (env) simulations = atoi(env);
    long long init[8], round, hand;
    for (int i = 0; i < 8; ++i)
        if (scanf("%lld", &init[i]) != 1) return 0;
    int players = (int)init[6], me = (int)init[7];
    if (players > MAX_PLAYERS) players = MAX_PLAYERS;
    static char possible[MAX_POSSIBLE][WORD];
    char board_text[WORD], hole_text[WORD], line[LINE_SIZE], out[WORD];
    while (scanf("%lld %lld", &round, &hand) == 2) {
        long long stack[MAX_PLAYERS], chips[MAX_PLAYERS];
        for (int k = 0; k < players; ++k)
            if (scanf("%lld %lld", &stack[k], &chips[k]) != 2) return 0;
        if (scanf("%63s %63s", board_text, hole_text) != 2) return 0;
        for (int block = 0; block < 2; ++block) { /* past actions, then showdowns */
            int count;
            if (scanf("%d", &count) != 1 || !fgets(line, sizeof line, stdin)) return 0;
            for (int i = 0; i < count; ++i)
                if (!fgets(line, sizeof line, stdin)) return 0;
        }
        int count;
        if (scanf("%d", &count) != 1) return 0;
        if (count > MAX_POSSIBLE) count = MAX_POSSIBLE;
        for (int i = 0; i < count; ++i)
            if (scanf("%63s", possible[i]) != 1) return 0;
        long long top = 0, pot = 0;
        int opponents = 0;
        for (int k = 0; k < players; ++k) {
            top = chips[k] > top ? chips[k] : top;
            pot += chips[k];
            if (k != me && stack[k] + chips[k] > 0) ++opponents;
        }
        int hole[MAX_CARDS], board[MAX_CARDS];
        int nh = parse(hole_text, hole), nb = parse(board_text, board);
        long long score = 0, total = 1;
        if (nh == 2) score = equity(hole, board, nb, opponents > 1 ? opponents : 1, (uint64_t)round * SEED_SALT + (uint64_t)hand, &total);
        decide(score, total, possible, count < 0 ? 0 : count, top - chips[me], pot, stack[me],
               big_blind(init[1], init[3], init[2], hand), out);
        printf("%s\n", out);
        fflush(stdout);
    }
    return 0;
}
