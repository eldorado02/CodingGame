/* Green Circle (Wood 4) — releases utiles et bureaux utiles. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXA 64
#define MAXL 16

static char line[4096];

static char *rd(void) {
    if (!fgets(line, sizeof(line), stdin)) return NULL;
    size_t n = strlen(line);
    while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = 0;
    return line;
}

static int imin(int a, int b) { return a < b ? a : b; }

static int good_score(int hand[10], int app[8]) {
    int covered = 0, total = 0;
    for (int i = 0; i < 8; i++) {
        covered += imin(app[i], 2 * hand[i]);
        total += app[i];
    }
    return covered + imin(hand[8], total - covered);
}

static int best_app(int hand[10], int aids[MAXA], int nids,
                    int app_ids[MAXA], int req[MAXA][8]) {
    int best_id = -1, best_score = -1;
    for (int k = 0; k < nids; k++) {
        int aid = aids[k];
        int app_index = -1;
        for (int i = 0; i < MAXA; i++) {
            if (app_ids[i] == aid) { app_index = i; break; }
        }
        if (app_index < 0) continue;
        int score = good_score(hand, req[app_index]);
        if (score > best_score || (score == best_score && aid < best_id)) {
            best_score = score;
            best_id = aid;
        }
    }
    return best_id;
}

int main(void) {
    char *p;
    while ((p = rd()) != NULL) {
        if (!*p) break;
        char phase[32];
        if (sscanf(p, "%31s", phase) != 1) break;

        int app_ids[MAXA], req[MAXA][8], napps = 0;
        int app_count = atoi(rd());
        for (int i = 0; i < app_count && napps < MAXA; i++) {
            char a[4096];
            char *r = rd();
            if (!r) return 0;
            strcpy(a, r);
            char kind[32];
            int aid, v[8];
            if (sscanf(a, "%31s %d %d %d %d %d %d %d %d %d",
                       kind, &aid, &v[0], &v[1], &v[2], &v[3],
                       &v[4], &v[5], &v[6], &v[7]) != 10) continue;
            app_ids[napps] = aid;
            for (int j = 0; j < 8; j++) req[napps][j] = v[j];
            napps++;
        }

        rd(); /* nous */
        rd(); /* adversaire */

        int hand[10] = {0};
        int loc_count = atoi(rd());
        for (int i = 0; i < loc_count; i++) {
            char l[4096];
            char *r = rd();
            if (!r) return 0;
            strcpy(l, r);
            char name[32];
            int v[10];
            if (sscanf(l, "%31s %d %d %d %d %d %d %d %d %d %d",
                       name, &v[0], &v[1], &v[2], &v[3], &v[4],
                       &v[5], &v[6], &v[7], &v[8], &v[9]) != 11) continue;
            if (strcmp(name, "HAND") == 0)
                for (int j = 0; j < 10; j++) hand[j] = v[j];
        }

        int move_count = atoi(rd());
        char moves[MAXA][256];
        int nmoves = 0;
        for (int i = 0; i < move_count && nmoves < MAXA; i++) {
            char *r = rd();
            if (!r) return 0;
            snprintf(moves[nmoves], sizeof(moves[nmoves]), "%s", r);
            nmoves++;
        }

        if (strncmp(phase, "MOVE", 4) == 0) {
            int aids[MAXA];
            for (int i = 0; i < napps; i++) aids[i] = app_ids[i];
            int chosen = -1, best_score = -1;
            for (int i = 0; i < nmoves; i++) {
                int dest;
                if (sscanf(moves[i], "MOVE %d", &dest) != 1) continue;
                int h2[10];
                for (int j = 0; j < 10; j++) h2[j] = hand[j];
                h2[dest]++;
                int aid = best_app(h2, aids, napps, app_ids, req);
                int score = aid >= 0 ? -1 : 0;
                if (aid >= 0) {
                    for (int k = 0; k < napps; k++)
                        if (app_ids[k] == aid) { score = good_score(h2, req[k]); break; }
                }
                if (score > best_score || (score == best_score && dest < chosen)) {
                    best_score = score;
                    chosen = dest;
                }
            }
            if (chosen >= 0) printf("MOVE %d\n", chosen);
            else if (nmoves) printf("%s\n", moves[0]);
            else printf("WAIT\n");
        } else if (strncmp(phase, "RELEASE", 7) == 0) {
            int aids[MAXA], nids = 0;
            for (int i = 0; i < nmoves && nids < MAXA; i++) {
                int aid;
                if (sscanf(moves[i], "RELEASE %d", &aid) == 1) aids[nids++] = aid;
            }
            int aid = best_app(hand, aids, nids, app_ids, req);
            if (aid >= 0) printf("RELEASE %d\n", aid);
            else printf("WAIT\n");
        } else {
            printf("%s\n", nmoves ? moves[0] : "WAIT");
        }
        fflush(stdout);
    }
    return 0;
}
