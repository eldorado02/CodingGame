/* Olymbits — Hurdle Race : BFS court terme, on ne touche jamais une haie. */
#include <stdio.h>
#include <string.h>

#define MAX_POS 64

static const char *const NAMES[4] = {"LEFT", "DOWN", "RIGHT", "UP"};
static const int STEPS[4] = {1, 2, 3, 2};
static const int JUMP[4] = {0, 0, 0, 1};

/* Position atteinte, ou -1 si le coureur heurte une haie et reste étourdi. */
static int move_pos(const char *track, int len, int pos, int steps, int jump) {
    int last = len - 1;
    int np = pos;
    for (int k = 0; k < steps; ++k) {
        if (np < last) {
            ++np;
        }
        if (track[np] == '#' && !jump) {
            return -1;
        }
        if (np == last) {
            break;
        }
        jump = 0; /* UP n'ignore que la première case. */
    }
    return np;
}

static const char *best_action(const char *track, int len, int pos) {
    int last = len - 1;
    if (pos >= last) {
        return "LEFT";
    }
    int queue[MAX_POS], head = 0, tail = 0;
    int first[MAX_POS];
    int seen[MAX_POS] = {0};
    queue[tail++] = pos;
    seen[pos] = 1;
    first[pos] = -1;
    while (head < tail) {
        int p = queue[head++];
        for (int a = 0; a < 4; ++a) {
            int np = move_pos(track, len, p, STEPS[a], JUMP[a]);
            if (np < 0 || seen[np]) {
                continue;
            }
            seen[np] = 1;
            first[np] = p == pos ? a : first[p];
            if (np == last) {
                return NAMES[first[np]];
            }
            queue[tail++] = np;
        }
    }
    return "LEFT";
}

int main(void) {
    char line[256];
    int player = 0, games = 0;
    if (!fgets(line, sizeof(line), stdin)) {
        return 0;
    }
    sscanf(line, "%d", &player);
    if (!fgets(line, sizeof(line), stdin)) {
        return 0;
    }
    sscanf(line, "%d", &games);

    for (;;) {
        int alive = 1;
        for (int i = 0; i < 3 && alive; ++i) {
            alive = fgets(line, sizeof(line), stdin) != NULL;
        }
        if (!alive) {
            break;
        }
        char gpu[64];
        int reg[7] = {0};
        int count = 0;
        for (int g = 0; g < games; ++g) {
            if (!fgets(line, sizeof(line), stdin)) {
                return 0;
            }
            if (sscanf(line, "%63s %d %d %d %d %d %d %d", gpu,
                       &reg[0], &reg[1], &reg[2], &reg[3], &reg[4],
                       &reg[5], &reg[6]) >= 1) {
                count++;
            }
        }
        if (count == 0 || strcmp(gpu, "GAME_OVER") == 0) {
            puts("LEFT");
        } else {
            printf("%s\n", best_action(gpu, (int)strlen(gpu), reg[player]));
        }
        fflush(stdout);
    }
    return 0;
}
