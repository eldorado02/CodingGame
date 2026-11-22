/* Othello 8x8 - determinist bitboard engine (fast fixed depth, no timeouts).
   Traduction C11 stricte de solutions/cpp/bot.cpp (parite de coups et departage).
   Bit layout: bit 0 = a1, bit 7 = h1, bit 8 = a2, bit 63 = h8. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t U64;

#define FA 0x0101010101010101ULL /* file a */
#define FH 0x8080808080808080ULL /* file h */

#define WIN 1000000000
#define INF 2000000000

static inline U64 shE (U64 x) { return (x << 1) & ~FA; }
static inline U64 shO (U64 x) { return (x >> 1) & ~FH; }
static inline U64 shN (U64 x) { return  x << 8;        }
static inline U64 shS (U64 x) { return  x >> 8;        }
static inline U64 shNE(U64 x) { return (x << 9) & ~FA; }
static inline U64 shNO(U64 x) { return (x << 7) & ~FH; }
static inline U64 shSE(U64 x) { return (x >> 7) & ~FA; }
static inline U64 shSO(U64 x) { return (x >> 9) & ~FH; }

/* legal moves: from each own stone, walk over opponent stones in direction d
   (6 dilation steps, max opponent run length on an 8x8 board). */
#define DILATE(SH) do {                 \
    U64 t = SH(P) & O;                  \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    t |= SH(t) & O;                     \
    m |= SH(t) & empty;                 \
} while (0)

static inline U64 moves(U64 P, U64 O) {
    U64 m = 0, empty = ~(P | O);
    DILATE(shE);  DILATE(shO);  DILATE(shN);  DILATE(shS);
    DILATE(shNE); DILATE(shNO); DILATE(shSE); DILATE(shSO);
    return m;
}
#undef DILATE

/* all discs flipped by putting a stone at mv (exactly one bit set) */
#define WALK(SH) do {                   \
    U64 s = SH(mv), t = 0;              \
    while (s & O) { t |= s; s = SH(s); }\
    if (s & P) f |= t;                  \
} while (0)

static inline U64 flips(U64 P, U64 O, U64 mv) {
    U64 f = 0;
    WALK(shE);  WALK(shO);  WALK(shN);  WALK(shS);
    WALK(shNE); WALK(shNO); WALK(shSE); WALK(shSO);
    return f;
}
#undef WALK

static inline void apply(U64 P, U64 O, U64 mv, U64 *nP, U64 *nO) {
    U64 f = flips(P, O, mv);
    *nP = P | mv | f;
    *nO = O & ~f;
}

static inline U64 neighbors(U64 x) {
    return shE(x) | shO(x) | shN(x) | shS(x)
         | shNE(x) | shNO(x) | shSE(x) | shSO(x);
}

/* identique a POS du C++, indexe par numero de bit */
static const int POS[64] = {
     120,-20, 20,  5,  5, 20,-20, 120,
     -20,-40, -5, -5, -5, -5,-40, -20,
      20, -5, 15,  3,  3, 15, -5,  20,
       5, -5,  3,  3,  3,  3, -5,   5,
       5, -5,  3,  3,  3,  3, -5,   5,
      20, -5, 15,  3,  3, 15, -5,  20,
     -20,-40, -5, -5, -5, -5,-40, -20,
     120,-20, 20,  5,  5, 20,-20, 120
};

static inline int evaluate(U64 P, U64 O) {
    int pos = 0;
    U64 x = P;
    while (x) { int b = __builtin_ctzll(x); pos += POS[b]; x &= x - 1; }
    x = O;
    while (x) { int b = __builtin_ctzll(x); pos -= POS[b]; x &= x - 1; }
    U64 empty = ~(P | O);
    U64 nb = neighbors(empty);
    int score = pos
              + 20 * (__builtin_popcountll(moves(P, O)) - __builtin_popcountll(moves(O, P)))
              + 8  * (__builtin_popcountll(O & nb) - __builtin_popcountll(P & nb));
    if (__builtin_popcountll(empty) <= 12)
        score += 8 * ((int)__builtin_popcountll(P) - (int)__builtin_popcountll(O));
    return score;
}

/* exact terminal score from material */
static inline int terminal(U64 P, U64 O) {
    int d = (int)__builtin_popcountll(P) - (int)__builtin_popcountll(O);
    return d > 0 ? WIN : (d < 0 ? -WIN : 0);
}

/* move ordering: POS[bit] descending, ties keep ascending bit index */
static inline int ordered_bits(U64 m, int *out) {
    int n = 0;
    while (m) {
        int b = __builtin_ctzll(m);
        out[n++] = b;
        m &= m - 1;
    }
    for (int i = 1; i < n; ++i) {
        int v = out[i], j = i - 1;
        while (j >= 0 && POS[out[j]] < POS[v]) { out[j + 1] = out[j]; --j; }
        out[j + 1] = v;
    }
    return n;
}

static int search(U64 P, U64 O, int depth, int alpha, int beta, int passed) {
    U64 m = moves(P, O);
    if (!m) {
        if (passed) return terminal(P, O);
        return -search(O, P, depth, -beta, -alpha, 1);
    }
    if (depth <= 0) return evaluate(P, O);
    int ord[32];
    int n = ordered_bits(m, ord);
    int best = -INF;
    for (int i = 0; i < n; ++i) {
        U64 nP, nO;
        apply(P, O, 1ULL << ord[i], &nP, &nO);
        int val = -search(nO, nP, depth - 1, -beta, -alpha, 0);
        if (val > best) best = val;
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }
    return best;
}

#ifndef OTHELLO_DEPTH
#define OTHELLO_DEPTH 6
#endif

static int choose(U64 P, U64 O) {
    U64 m = moves(P, O);
    if (!m) return -1;
    int ord[32];
    int n = ordered_bits(m, ord);
    int best = ord[0], best_val = -INF;
    for (int depth = 1; depth <= OTHELLO_DEPTH; ++depth) {
        int local_best = -INF, local_move = best, alpha = -INF;
        for (int i = 0; i < n; ++i) {
            U64 nP, nO;
            apply(P, O, 1ULL << ord[i], &nP, &nO);
            int val = -search(nO, nP, depth - 1, -INF, -alpha, 0);
            if (val > local_best) { local_best = val; local_move = ord[i]; }
            if (val > alpha) alpha = val;
        }
        best = local_move;
        best_val = local_best;
        if (best_val >= WIN / 2) break;
    }
    return best;
}

/* getline-like: strip the '\n' delimiter, keep any '\r', NULL on EOF */

static char *read_line(FILE *f) {
    static char *buf = NULL;
    static size_t cap = 0;
    size_t len = 0;
    int ch;
    if (!buf) {
        cap = 128;
        buf = (char *)malloc(cap);
        if (!buf) exit(1);
    }
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') break;
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = (char *)realloc(buf, ncap);
            if (!nb) exit(1);
            buf = nb;
            cap = ncap;
        }
        buf[len++] = (char)ch;
    }
    if (ch == EOF && len == 0) return NULL;
    buf[len] = '\0';
    return buf;
}

/* true if s is a single non-negative integer (with optional surrounding blanks) */
static int parse_int(const char *s, int *v) {
    size_t i = 0;
    while (s[i] == ' ') ++i;
    if (s[i] < '0' || s[i] > '9') return 0;
    long long x = 0;
    while (s[i] >= '0' && s[i] <= '9') {
        x = x * 10 + (s[i] - '0');
        if (x > 1000000) return 0;
        ++i;
    }
    while (s[i] == ' ') ++i;
    if (s[i] != '\0') return 0;
    *v = (int)x;
    return 1;
}

static int is_board_row(const char *s) {
    if (strlen(s) < 8) return 0;
    for (int c = 0; c < 8; ++c)
        if (s[c] != '.' && s[c] != '0' && s[c] != '1') return 0;
    return 1;
}

int main(void) {
    const char *line;
    int id = 0, size = 8;
    /* id / boardSize are only sent on the first three turns by the referee. */
    line = read_line(stdin);
    if (line) parse_int(line, &id);
    line = read_line(stdin);
    if (line) parse_int(line, &size);
    (void)size;
    for (;;) {
        U64 P = 0, O = 0;
        int rows = 0;
        /* Skip id/size lines (when present) and any other non-board line. */
        while (rows < 8) {
            line = read_line(stdin);
            if (!line) return 0;
            if (!is_board_row(line)) continue;
            for (int c = 0; c < 8; ++c) {
                U64 bit = 1ULL << (rows * 8 + c);
                if (line[c] == '0') P |= bit;
                else if (line[c] == '1') O |= bit;
            }
            ++rows;
        }
        if (id == 1) { U64 t = P; P = O; O = t; }
        /* Optional "EXPERT" extra line (only if WE output EXPERT). */
        int n = -1;
        while ((line = read_line(stdin)) != NULL) {
            if (parse_int(line, &n)) break;
        }
        if (n < 0) break;
        int *actions = (int *)malloc((size_t)(n > 0 ? n : 1) * sizeof(int));
        if (!actions) return 1;
        int na = 0;
        for (int i = 0; i < n; ++i) {
            line = read_line(stdin);
            if (!line) { free(actions); return 0; }
            size_t k = 0;
            while (line[k] == ' ') ++k;
            if (k + 1 >= strlen(line)) continue;
            actions[na++] = (line[k] - 'a') + (line[k + 1] - '1') * 8;
        }
        int mv = choose(P, O);
        int found = 0;
        for (int i = 0; i < na; ++i) if (actions[i] == mv) { found = 1; break; }
        if (!found && na > 0) mv = actions[0];
        free(actions);
        if (mv < 0) { printf("PASS\n"); fflush(stdout); continue; }
        printf("%c%c\n", (char)('a' + mv % 8), (char)('1' + mv / 8));
        fflush(stdout);
    }
    return 0;
}
