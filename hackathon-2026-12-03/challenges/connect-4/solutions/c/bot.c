/* Connect 4 (7x9): alpha-beta depth 6, window evaluation, STEAL. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define R 7
#define C 9
#define WIN 1000000
#define TSIZE 524288
#define CONST UINT64_C(0x9E3779B97F4A7C15)

static const int DR[4] = {0, 1, 1, 1};
static const int DC[4] = {1, 0, 1, -1};

typedef struct {
    unsigned char cell[R * C];
    unsigned char h[C];
} Board;

typedef struct {
    uint64_t key;
    int depth, val, flag, move, used;
} Entry;
static Entry tt[TSIZE];

static inline int at(const Board *b, int r, int c) { return b->cell[r * C + c]; }

static int wins_at(const Board *b, int r, int c, int p) {
    for (int d = 0; d < 4; ++d) {
        int cnt = 1;
        for (int s = 1; s < 4; ++s) {
            int nr = r + DR[d] * s, nc = c + DC[d] * s;
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || at(b, nr, nc) != p) break;
            ++cnt;
        }
        for (int s = 1; s < 4; ++s) {
            int nr = r - DR[d] * s, nc = c - DC[d] * s;
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || at(b, nr, nc) != p) break;
            ++cnt;
        }
        if (cnt >= 4) return 1;
    }
    return 0;
}

static int play(Board *b, int col, int p) {
    int r = R - 1 - b->h[col];
    b->cell[r * C + col] = (unsigned char)p;
    b->h[col]++;
    return wins_at(b, r, col, p);
}

static int evaluate(const Board *b, int p) {
    int opp = 3 - p, score = 0;
    for (int r = 0; r < R; ++r) for (int c = 0; c < C; ++c) {
        for (int d = 0; d < 4; ++d) {
            int ar = r - DR[d] * 3, ac = c - DC[d] * 3;
            if (ar < 0 || ac < 0 || ac >= C) continue;
            int mine = 0, theirs = 0, ok = 1;
            for (int s = 0; s < 4; ++s) {
                int rr = ar + DR[d] * s, cc = ac + DC[d] * s;
                if (rr >= R || cc >= C) { ok = 0; break; }
                int v = at(b, rr, cc);
                if (v == p) ++mine;
                else if (v == opp) ++theirs;
            }
            if (!ok) continue;
            if (mine && !theirs) score += 1 << (mine - 1);
            if (theirs && !mine) score -= 1 << (theirs - 1);
        }
    }
    score += 2 * (at(b, 0, 4) == p) - 2 * (at(b, 0, 4) == opp);
    return score;
}

static uint64_t key(const Board *b, int p) {
    uint64_t h = UINT64_C(1469598103934665603);
    for (int i = 0; i < R * C; ++i)
        h ^= (uint64_t)b->cell[i] + CONST + (h << 6) + (h >> 2);
    return h ^ (uint64_t)p;
}

static Entry *tt_get(uint64_t k) {
    size_t i = (size_t)k & (TSIZE - 1);
    for (int probe = 0; probe < 8; ++probe, i = (i + 1) & (TSIZE - 1)) {
        if (!tt[i].used) return &tt[i];
        if (tt[i].key == k) return &tt[i];
    }
    return &tt[i];
}

static int contains(const Board *b, int col) {
    return col >= 0 && col < C && b->h[col] < R;
}

static int search(Board *b, int p, int depth, int alpha, int beta, int ply) {
    if (depth <= 0) return evaluate(b, p);
    int best = -2 * WIN, best_move = -1;
    uint64_t k = key(b, p);
    Entry *e = tt_get(k);
    int tt_move = -1;
    if (e->used && e->key == k && e->depth >= depth) {
        if (e->flag == 0) return e->val;
        if (e->flag == 1 && e->val >= beta) return e->val;
        if (e->flag == 2 && e->val <= alpha) return e->val;
        tt_move = e->move;
    }
    int moves[C], n = 0;
    if (tt_move >= 0 && contains(b, tt_move)) moves[n++] = tt_move;
    int c = 4, k2 = 0;
    while (k2 < C) {
        if (contains(b, c)) {
            int dup = 0;
            for (int i = 0; i < n; ++i) if (moves[i] == c) { dup = 1; break; }
            if (!dup) moves[n++] = c;
        }
        ++k2;
        c = (c + (k2 % 2 ? k2 : -k2) + C) % C;
    }
    int full = 1;
    for (int i = 0; i < n; ++i) {
        int mv = moves[i];
        Board nb = *b;
        int win = play(&nb, mv, p);
        int val;
        if (win) val = WIN - ply;
        else {
            int draw = 1;
            for (int j = 0; j < C; ++j) if (nb.h[j] < R) { draw = 0; break; }
            val = draw ? 0 : -search(&nb, 3 - p, depth - 1, -beta, -alpha, ply + 1);
        }
        if (val > best) { best = val; best_move = mv; }
        if (best > alpha) alpha = best;
        if (alpha >= beta) { full = 0; break; }
    }
    e->key = k; e->depth = depth; e->val = best; e->move = best_move;
    e->flag = full ? 0 : (best >= beta ? 1 : 2); e->used = 1;
    return best;
}

static int choose(const Board *b, const int *valid, int nvalid, int p) {
    int best_col = nvalid ? valid[0] : 4, best_val = -2 * WIN;
    for (int depth = 1; depth <= 6; ++depth) {
        int alpha = -2 * WIN, local_best = -1, local_val = -2 * WIN;
        for (int i = 0; i < nvalid; ++i) {
            int c = valid[i];
            Board nb = *b;
            int win = play(&nb, c, p);
            int val;
            if (win) val = WIN;
            else if (depth == 1) val = evaluate(&nb, p);
            else val = -search(&nb, 3 - p, depth - 1, -2 * WIN, -alpha, 1);
            if (val > local_val) { local_val = val; local_best = c; }
            if (val > alpha) alpha = val;
        }
        best_col = local_best; best_val = local_val;
        if (best_val >= WIN / 2) break;
    }
    return best_col;
}

int main(void) {
    int my_id, opp_id;
    if (scanf("%d %d", &my_id, &opp_id) != 2) return 0;
    int me = my_id + 1;
    while (1) {
        int turn;
        if (scanf("%d", &turn) != 1) break;
        Board b;
        memset(&b, 0, sizeof(b));
        for (int r = 0; r < R; ++r) {
            char row[C + 2];
            if (scanf("%63s", row) != 1) return 0;
            for (int c = 0; c < C; ++c)
                b.cell[r * C + c] = row[c] == '.' ? 0 : (unsigned char)(row[c] - '0' + 1);
        }
        for (int c = 0; c < C; ++c) {
            int cnt = 0;
            for (int r = 0; r < R; ++r) if (b.cell[r * C + c]) ++cnt;
            b.h[c] = (unsigned char)cnt;
        }
        int n;
        if (scanf("%d", &n) != 1) break;
        int valid[16], steal = 0;
        for (int i = 0; i < n; ++i) {
            scanf("%d", &valid[i]);
            if (valid[i] == -2) steal = 1;
        }
        int prev;
        scanf("%d", &prev);
        if (steal) printf("STEAL\n");
        else printf("%d\n", choose(&b, valid, n, me));
        fflush(stdout);
    }
    return 0;
}
