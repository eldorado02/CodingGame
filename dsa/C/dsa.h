/*
 * dsa.h — socle de structures de donnees pour CodinGame (C11, header-only).
 *
 * Regles : snake_case, allocation explicite, chaque fonction `*_free` rend
 * la memoire. Les fonctions qui renvoient un pointeur peuvent renvoyer NULL
 * en cas d'echec d'allocation ; l'appelant doit le verifier.
 * Aucune dependence externe : compilable avec `gcc -O2 -std=c11`.
 */
#ifndef DSA_H
#define DSA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Header-only : les fonctions non utilisees par un challenge declenchent
 * -Wunused-function. On desactive localement cet avertissement. */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

/* ------------------------------------------------------------------ */
/* 1. Union-Find (DSU)                                                  */
/* ------------------------------------------------------------------ */

typedef struct {
    size_t *parent;
    size_t *rank;
    size_t count; /* nombre de composantes */
} Dsu;

static bool dsu_init(Dsu *d, size_t n) {
    d->parent = malloc(n * sizeof *d->parent);
    d->rank = calloc(n, sizeof *d->rank);
    if (!d->parent || !d->rank) {
        free(d->parent);
        free(d->rank);
        d->parent = NULL;
        d->rank = NULL;
        d->count = 0;
        return false;
    }
    for (size_t i = 0; i < n; ++i) d->parent[i] = i;
    d->count = n;
    return true;
}

static void dsu_free(Dsu *d) {
    free(d->parent);
    free(d->rank);
    d->parent = NULL;
    d->rank = NULL;
    d->count = 0;
}

static size_t dsu_find(Dsu *d, size_t x) {
    while (d->parent[x] != x) {
        d->parent[x] = d->parent[d->parent[x]]; /* compression */
        x = d->parent[x];
    }
    return x;
}

static bool dsu_union(Dsu *d, size_t a, size_t b) {
    size_t ra = dsu_find(d, a), rb = dsu_find(d, b);
    if (ra == rb) return false;
    if (d->rank[ra] < d->rank[rb]) { size_t t = ra; ra = rb; rb = t; }
    d->parent[rb] = ra;
    if (d->rank[ra] == d->rank[rb]) ++d->rank[ra];
    --d->count;
    return true;
}

static bool dsu_connected(Dsu *d, size_t a, size_t b) {
    return dsu_find(d, a) == dsu_find(d, b);
}

/* ------------------------------------------------------------------ */
/* 2. Graphe (liste d'adjacence ponderee)                               */
/* ------------------------------------------------------------------ */

typedef struct {
    size_t n;
    size_t *degree;
    size_t *capacity;
    int **adj;
    long long **weight; /* NULL si non pondere ; sinon parallel a adj */
} Graph;

static void graph_free(Graph *g);

static bool graph_init(Graph *g, size_t n, bool weighted) {
    memset(g, 0, sizeof *g);
    g->n = n;
    g->degree = calloc(n ? n : 1, sizeof *g->degree);
    g->capacity = calloc(n ? n : 1, sizeof *g->capacity);
    g->adj = calloc(n ? n : 1, sizeof *g->adj);
    if (weighted) g->weight = calloc(n ? n : 1, sizeof *g->weight);
    if (!g->degree || !g->capacity || !g->adj || (weighted && !g->weight)) {
        graph_free(g);
        return false;
    }
    return true;
}

static bool graph_add_edge(Graph *g, size_t u, size_t v, long long w) {
    if (u >= g->n || v >= g->n) return false;
    if (g->degree[u] == g->capacity[u]) {
        size_t nc = g->capacity[u] ? g->capacity[u] * 2 : 4;
        int *na = realloc(g->adj[u], nc * sizeof *na);
        if (!na) return false;
        g->adj[u] = na;
        if (g->weight) {
            long long *nw = realloc(g->weight[u], nc * sizeof *nw);
            if (!nw) return false;
            g->weight[u] = nw;
        }
        g->capacity[u] = nc;
    }
    g->adj[u][g->degree[u]] = (int)v;
    if (g->weight) g->weight[u][g->degree[u]] = w;
    ++g->degree[u];
    return true;
}

static void graph_add_undirected(Graph *g, size_t u, size_t v, long long w) {
    graph_add_edge(g, u, v, w);
    graph_add_edge(g, v, u, w);
}

static void graph_free(Graph *g) {
    if (!g) return;
    for (size_t i = 0; i < g->n; ++i) {
        free(g->adj[i]);
        if (g->weight) free(g->weight[i]);
    }
    free(g->adj);
    free(g->weight);
    free(g->degree);
    free(g->capacity);
    memset(g, 0, sizeof *g);
}

/* ------------------------------------------------------------------ */
/* 3. Min-heap de couples (cle, valeur)                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    long long *key;
    size_t *val;
    size_t len;
    size_t cap;
} MinHeap;

static void heap_free(MinHeap *h);

static bool heap_init(MinHeap *h, size_t cap) {
    memset(h, 0, sizeof *h);
    if (!cap) cap = 16;
    h->key = malloc(cap * sizeof *h->key);
    h->val = malloc(cap * sizeof *h->val);
    if (!h->key || !h->val) { heap_free(h); return false; }
    h->cap = cap;
    return true;
}

static void heap_free(MinHeap *h) {
    free(h->key); free(h->val);
    memset(h, 0, sizeof *h);
}

static bool heap_push(MinHeap *h, long long key, size_t val) {
    if (h->len == h->cap) {
        size_t nc = h->cap * 2;
        long long *nk = realloc(h->key, nc * sizeof *nk);
        if (!nk) return false;
        h->key = nk;
        size_t *nv = realloc(h->val, nc * sizeof *nv);
        if (!nv) return false;
        h->val = nv;
        h->cap = nc;
    }
    size_t i = h->len++;
    h->key[i] = key; h->val[i] = val;
    while (i > 0) {
        size_t p = (i - 1) / 2;
        if (h->key[p] <= h->key[i]) break;
        long long tk = h->key[p]; h->key[p] = h->key[i]; h->key[i] = tk;
        size_t tv = h->val[p]; h->val[p] = h->val[i]; h->val[i] = tv;
        i = p;
    }
    return true;
}

static bool heap_pop(MinHeap *h, long long *out_key, size_t *out_val) {
    if (h->len == 0) return false;
    if (out_key) *out_key = h->key[0];
    if (out_val) *out_val = h->val[0];
    --h->len;
    if (h->len > 0) {
        h->key[0] = h->key[h->len];
        h->val[0] = h->val[h->len];
        size_t i = 0;
        for (;;) {
            size_t l = 2 * i + 1, r = l + 1, m = i;
            if (l < h->len && h->key[l] < h->key[m]) m = l;
            if (r < h->len && h->key[r] < h->key[m]) m = r;
            if (m == i) break;
            long long tk = h->key[m]; h->key[m] = h->key[i]; h->key[i] = tk;
            size_t tv = h->val[m]; h->val[m] = h->val[i]; h->val[i] = tv;
            i = m;
        }
    }
    return true;
}

/* ------------------------------------------------------------------ */
/* 4. Parcours : BFS, DFS, Dijkstra, A*, topologique, SCC               */
/* ------------------------------------------------------------------ */

/* distances en nombre d'aretes ; -1 = inatteignable. Appelant : free(). */
static int *bfs_distances(const Graph *g, size_t src, int *out_count) {
    int *dist = malloc(g->n * sizeof *dist);
    size_t *queue = malloc(g->n * sizeof *queue);
    if (!dist || !queue) { free(dist); free(queue); return NULL; }
    for (size_t i = 0; i < g->n; ++i) dist[i] = -1;
    size_t head = 0, tail = 0;
    dist[src] = 0; queue[tail++] = src;
    while (head < tail) {
        size_t u = queue[head++];
        for (size_t k = 0; k < g->degree[u]; ++k) {
            size_t v = (size_t)g->adj[u][k];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                queue[tail++] = v;
            }
        }
    }
    if (out_count) *out_count = (int)tail;
    free(queue);
    return dist;
}

static void dfs_recursive(const Graph *g, size_t u, bool *seen,
                          void (*visit)(size_t, void *), void *ctx) {
    seen[u] = true;
    if (visit) visit(u, ctx);
    for (size_t k = 0; k < g->degree[u]; ++k) {
        size_t v = (size_t)g->adj[u][k];
        if (!seen[v]) dfs_recursive(g, v, seen, visit, ctx);
    }
}

static void dfs_from(const Graph *g, size_t src, void (*visit)(size_t, void *),
                     void *ctx) {
    bool *seen = calloc(g->n ? g->n : 1, sizeof *seen);
    if (!seen) return;
    dfs_recursive(g, src, seen, visit, ctx);
    free(seen);
}

/* LLONG_MAX = inatteignable. Poids negatifs non supportes. */
static long long *dijkstra(const Graph *g, size_t src) {
    const long long INF = INT64_MAX;
    long long *dist = malloc(g->n * sizeof *dist);
    bool *done = calloc(g->n ? g->n : 1, sizeof *done);
    MinHeap heap;
    if (!dist || !done || !heap_init(&heap, g->n * 2 + 1)) {
        free(dist); free(done); heap_free(&heap); return NULL;
    }
    for (size_t i = 0; i < g->n; ++i) dist[i] = INF;
    dist[src] = 0;
    heap_push(&heap, 0, src);
    while (heap.len) {
        long long d; size_t u;
        heap_pop(&heap, &d, &u);
        if (done[u]) continue;
        done[u] = true;
        for (size_t k = 0; k < g->degree[u]; ++k) {
            size_t v = (size_t)g->adj[u][k];
            long long w = g->weight ? g->weight[u][k] : 1;
            if (d + w < dist[v]) {
                dist[v] = d + w;
                heap_push(&heap, dist[v], v);
            }
        }
    }
    heap_free(&heap); free(done);
    return dist;
}

/* heuristic[i] = estimation de i -> goal ; NULL = 0 (equivaut a Dijkstra). */
static long long *astar(const Graph *g, size_t src, size_t goal,
                        const long long *heuristic) {
    const long long INF = INT64_MAX;
    long long *dist = malloc(g->n * sizeof *dist);
    bool *closed = calloc(g->n ? g->n : 1, sizeof *closed);
    MinHeap heap;
    if (!dist || !closed || !heap_init(&heap, g->n * 2 + 1)) {
        free(dist); free(closed); heap_free(&heap); return NULL;
    }
    for (size_t i = 0; i < g->n; ++i) dist[i] = INF;
    dist[src] = 0;
    heap_push(&heap, heuristic ? heuristic[src] : 0, src);
    while (heap.len) {
        long long f; size_t u;
        heap_pop(&heap, &f, &u);
        if (closed[u]) continue;
        if (u == goal) break;
        closed[u] = true;
        for (size_t k = 0; k < g->degree[u]; ++k) {
            size_t v = (size_t)g->adj[u][k];
            if (closed[v]) continue;
            long long w = g->weight ? g->weight[u][k] : 1;
            long long nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                heap_push(&heap, nd + (heuristic ? heuristic[v] : 0), v);
            }
        }
    }
    heap_free(&heap); free(closed);
    return dist;
}

/* Ordre topologique (Kahn). Renvoie false s'il existe un cycle. */
static bool topological_sort(const Graph *g, size_t *order, size_t *out_len) {
    size_t *indeg = calloc(g->n ? g->n : 1, sizeof *indeg);
    size_t *queue = malloc((g->n ? g->n : 1) * sizeof *queue);
    if (!indeg || !queue) { free(indeg); free(queue); return false; }
    for (size_t u = 0; u < g->n; ++u)
        for (size_t k = 0; k < g->degree[u]; ++k) ++indeg[g->adj[u][k]];
    size_t head = 0, tail = 0;
    for (size_t i = 0; i < g->n; ++i) if (!indeg[i]) queue[tail++] = i;
    size_t written = 0;
    while (head < tail) {
        size_t u = queue[head++];
        order[written++] = u;
        for (size_t k = 0; k < g->degree[u]; ++k) {
            size_t v = (size_t)g->adj[u][k];
            if (--indeg[v] == 0) queue[tail++] = v;
        }
    }
    *out_len = written;
    free(indeg); free(queue);
    return written == g->n;
}

/* SCC de Tarjan (iteratif sur la pile d'appels via recursion). */
typedef struct {
    const Graph *g;
    long long *index, *low;
    bool *on_stack;
    size_t *stack, *comp;
    size_t top, time_index, comp_count;
} Tarjan;

static void tarjan_visit(Tarjan *t, size_t u) {
    t->index[u] = t->low[u] = t->time_index++;
    t->stack[t->top++] = u;
    t->on_stack[u] = true;
    for (size_t k = 0; k < t->g->degree[u]; ++k) {
        size_t v = (size_t)t->g->adj[u][k];
        if (t->index[v] < 0) {
            tarjan_visit(t, v);
            if (t->low[v] < t->low[u]) t->low[u] = t->low[v];
        } else if (t->on_stack[v] && t->index[v] < t->low[u]) {
            t->low[u] = t->index[v];
        }
    }
    if (t->low[u] == t->index[u]) {
        for (;;) {
            size_t w = t->stack[--t->top];
            t->on_stack[w] = false;
            t->comp[w] = t->comp_count;
            if (w == u) break;
        }
        ++t->comp_count;
    }
}

/* comp[i] = id de composante ; *out_count = nombre de composantes. */
static size_t *strongly_connected_components(const Graph *g, size_t *out_count) {
    size_t n = g->n ? g->n : 1;
    Tarjan t;
    memset(&t, 0, sizeof t);
    t.g = g;
    t.index = malloc(n * sizeof *t.index);
    t.low = malloc(n * sizeof *t.low);
    t.on_stack = calloc(n, sizeof *t.on_stack);
    t.stack = malloc(n * sizeof *t.stack);
    t.comp = malloc(n * sizeof *t.comp);
    if (!t.index || !t.low || !t.on_stack || !t.stack || !t.comp) {
        free(t.index); free(t.low); free(t.on_stack); free(t.stack); free(t.comp);
        return NULL;
    }
    for (size_t i = 0; i < g->n; ++i) t.index[i] = -1;
    for (size_t i = 0; i < g->n; ++i)
        if (t.index[i] < 0) tarjan_visit(&t, i);
    free(t.index); free(t.low); free(t.on_stack); free(t.stack);
    if (out_count) *out_count = t.comp_count;
    return t.comp;
}

/* ------------------------------------------------------------------ */
/* 5. Fenwick & Segment tree                                           */
/* ------------------------------------------------------------------ */

typedef struct { size_t n; long long *bit; } Fenwick;

static bool fenwick_init(Fenwick *f, size_t n) {
    f->n = n;
    f->bit = calloc(n + 1, sizeof *f->bit);
    return f->bit != NULL;
}
static void fenwick_free(Fenwick *f) { free(f->bit); f->bit = NULL; f->n = 0; }
static void fenwick_add(Fenwick *f, size_t i, long long delta) {
    for (++i; i <= f->n; i += i & (~i + 1)) f->bit[i] += delta;
}
static long long fenwick_sum(const Fenwick *f, size_t i) {
    long long s = 0;
    for (++i; i > 0; i -= i & (~i + 1)) s += f->bit[i];
    return s;
}
static long long fenwick_range(const Fenwick *f, size_t l, size_t r) {
    if (l == 0) return fenwick_sum(f, r - 1);
    return fenwick_sum(f, r - 1) - fenwick_sum(f, l - 1);
}

typedef struct { size_t n, size_pow2; long long *tree; } SegmentTree;

static bool segtree_init(SegmentTree *s, const long long *values, size_t n) {
    memset(s, 0, sizeof *s);
    s->size_pow2 = 1;
    while (s->size_pow2 < (n ? n : 1)) s->size_pow2 *= 2;
    s->n = n;
    s->tree = calloc(2 * s->size_pow2, sizeof *s->tree);
    if (!s->tree) return false;
    for (size_t i = 0; i < n; ++i) s->tree[s->size_pow2 + i] = values[i];
    for (size_t i = s->size_pow2 - 1; i > 0; --i)
        s->tree[i] = s->tree[2 * i] + s->tree[2 * i + 1];
    return true;
}
static void segtree_free(SegmentTree *s) { free(s->tree); memset(s, 0, sizeof *s); }
static void segtree_add(SegmentTree *s, size_t i, long long delta) {
    for (size_t p = s->size_pow2 + i; p > 0; p /= 2) s->tree[p] += delta;
}
/* somme sur [l, r) — index 0-based, r exclu. */
static long long segtree_query(const SegmentTree *s, size_t l, size_t r) {
    long long res = 0;
    l += s->size_pow2; r += s->size_pow2;
    while (l < r) {
        if (l & 1) res += s->tree[l++];
        if (r & 1) res += s->tree[--r];
        l /= 2; r /= 2;
    }
    return res;
}

/* ------------------------------------------------------------------ */
/* 6. Mathematiques modulaires & geometrie                             */
/* ------------------------------------------------------------------ */

static long long gcd_ll(long long a, long long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { long long t = a % b; a = b; b = t; }
    return a;
}
static long long lcm_ll(long long a, long long b) {
    return a / gcd_ll(a, b) * b;
}
static long long pow_mod(long long base, long long exp, long long mod) {
    long long result = 1 % mod;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (__int128)result * base % mod;
        base = (__int128)base * base % mod;
        exp >>= 1;
    }
    return result;
}
/* mod premier ; a != 0 mod mod. */
static long long mod_inverse(long long a, long long mod) {
    return pow_mod(a, mod - 2, mod);
}
/* primes[0..n-1] = est premier. Appelant : free(). */
static bool *sieve_primes(size_t n) {
    bool *prime = malloc((n + 1) * sizeof *prime);
    if (!prime) return NULL;
    memset(prime, true, (n + 1) * sizeof *prime);
    prime[0] = false;
    if (n >= 1) prime[1] = false;
    for (size_t p = 2; p * p <= n; ++p)
        if (prime[p])
            for (size_t k = p * p; k <= n; k += p) prime[k] = false;
    return prime;
}

typedef struct { double x, y; } Point;

static double point_dist2(Point a, Point b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}
static double point_dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
static double point_cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
/* orientation de (a,b,c) : >0 gauche, <0 droite, =0 aligne. */
static double orient(Point a, Point b, Point c) {
    return point_cross((Point){b.x - a.x, b.y - a.y},
                       (Point){c.x - a.x, c.y - a.y});
}
static bool segments_intersect(Point a, Point b, Point c, Point d) {
    double o1 = orient(a, b, c), o2 = orient(a, b, d);
    double o3 = orient(c, d, a), o4 = orient(c, d, b);
    return ((o1 > 0) != (o2 > 0)) && ((o3 > 0) != (o4 > 0));
}

/* ------------------------------------------------------------------ */
/* 7. Trie, recherche binaire, table de hachage                        */
/* ------------------------------------------------------------------ */

#define TRIE_ALPHABET 26

typedef struct {
    int next[TRIE_ALPHABET];
    int count; /* mots passant par ce noeud */
} TrieNode;

typedef struct {
    TrieNode *nodes;
    size_t len, cap;
} Trie;

static bool trie_init(Trie *t, size_t expected_words) {
    memset(t, 0, sizeof *t);
    t->cap = expected_words ? expected_words * 8 + 1 : 256;
    t->nodes = calloc(t->cap, sizeof *t->nodes);
    /* Indice 0 = racine, 0 dans `next` = absence : les vrais noeuds
     * commencent donc a 1. */
    if (t->nodes) t->len = 1;
    return t->nodes != NULL;
}
static void trie_free(Trie *t) { free(t->nodes); memset(t, 0, sizeof *t); }
static bool trie_insert(Trie *t, const char *word) {
    int cur = 0;
    for (const char *p = word; *p; ++p) {
        int c = *p - 'a';
        if (c < 0 || c >= TRIE_ALPHABET) return false;
        if (!t->nodes[cur].next[c]) {
            if (t->len + 1 >= t->cap) {
                size_t nc = t->cap * 2;
                TrieNode *nn = realloc(t->nodes, nc * sizeof *nn);
                if (!nn) return false;
                t->nodes = nn;
                memset(t->nodes + t->cap, 0, (nc - t->cap) * sizeof *t->nodes);
                t->cap = nc;
            }
            t->nodes[cur].next[c] = (int)t->len++;
        }
        cur = t->nodes[cur].next[c];
        ++t->nodes[cur].count;
    }
    return true;
}
static bool trie_search(const Trie *t, const char *word) {
    int cur = 0;
    for (const char *p = word; *p; ++p) {
        int c = *p - 'a';
        if (c < 0 || c >= TRIE_ALPHABET) return false;
        cur = t->nodes[cur].next[c];
        if (!cur) return false;
    }
    return t->nodes[cur].count > 0;
}
static size_t trie_count_prefix(const Trie *t, const char *prefix) {
    int cur = 0;
    for (const char *p = prefix; *p; ++p) {
        int c = *p - 'a';
        if (c < 0 || c >= TRIE_ALPHABET) return 0;
        cur = t->nodes[cur].next[c];
        if (!cur) return 0;
    }
    return (size_t)t->nodes[cur].count;
}

/* premier index >= value ; n si aucun. */
static size_t lower_bound_i(const int *a, size_t n, int value) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (a[mid] < value) lo = mid + 1; else hi = mid;
    }
    return lo;
}
static size_t upper_bound_i(const int *a, size_t n, int value) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (a[mid] <= value) lo = mid + 1; else hi = mid;
    }
    return lo;
}

/* `strdup` est POSIX, pas C11 : on fournit une version portable. */
static char *dsa_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *copy = malloc(n);
    if (copy) memcpy(copy, s, n);
    return copy;
}

/* Table chainee string -> int (open addressing, linear probing). */
typedef struct {
    char **keys;
    int *values;
    bool *used;
    size_t cap, len;
} StringMap;

static bool string_map_init(StringMap *m, size_t expected) {
    memset(m, 0, sizeof *m);
    m->cap = 1;
    while (m->cap < expected * 2 + 1) m->cap *= 2;
    m->keys = calloc(m->cap, sizeof *m->keys);
    m->values = calloc(m->cap, sizeof *m->values);
    m->used = calloc(m->cap, sizeof *m->used);
    return m->keys && m->values && m->used;
}
static void string_map_free(StringMap *m) {
    if (!m) return;
    for (size_t i = 0; i < m->cap; ++i) free(m->keys[i]);
    free(m->keys); free(m->values); free(m->used);
    memset(m, 0, sizeof *m);
}
static size_t string_hash(const char *s) {
    size_t h = 1469598103934665603ULL;
    for (; *s; ++s) { h ^= (unsigned char)*s; h *= 1099511628211ULL; }
    return h;
}
static bool string_map_set(StringMap *m, const char *key, int value) {
    size_t i = string_hash(key) & (m->cap - 1);
    while (m->used[i] && strcmp(m->keys[i], key) != 0)
        i = (i + 1) & (m->cap - 1);
    if (!m->used[i]) {
        char *copy = dsa_strdup(key);
        if (!copy) return false;
        m->keys[i] = copy;
        m->used[i] = true;
        ++m->len;
    }
    m->values[i] = value;
    return true;
}
static bool string_map_get(const StringMap *m, const char *key, int *out) {
    size_t i = string_hash(key) & (m->cap - 1);
    while (m->used[i]) {
        if (strcmp(m->keys[i], key) == 0) { *out = m->values[i]; return true; }
        i = (i + 1) & (m->cap - 1);
    }
    return false;
}

/* ------------------------------------------------------------------ */
/* Exemple d'utilisation / test rapide                                 */
/* ------------------------------------------------------------------ */
static void dsa_selftest(void) {
    Dsu d; dsu_init(&d, 5);
    dsu_union(&d, 0, 1); dsu_union(&d, 1, 2);
    if (!dsu_connected(&d, 0, 2) || d.count != 3) abort();

    Graph g; graph_init(&g, 4, true);
    graph_add_undirected(&g, 0, 1, 1);
    graph_add_undirected(&g, 1, 2, 2);
    graph_add_undirected(&g, 2, 3, 1);
    int *dist = bfs_distances(&g, 0, NULL);
    if (!dist || dist[3] != 3) abort();
    free(dist);
    long long *dij = dijkstra(&g, 0);
    if (!dij || dij[3] != 4) abort();
    free(dij);
    graph_free(&g);

    Fenwick f; fenwick_init(&f, 5);
    fenwick_add(&f, 2, 7);
    if (fenwick_range(&f, 0, 3) != 7) abort();
    fenwick_free(&f);

    long long vals[] = {1, 2, 3, 4, 5};
    SegmentTree s; segtree_init(&s, vals, 5);
    if (segtree_query(&s, 1, 4) != 9) abort();
    segtree_add(&s, 1, 1);
    if (segtree_query(&s, 1, 4) != 10) abort();
    segtree_free(&s);

    Trie t; trie_init(&t, 10);
    trie_insert(&t, "chat"); trie_insert(&t, "chien");
    if (!trie_search(&t, "chat") || trie_count_prefix(&t, "ch") != 2) abort();
    trie_free(&t);

    StringMap m; string_map_init(&m, 4);
    if (!string_map_set(&m, "cle", 42)) abort();
    int v = 0;
    if (!string_map_get(&m, "cle", &v) || v != 42) abort();
    string_map_free(&m);

    puts("dsa C : OK");
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif /* DSA_H */

/* gcc -O2 -std=c11 dsa_selftest.c -o /tmp/dsa_c && /tmp/dsa_c
 * (definir DSA_MAIN pour compiler ce test directement depuis le header) */
#ifdef DSA_MAIN
int main(void) { dsa_selftest(); return 0; }
#endif
