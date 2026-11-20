/*
 * dsa.hpp — socle DSA pour CodinGame (C++20, header-only).
 *
 * Conventions : STL, RAII, `const correcteness`, templates la ou c'est utile.
 * Aucune allocation manuelle : les conteneurs possedent leur memoire.
 */
#ifndef HACKATHON_DSA_HPP
#define HACKATHON_DSA_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dsa {

/* ------------------------------------------------------------------ */
/* Union-Find                                                          */
/* ------------------------------------------------------------------ */
class Dsu {
public:
    explicit Dsu(std::size_t n) : parent_(n), rank_(n, 0), count_(n) {
        std::iota(parent_.begin(), parent_.end(), 0);
    }

    std::size_t find(std::size_t x) {
        if (parent_[x] != x) parent_[x] = find(parent_[x]);
        return parent_[x];
    }

    bool unite(std::size_t a, std::size_t b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rank_[a] < rank_[b]) std::swap(a, b);
        parent_[b] = a;
        if (rank_[a] == rank_[b]) ++rank_[a];
        --count_;
        return true;
    }

    bool connected(std::size_t a, std::size_t b) { return find(a) == find(b); }
    std::size_t count() const noexcept { return count_; }

private:
    std::vector<std::size_t> parent_;
    std::vector<int> rank_;
    std::size_t count_;
};

/* ------------------------------------------------------------------ */
/* Graphe pondere                                                      */
/* ------------------------------------------------------------------ */
struct WeightedGraph {
    using Edge = std::pair<std::size_t, std::int64_t>; // (destination, poids)
    std::vector<std::vector<Edge>> adj;

    explicit WeightedGraph(std::size_t n) : adj(n) {}

    void add_edge(std::size_t u, std::size_t v, std::int64_t w = 1) {
        adj[u].emplace_back(v, w);
    }
    void add_undirected(std::size_t u, std::size_t v, std::int64_t w = 1) {
        add_edge(u, v, w);
        add_edge(v, u, w);
    }
    std::size_t size() const noexcept { return adj.size(); }
};

/* Distances en nombre d'aretes ; -1 si inatteignable. */
inline std::vector<int> bfs_distances(const WeightedGraph &g, std::size_t src) {
    std::vector<int> dist(g.size(), -1);
    std::queue<std::size_t> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        const auto u = q.front();
        q.pop();
        for (const auto &[v, w] : g.adj[u]) {
            (void)w;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

template <class Visitor>
void dfs(const WeightedGraph &g, std::size_t src, Visitor &&visit) {
    std::vector<bool> seen(g.size(), false);
    std::function<void(std::size_t)> recurse = [&](std::size_t u) {
        seen[u] = true;
        visit(u);
        for (const auto &[v, w] : g.adj[u]) {
            (void)w;
            if (!seen[v]) recurse(v);
        }
    };
    recurse(src);
}

/* LLONG_MAX si inatteignable. Poids negatifs non supportes. */
inline std::vector<std::int64_t> dijkstra(const WeightedGraph &g, std::size_t src) {
    constexpr auto INF = std::numeric_limits<std::int64_t>::max();
    using Item = std::pair<std::int64_t, std::size_t>;
    std::vector<std::int64_t> dist(g.size(), INF);
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    dist[src] = 0;
    pq.emplace(0, src);
    while (!pq.empty()) {
        const auto [d, u] = pq.top();
        pq.pop();
        if (d != dist[u]) continue; // entree obsolete
        for (const auto &[v, w] : g.adj[u]) {
            if (d + w < dist[v]) {
                dist[v] = d + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

/* heuristic[v] = estimation admissible de v -> goal ; {} = Dijkstra. */
inline std::vector<std::int64_t> astar(
    const WeightedGraph &g, std::size_t src, std::size_t goal,
    const std::vector<std::int64_t> &heuristic = {}) {
    constexpr auto INF = std::numeric_limits<std::int64_t>::max();
    using Item = std::pair<std::int64_t, std::size_t>;
    const auto h = [&](std::size_t v) -> std::int64_t {
        return heuristic.empty() ? 0 : heuristic[v];
    };
    std::vector<std::int64_t> dist(g.size(), INF);
    std::vector<bool> closed(g.size(), false);
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    dist[src] = 0;
    pq.emplace(h(src), src);
    while (!pq.empty()) {
        const auto u = pq.top().second;
        pq.pop();
        if (closed[u]) continue;
        if (u == goal) break;
        closed[u] = true;
        for (const auto &[v, w] : g.adj[u]) {
            if (closed[v]) continue;
            const auto nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                pq.emplace(nd + h(v), v);
            }
        }
    }
    return dist;
}

/* std::nullopt si cycle. */
inline std::optional<std::vector<std::size_t>> topological_sort(const WeightedGraph &g) {
    std::vector<std::size_t> indegree(g.size(), 0);
    for (const auto &edges : g.adj)
        for (const auto &[v, w] : edges) {
            (void)w;
            ++indegree[v];
        }
    std::queue<std::size_t> q;
    for (std::size_t i = 0; i < g.size(); ++i)
        if (indegree[i] == 0) q.push(i);
    std::vector<std::size_t> order;
    order.reserve(g.size());
    while (!q.empty()) {
        const auto u = q.front();
        q.pop();
        order.push_back(u);
        for (const auto &[v, w] : g.adj[u]) {
            (void)w;
            if (--indegree[v] == 0) q.push(v);
        }
    }
    if (order.size() != g.size()) return std::nullopt;
    return order;
}

/* Tarjan : renvoie {composante_de[i], nombre_de_composantes}. */
inline std::pair<std::vector<std::size_t>, std::size_t>
strongly_connected_components(const WeightedGraph &g) {
    std::vector<int> index(g.size(), -1), low(g.size(), 0);
    std::vector<bool> on_stack(g.size(), false);
    std::vector<std::size_t> stack;
    std::vector<std::size_t> comp(g.size(), 0);
    int timer = 0;
    std::size_t comps = 0;

    std::function<void(std::size_t)> visit = [&](std::size_t u) {
        index[u] = low[u] = timer++;
        stack.push_back(u);
        on_stack[u] = true;
        for (const auto &[v, w] : g.adj[u]) {
            (void)w;
            if (index[v] < 0) {
                visit(v);
                low[u] = std::min(low[u], low[v]);
            } else if (on_stack[v]) {
                low[u] = std::min(low[u], index[v]);
            }
        }
        if (low[u] == index[u]) {
            for (;;) {
                const auto v = stack.back();
                stack.pop_back();
                on_stack[v] = false;
                comp[v] = comps;
                if (v == u) break;
            }
            ++comps;
        }
    };
    for (std::size_t i = 0; i < g.size(); ++i)
        if (index[i] < 0) visit(i);
    return {comp, comps};
}

/* ------------------------------------------------------------------ */
/* Fenwick & Segment tree                                              */
/* ------------------------------------------------------------------ */
class Fenwick {
public:
    explicit Fenwick(std::size_t n) : bit_(n + 1, 0) {}

    void add(std::size_t i, std::int64_t delta) {
        for (++i; i < bit_.size(); i += i & (~i + 1)) bit_[i] += delta;
    }
    std::int64_t prefix_sum(std::size_t i) const {
        std::int64_t s = 0;
        for (++i; i > 0; i -= i & (~i + 1)) s += bit_[i];
        return s;
    }
    std::int64_t range_sum(std::size_t l, std::size_t r) const {
        return l == 0 ? prefix_sum(r - 1) : prefix_sum(r - 1) - prefix_sum(l - 1);
    }

private:
    std::vector<std::int64_t> bit_;
};

class SegmentTree {
public:
    explicit SegmentTree(const std::vector<std::int64_t> &values) : n_(values.size()) {
        size_ = 1;
        while (size_ < std::max<std::size_t>(n_, 1)) size_ *= 2;
        tree_.assign(2 * size_, 0);
        for (std::size_t i = 0; i < n_; ++i) tree_[size_ + i] = values[i];
        for (std::size_t i = size_ - 1; i > 0; --i)
            tree_[i] = tree_[2 * i] + tree_[2 * i + 1];
    }

    void add(std::size_t i, std::int64_t delta) {
        for (std::size_t p = size_ + i; p > 0; p /= 2) tree_[p] += delta;
    }
    /* somme sur [l, r). */
    std::int64_t query(std::size_t l, std::size_t r) const {
        std::int64_t res = 0;
        l += size_;
        r += size_;
        while (l < r) {
            if (l & 1) res += tree_[l++];
            if (r & 1) res += tree_[--r];
            l /= 2;
            r /= 2;
        }
        return res;
    }

private:
    std::size_t n_ = 0, size_ = 1;
    std::vector<std::int64_t> tree_;
};

/* ------------------------------------------------------------------ */
/* Mathematiques modulaires                                            */
/* ------------------------------------------------------------------ */
inline std::int64_t pow_mod(std::int64_t base, std::int64_t exp, std::int64_t mod) {
    std::int64_t result = 1 % mod;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = static_cast<__int128>(result) * base % mod;
        base = static_cast<__int128>(base) * base % mod;
        exp >>= 1;
    }
    return result;
}

/* mod premier ; a != 0 mod mod. */
inline std::int64_t mod_inverse(std::int64_t a, std::int64_t mod) {
    return pow_mod(a, mod - 2, mod);
}

inline std::vector<bool> sieve_primes(std::size_t n) {
    std::vector<bool> prime(n + 1, true);
    prime[0] = false;
    if (n >= 1) prime[1] = false;
    for (std::size_t p = 2; p * p <= n; ++p)
        if (prime[p])
            for (std::size_t k = p * p; k <= n; k += p) prime[k] = false;
    return prime;
}

/* ------------------------------------------------------------------ */
/* Geometrie                                                           */
/* ------------------------------------------------------------------ */
struct Point {
    double x = 0.0, y = 0.0;
    Point operator+(const Point &o) const { return {x + o.x, y + o.y}; }
    Point operator-(const Point &o) const { return {x - o.x, y - o.y}; }
    Point operator*(double k) const { return {x * k, y * k}; }
};

inline double dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
inline double cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
inline double dist2(Point a, Point b) { const auto d = a - b; return dot(d, d); }
inline double orient(Point a, Point b, Point c) { return cross(b - a, c - a); }

inline bool segments_intersect(Point a, Point b, Point c, Point d) {
    const double o1 = orient(a, b, c), o2 = orient(a, b, d);
    const double o3 = orient(c, d, a), o4 = orient(c, d, b);
    return ((o1 > 0) != (o2 > 0)) && ((o3 > 0) != (o4 > 0));
}

/* ------------------------------------------------------------------ */
/* Trie                                                                */
/* ------------------------------------------------------------------ */
class Trie {
public:
    void insert(const std::string &word) {
        std::size_t cur = 0;
        for (const char c : word) {
            const auto i = static_cast<std::size_t>(c - 'a');
            assert(i < kAlphabet && "trie: minuscules uniquement");
            if (!nodes_[cur].next[i]) {
                nodes_[cur].next[i] = nodes_.size();
                nodes_.emplace_back();
            }
            cur = nodes_[cur].next[i];
            ++nodes_[cur].count;
        }
    }

    bool contains(const std::string &word) const {
        const auto *node = descend(word);
        return node && node->count > 0;
    }

    std::size_t count_with_prefix(const std::string &prefix) const {
        const auto *node = descend(prefix);
        return node ? node->count : 0;
    }

private:
    static constexpr std::size_t kAlphabet = 26;
    struct Node {
        std::array<std::size_t, kAlphabet> next{};
        std::size_t count = 0;
    };
    std::vector<Node> nodes_{1};

    const Node *descend(const std::string &s) const {
        std::size_t cur = 0;
        for (const char c : s) {
            const auto i = static_cast<std::size_t>(c - 'a');
            if (i >= kAlphabet) return nullptr;
            cur = nodes_[cur].next[i];
            if (!cur) return nullptr;
        }
        return &nodes_[cur];
    }
};

} // namespace dsa

/* Exemple d'utilisation : g++ -O2 -std=c++20 dsa_selftest.cpp -o /tmp/dsa_cpp */
#ifdef DSA_SELFTEST_CPP
int main() {
    using namespace dsa;
    Dsu d(5);
    assert(d.unite(0, 1) && d.unite(1, 2));
    assert(d.connected(0, 2) && d.count() == 3);

    WeightedGraph g(4);
    g.add_undirected(0, 1, 1);
    g.add_undirected(1, 2, 2);
    g.add_undirected(2, 3, 1);
    assert(bfs_distances(g, 0)[3] == 3);
    assert(dijkstra(g, 0)[3] == 4);
    assert(topological_sort(g) == std::nullopt); // cycle

    Fenwick fw(5);
    fw.add(2, 7);
    assert(fw.range_sum(0, 3) == 7);

    SegmentTree st({1, 2, 3, 4, 5});
    assert(st.query(1, 4) == 9);
    st.add(1, 1);
    assert(st.query(1, 4) == 10);

    Trie trie;
    trie.insert("chat");
    trie.insert("chien");
    assert(trie.contains("chat") && trie.count_with_prefix("ch") == 2);

    std::cout << "dsa C++ : OK\n";
}
#endif

#endif // HACKATHON_DSA_HPP
