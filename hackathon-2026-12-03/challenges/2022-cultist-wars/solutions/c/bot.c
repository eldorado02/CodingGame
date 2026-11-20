/* Cultist Wars — conversions, neutralisation des menaces, tirs. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UNITS 20
#define INF 1000000

typedef struct { int id, type, hp, x, y, owner; } Unit;

static int W, H;
static char grid[16][32];

static int absi(int v) { return v < 0 ? -v : v; }
static int manhattan(const Unit *a, const Unit *b) {
    return absi(a->x - b->x) + absi(a->y - b->y);
}

/* Version avec occupation : 1 si tir valide, *_victim = id touché. */
static int shot_victim(const Unit *s, const Unit *t, const Unit *units, int n,
                       int *victim) {
    int x0, y0, x1, y1, backward;
    if (s->y < t->y) { x0=s->x; y0=s->y; x1=t->x; y1=t->y; backward=0; }
    else { x0=t->x; y0=t->y; x1=s->x; y1=s->y; backward=1; }
    int dx = absi(x1-x0), dy = absi(y1-y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx - dy, cx = x0, cy = y0;
    int bx = x1, by = y1;
    for (;;) {
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; cx += sx; }
        if (e2 < dx) { err += dx; cy += sy; }
        if (cx == x1 && cy == y1) break;
        if (cx < 0 || cx >= W || cy < 0 || cy >= H) return 0;
        int blocker = -1;
        if (grid[cy][cx] == 'x') blocker = -2;
        else {
            for (int i = 0; i < n; i++)
                if (units[i].hp > 0 && units[i].x == cx && units[i].y == cy) {
                    blocker = i; break;
                }
        }
        if (blocker >= 0 || blocker == -2) {
            if (backward) { bx = cx; by = cy; }
            else {
                if (blocker < 0) return 0;
                *victim = blocker;
                return 1;
            }
        }
    }
    for (int i = 0; i < n; i++)
        if (units[i].hp > 0 && units[i].x == bx && units[i].y == by) {
            *victim = i; return 1;
        }
    return 0;
}

static int bfs_distance(const Unit *s, const Unit *g) {
    if (s->x == g->x && s->y == g->y) return 0;
    int qx[256], qy[256], qd[256], head = 0, tail = 0;
    int seen[16][8];
    memset(seen, 0, sizeof(seen));
    qx[tail]=s->x; qy[tail]=s->y; qd[tail]=0; tail++;
    seen[s->x][s->y] = 1;
    static const int dx[4] = {0,1,0,-1}, dy[4] = {-1,0,1,0};
    while (head < tail) {
        int x=qx[head], y=qy[head], d=qd[head]; head++;
        for (int k=0;k<4;k++) {
            int nx=x+dx[k], ny=y+dy[k];
            if (nx<0||nx>=W||ny<0||ny>=H||seen[nx][ny]||grid[ny][nx]=='x') continue;
            if (nx==g->x && ny==g->y) return d+1;
            seen[nx][ny]=1; qx[tail]=nx; qy[tail]=ny; qd[tail]=d+1; tail++;
        }
    }
    return INF;
}

static int seek_ray(const Unit *shooter, const Unit *target, const Unit *units,
                    int n, int *victim) {
    return shot_victim(shooter, target, units, n, victim);
}

int main(void) {
    int my_id;
    if (scanf("%d", &my_id) != 1) return 0;
    scanf("%d %d", &W, &H);
    for (int y=0; y<H; y++) {
        char line[64];
        scanf("%63s", line);
        strncpy(grid[y], line, sizeof(grid[y])-1);
    }

    for (;;) {
        int n;
        if (scanf("%d", &n) != 1) break;
        Unit units[MAX_UNITS];
        for (int i=0; i<n && i<MAX_UNITS; i++)
            scanf("%d %d %d %d %d %d", &units[i].id, &units[i].type,
                  &units[i].hp, &units[i].x, &units[i].y, &units[i].owner);
        if (n > MAX_UNITS) n = MAX_UNITS;

        int leader=-1;
        int cultists[MAX_UNITS], nc=0;
        int enemies[MAX_UNITS], ne=0;
        int enemy_cultists[MAX_UNITS], nec=0;
        int mine[MAX_UNITS], nm=0;
        int neutral_idx[MAX_UNITS], nn=0;
        for (int i=0;i<n;i++) {
            if (units[i].hp <= 0) continue;
            if (units[i].owner == my_id) {
                mine[nm++] = i;
                if (units[i].type == 1) leader = i;
                else cultists[nc++] = i;
            } else if (units[i].owner == 2) {
                neutral_idx[nn++] = i;
            } else {
                enemies[ne++] = i;
                if (units[i].type == 0) enemy_cultists[nec++] = i;
            }
        }

        /* 1. cultiste ennemi qui menace l'une de nos unités */
        int threats[MAX_UNITS], nt=0;
        for (int a=0;a<nec;a++) {
            int e = enemy_cultists[a], found = 0;
            for (int b=0;b<nm;b++) {
                int u = mine[b];
                if (manhattan(&units[e], &units[u]) > 6) continue;
                int v;
                if (seek_ray(&units[e], &units[u], units, n, &v) && v == u) {
                    found = 1; break;
                }
            }
            if (found) threats[nt++] = e;
        }
        int best_key[5] = {9, 0, 0, 0, 0};
        int best_shooter=-1, best_victim=-1;
        if (nt > 0) {
            for (int a=0;a<nc;a++) {
                int c = cultists[a];
                for (int b=0;b<nt;b++) {
                    int e = threats[b], v;
                    if (manhattan(&units[c], &units[e]) > 6) continue;
                    if (!seek_ray(&units[c], &units[e], units, n, &v)) continue;
                    if (units[v].owner == my_id || units[v].owner == 2) continue;
                    int d = manhattan(&units[c], &units[v]);
                    int dmg = 7 - d;
                    int key[5] = {dmg >= units[v].hp ? 0 : 1, -dmg, d, units[v].id, units[c].id};
                    int better = 0;
                    for (int k=0;k<5;k++) {
                        if (key[k] < best_key[k]) { better = 1; break; }
                        if (key[k] > best_key[k]) break;
                    }
                    if (better) {
                        for (int k=0;k<5;k++) best_key[k]=key[k];
                        best_shooter=c; best_victim=v;
                    }
                }
            }
            if (best_shooter >= 0) {
                printf("%d SHOOT %d\n", units[best_shooter].id, units[best_victim].id);
                fflush(stdout);
                continue;
            }
        }

        /* 2. le leader convertit les neutres de sa moitié */
        if (leader >= 0 && nn > 0) {
            int mid = W / 2, target = -1, best_d = INF;
            for (int i=0;i<nn;i++) {
                int idx = neutral_idx[i];
                if (my_id == 0 ? units[idx].x > mid : units[idx].x < mid) continue;
                int d = bfs_distance(&units[leader], &units[idx]);
                if (d < best_d || (d == best_d && target >= 0 && units[idx].id < units[target].id)) {
                    best_d = d; target = idx;
                }
            }
            if (target >= 0) {
                printf("%d CONVERT %d\n", units[leader].id, units[target].id);
                fflush(stdout);
                continue;
            }
        }

        /* 3. vol d'un cultiste ennemi voisin */
        if (leader >= 0 && nec > 0) {
            int target=-1, best_d=INF;
            for (int a=0;a<nec;a++) {
                int d = bfs_distance(&units[leader], &units[enemy_cultists[a]]);
                if (d <= 2 && (d < best_d || (d==best_d && target>=0 &&
                        units[enemy_cultists[a]].id < units[target].id))) {
                    best_d=d; target=enemy_cultists[a];
                }
            }
            if (target >= 0) {
                printf("%d CONVERT %d\n", units[leader].id, units[target].id);
                fflush(stdout);
                continue;
            }
        }

        /* 4. tir libre */
        best_key[0]=9; best_key[1]=0; best_key[2]=0; best_key[3]=0; best_key[4]=0;
        best_shooter=-1; best_victim=-1;
        for (int a=0;a<nc;a++) {
            int c=cultists[a];
            for (int b=0;b<ne;b++) {
                int e=enemies[b], v;
                if (manhattan(&units[c], &units[e]) > 6) continue;
                if (!seek_ray(&units[c], &units[e], units, n, &v)) continue;
                if (units[v].owner == my_id || units[v].owner == 2) continue;
                int d=manhattan(&units[c], &units[v]), dmg=7-d;
                int key[5]={dmg>=units[v].hp?0:1,-dmg,d,units[v].id,units[c].id};
                int better=0;
                for (int k=0;k<5;k++) {
                    if (key[k]<best_key[k]) { better=1; break; }
                    if (key[k]>best_key[k]) break;
                }
                if (better) {
                    for (int k=0;k<5;k++) best_key[k]=key[k];
                    best_shooter=c; best_victim=v;
                }
            }
        }
        if (best_shooter>=0) {
            printf("%d SHOOT %d\n", units[best_shooter].id, units[best_victim].id);
            fflush(stdout);
            continue;
        }

        /* 5. SHOOT hors portée = déplacement */
        if (nc > 0 && ne > 0) {
            int best_c=cultists[0], best_e=enemies[0], best_d=INF;
            for (int a=0;a<nc;a++) {
                int md=INF;
                for (int b=0;b<ne;b++) {
                    int d=manhattan(&units[cultists[a]], &units[enemies[b]]);
                    if (d<md) md=d;
                }
                if (md<best_d || (md==best_d && units[cultists[a]].id<units[best_c].id)) {
                    best_d=md; best_c=cultists[a];
                }
            }
            best_d=INF;
            for (int b=0;b<ne;b++) {
                int d=manhattan(&units[best_c], &units[enemies[b]]);
                if (d<best_d || (d==best_d && units[enemies[b]].id<units[best_e].id)) {
                    best_d=d; best_e=enemies[b];
                }
            }
            printf("%d SHOOT %d\n", units[best_c].id, units[best_e].id);
            fflush(stdout);
            continue;
        }
        printf("WAIT\n");
        fflush(stdout);
    }
    return 0;
}
