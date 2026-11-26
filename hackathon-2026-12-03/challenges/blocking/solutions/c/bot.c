/* Blocking — C port of cpp/bot.cpp (same score, same tie-break).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { SIZE_WEIGHT = 100, LETTERS = 128, WORD = 64, BEST = 96 };

int main(void) {
    int shape_count, sizes[LETTERS] = {0};
    char letter[WORD], definition[WORD], word[WORD];
    if (scanf("%d", &shape_count) != 1) return 0;
    for (int i = 0; i < shape_count; ++i) {
        int cols, rows, squares = 0;
        if (scanf("%63s %d %d %63s", letter, &cols, &rows, definition) != 4) return 0;
        for (const char *p = definition; *p; ++p) squares += *p == '#';
        sizes[(unsigned char)letter[0] % LETTERS] = squares;
    }
    int players, me, board_size, size = 0;
    if (scanf("%d %d %d %63s", &players, &me, &board_size, word) != 4) return 0;
    while (scanf("%63s", word) == 1) {
        if (!size) size = (int)strlen(word);
        for (int i = 0; i < size - 1; ++i)
            if (scanf("%63s", word) != 1) return 0;
        int played, count;
        if (scanf("%d", &played) != 1) return 0;
        for (int i = 0; i < 4 * played; ++i)
            if (scanf("%63s", word) != 1) return 0;
        if (scanf("%d", &count) != 1) return 0;
        bool found = false;
        long long best_score = 0;
        char best[BEST] = "0 0 A001";
        for (int i = 0; i < count; ++i) {
            int col, row;
            if (scanf("%d %d %63s", &col, &row, word) != 3) return 0;
            /* doubled distance to the centre keeps everything integer */
            long long distance = abs(2 * col - (size - 1)) + abs(2 * row - (size - 1));
            long long score = 2LL * SIZE_WEIGHT * sizes[(unsigned char)word[0] % LETTERS] - distance;
            if (!found || score > best_score) {
                found = true;
                best_score = score;
                snprintf(best, sizeof best, "%d %d %s", col, row, word);
            }
        }
        printf("%s\n", best);
        fflush(stdout);
    }
    return 0;
}
