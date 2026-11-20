/* Fall Challenge 2020 (Wood 4) — brasser la recette la plus rentable. */
#include <stdio.h>
#include <string.h>

#define MAXA 128

static char line[4096];

static char *rd(void) {
    if (!fgets(line, sizeof(line), stdin)) return NULL;
    size_t n = 0;
    while (line[n]) n++;
    while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = 0;
    return line;
}

int main(void) {
    char *p;
    while ((p = rd()) != NULL) {
        if (!*p) break;
        int n;
        if (sscanf(p, "%d", &n) != 1) break;
        int id[MAXA], need[MAXA][4], price[MAXA], cnt = 0;
        for (int i = 0; i < n && cnt < MAXA; i++) {
            char o[4096];
            char *r = rd();
            if (!r) return 0;
            strcpy(o, r);
            int aid, d0, d1, d2, d3, pr, ti, tax, cast, rep;
            char type[32];
            if (sscanf(o, "%d %31s %d %d %d %d %d %d %d %d %d",
                       &aid, type, &d0, &d1, &d2, &d3, &pr, &ti, &tax,
                       &cast, &rep) != 11) continue;
            if (type[0] != 'B') continue;
            id[cnt] = aid;
            need[cnt][0] = -d0; need[cnt][1] = -d1;
            need[cnt][2] = -d2; need[cnt][3] = -d3;
            price[cnt] = pr;
            cnt++;
        }
        int inv[4], myscore;
        char *invline = rd();
        if (!invline) return 0;
        if (sscanf(invline, "%d %d %d %d %d", &inv[0], &inv[1], &inv[2],
                   &inv[3], &myscore) != 5) break;
        rd(); /* adversaire */

        int bestidx = -1, bestprice = -1, bestsum = 0;
        for (int i = 0; i < cnt; i++) {
            int ok = 1, sum = 0;
            for (int j = 0; j < 4; j++) {
                if (need[i][j] > inv[j]) { ok = 0; break; }
                sum += need[i][j];
            }
            if (!ok) continue;
            if (bestidx < 0 || price[i] > bestprice ||
                (price[i] == bestprice && sum < bestsum) ||
                (price[i] == bestprice && sum == bestsum && id[i] < id[bestidx])) {
                bestidx = i; bestprice = price[i]; bestsum = sum;
            }
        }
        if (bestidx >= 0) printf("BREW %d\n", id[bestidx]);
        else printf("WAIT\n");
        fflush(stdout);
    }
    return 0;
}
