//! Socle DSA pour CodinGame (Rust 2021, sans dépendance externe).
//!
//! Conventions : ownership explicite, `Result`/`Option`, aucun `unwrap` dans la
//! logique métier. Les erreurs proviennent de données invalides (sommet hors
//! graphe, mot hors alphabet) ; les erreurs de programmation (index hors table)
//! restent des paniques d'indexation, comme il est d'usage en Rust.

use std::cmp::{min, Ordering};
use std::collections::{BinaryHeap, HashMap, VecDeque};
use std::error::Error;
use std::fmt;

/// Erreurs liées aux données fournies au socle.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DsaError {
    InvalidVertex { vertex: usize, len: usize },
    InvalidCharacter(char),
}

impl fmt::Display for DsaError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::InvalidVertex { vertex, len } => {
                write!(f, "sommet {vertex} hors graphe (taille {len})")
            }
            Self::InvalidCharacter(c) => write!(f, "caractère invalide pour le trie : '{c}'"),
        }
    }
}

impl Error for DsaError {}

/* ------------------------------------------------------------------------ */
/* Union-Find                                                                */
/* ------------------------------------------------------------------------ */

#[derive(Debug, Clone)]
pub struct Dsu {
    parent: Vec<usize>,
    rank: Vec<usize>,
    count: usize,
}

impl Dsu {
    pub fn new(n: usize) -> Self {
        Self {
            parent: (0..n).collect(),
            rank: vec![0; n],
            count: n,
        }
    }

    pub fn find(&mut self, x: usize) -> usize {
        if self.parent[x] != x {
            let root = self.find(self.parent[x]);
            self.parent[x] = root; // compression
        }
        self.parent[x]
    }

    /// Renvoie `true` si les deux composantes étaient distinctes.
    pub fn union(&mut self, a: usize, b: usize) -> bool {
        let (a, b) = (self.find(a), self.find(b));
        if a == b {
            return false;
        }
        match self.rank[a].cmp(&self.rank[b]) {
            Ordering::Less => self.parent[a] = b,
            Ordering::Greater => self.parent[b] = a,
            Ordering::Equal => {
                self.parent[b] = a;
                self.rank[a] += 1;
            }
        }
        self.count -= 1;
        true
    }

    pub fn connected(&mut self, a: usize, b: usize) -> bool {
        self.find(a) == self.find(b)
    }

    pub fn count(&self) -> usize {
        self.count
    }
}

/* ------------------------------------------------------------------------ */
/* Graphe & parcours                                                         */
/* ------------------------------------------------------------------------ */

#[derive(Debug, Clone)]
pub struct Graph {
    /// `(destination, poids)` ; le poids vaut 1 pour un graphe non pondéré.
    adj: Vec<Vec<(usize, i64)>>,
}

impl Graph {
    pub fn new(n: usize) -> Self {
        Self { adj: vec![Vec::new(); n] }
    }

    pub fn len(&self) -> usize {
        self.adj.len()
    }

    pub fn is_empty(&self) -> bool {
        self.adj.is_empty()
    }

    pub fn add_edge(&mut self, u: usize, v: usize, weight: i64) -> Result<(), DsaError> {
        if u >= self.len() || v >= self.len() {
            return Err(DsaError::InvalidVertex {
                vertex: if u >= self.len() { u } else { v },
                len: self.len(),
            });
        }
        self.adj[u].push((v, weight));
        Ok(())
    }

    pub fn add_undirected(&mut self, u: usize, v: usize, weight: i64) -> Result<(), DsaError> {
        self.add_edge(u, v, weight)?;
        self.add_edge(v, u, weight)
    }

    /// Distances en nombre d'arêtes, `-1` si inatteignable.
    pub fn bfs_distances(&self, src: usize) -> Result<Vec<i32>, DsaError> {
        if src >= self.len() {
            return Err(DsaError::InvalidVertex { vertex: src, len: self.len() });
        }
        let mut dist = vec![-1; self.len()];
        let mut queue = VecDeque::new();
        dist[src] = 0;
        queue.push_back(src);
        while let Some(u) = queue.pop_front() {
            for &(v, _) in &self.adj[u] {
                if dist[v] == -1 {
                    dist[v] = dist[u] + 1;
                    queue.push_back(v);
                }
            }
        }
        Ok(dist)
    }

    pub fn dfs<F: FnMut(usize)>(&self, src: usize, mut visit: F) -> Result<(), DsaError> {
        if src >= self.len() {
            return Err(DsaError::InvalidVertex { vertex: src, len: self.len() });
        }
        let mut seen = vec![false; self.len()];
        self.dfs_from(src, &mut seen, &mut visit);
        Ok(())
    }

    fn dfs_from<F: FnMut(usize)>(&self, u: usize, seen: &mut [bool], visit: &mut F) {
        seen[u] = true;
        visit(u);
        for &(v, _) in &self.adj[u] {
            if !seen[v] {
                self.dfs_from(v, seen, visit);
            }
        }
    }

    /// Plus courts chemins ; `i64::MAX` si inatteignable. Poids négatifs non supportés.
    pub fn dijkstra(&self, src: usize) -> Result<Vec<i64>, DsaError> {
        if src >= self.len() {
            return Err(DsaError::InvalidVertex { vertex: src, len: self.len() });
        }
        let mut dist = vec![i64::MAX; self.len()];
        let mut heap = BinaryHeap::new(); // max-heap -> Reverse donne un min-heap
        dist[src] = 0;
        heap.push(std::cmp::Reverse((0_i64, src)));
        while let Some(std::cmp::Reverse((d, u))) = heap.pop() {
            if d != dist[u] {
                continue; // entrée obsolète
            }
            for &(v, w) in &self.adj[u] {
                if d + w < dist[v] {
                    dist[v] = d + w;
                    heap.push(std::cmp::Reverse((dist[v], v)));
                }
            }
        }
        Ok(dist)
    }

    /// `heuristic[v]` doit être une estimation admissible de `v -> goal`.
    pub fn astar(&self, src: usize, goal: usize, heuristic: Option<&[i64]>) -> Result<Vec<i64>, DsaError> {
        if src >= self.len() || goal >= self.len() {
            return Err(DsaError::InvalidVertex {
                vertex: if src >= self.len() { src } else { goal },
                len: self.len(),
            });
        }
        let h = |v: usize| -> i64 { heuristic.map_or(0, |values| values[v]) };
        let mut dist = vec![i64::MAX; self.len()];
        let mut closed = vec![false; self.len()];
        let mut heap = BinaryHeap::new();
        dist[src] = 0;
        heap.push(std::cmp::Reverse((h(src), src)));
        while let Some(std::cmp::Reverse((_, u))) = heap.pop() {
            if closed[u] {
                continue;
            }
            if u == goal {
                break;
            }
            closed[u] = true;
            for &(v, w) in &self.adj[u] {
                if closed[v] {
                    continue;
                }
                let nd = dist[u] + w;
                if nd < dist[v] {
                    dist[v] = nd;
                    heap.push(std::cmp::Reverse((nd + h(v), v)));
                }
            }
        }
        Ok(dist)
    }

    /// Ordre topologique (Kahn). `None` si le graphe contient un cycle.
    pub fn topological_sort(&self) -> Option<Vec<usize>> {
        let mut indegree = vec![0_usize; self.len()];
        for edges in &self.adj {
            for &(v, _) in edges {
                indegree[v] += 1;
            }
        }
        let mut queue: VecDeque<usize> = (0..self.len()).filter(|&i| indegree[i] == 0).collect();
        let mut order = Vec::with_capacity(self.len());
        while let Some(u) = queue.pop_front() {
            order.push(u);
            for &(v, _) in &self.adj[u] {
                indegree[v] -= 1;
                if indegree[v] == 0 {
                    queue.push_back(v);
                }
            }
        }
        (order.len() == self.len()).then_some(order)
    }

    /// Tarjan : `(composante_de[i], nombre_de_composantes)`.
    pub fn scc(&self) -> (Vec<usize>, usize) {
        let mut state = TarjanState {
            index: vec![-1; self.len()],
            low: vec![0; self.len()],
            on_stack: vec![false; self.len()],
            stack: Vec::new(),
            comp: vec![0; self.len()],
            timer: 0,
            comps: 0,
        };
        for u in 0..self.len() {
            if state.index[u] < 0 {
                self.tarjan_visit(u, &mut state);
            }
        }
        (state.comp, state.comps)
    }

    fn tarjan_visit(&self, u: usize, s: &mut TarjanState) {
        s.index[u] = s.timer;
        s.low[u] = s.timer;
        s.timer += 1;
        s.stack.push(u);
        s.on_stack[u] = true;
        for &(v, _) in &self.adj[u] {
            if s.index[v] < 0 {
                self.tarjan_visit(v, s);
                s.low[u] = min(s.low[u], s.low[v]);
            } else if s.on_stack[v] {
                s.low[u] = min(s.low[u], s.index[v]);
            }
        }
        if s.low[u] == s.index[u] {
            loop {
                let v = s.stack.pop().expect("pile SCC non vide");
                s.on_stack[v] = false;
                s.comp[v] = s.comps;
                if v == u {
                    break;
                }
            }
            s.comps += 1;
        }
    }
}

struct TarjanState {
    index: Vec<i64>,
    low: Vec<i64>,
    on_stack: Vec<bool>,
    stack: Vec<usize>,
    comp: Vec<usize>,
    timer: i64,
    comps: usize,
}

/* ------------------------------------------------------------------------ */
/* Fenwick & Segment tree                                                    */
/* ------------------------------------------------------------------------ */

#[derive(Debug, Clone)]
pub struct Fenwick {
    bit: Vec<i64>,
}

impl Fenwick {
    pub fn new(n: usize) -> Self {
        Self { bit: vec![0; n + 1] }
    }

    pub fn add(&mut self, i: usize, delta: i64) {
        let mut p = i + 1;
        while p < self.bit.len() {
            self.bit[p] += delta;
            p += p & (!p + 1);
        }
    }

    pub fn prefix_sum(&self, i: usize) -> i64 {
        let mut p = i + 1;
        let mut s = 0;
        while p > 0 {
            s += self.bit[p];
            p -= p & (!p + 1);
        }
        s
    }

    pub fn range_sum(&self, l: usize, r: usize) -> i64 {
        if l == 0 {
            self.prefix_sum(r - 1)
        } else {
            self.prefix_sum(r - 1) - self.prefix_sum(l - 1)
        }
    }
}

#[derive(Debug, Clone)]
pub struct SegmentTree {
    size: usize,
    tree: Vec<i64>,
}

impl SegmentTree {
    pub fn new(values: &[i64]) -> Self {
        let mut size = 1;
        while size < values.len().max(1) {
            size *= 2;
        }
        let mut tree = vec![0_i64; 2 * size];
        for (i, &v) in values.iter().enumerate() {
            tree[size + i] = v;
        }
        for i in (1..size).rev() {
            tree[i] = tree[2 * i] + tree[2 * i + 1];
        }
        Self { size, tree }
    }

    pub fn add(&mut self, i: usize, delta: i64) {
        let mut p = self.size + i;
        while p > 0 {
            self.tree[p] += delta;
            p /= 2;
        }
    }

    /// Somme sur l'intervalle `[l, r)`.
    pub fn query(&self, l: usize, r: usize) -> i64 {
        let (mut l, mut r) = (l + self.size, r + self.size);
        let mut result = 0;
        while l < r {
            if l & 1 == 1 {
                result += self.tree[l];
                l += 1;
            }
            if r & 1 == 1 {
                r -= 1;
                result += self.tree[r];
            }
            l /= 2;
            r /= 2;
        }
        result
    }
}

/* ------------------------------------------------------------------------ */
/* Mathématiques modulaires                                                  */
/* ------------------------------------------------------------------------ */

pub fn gcd(mut a: i64, mut b: i64) -> i64 {
    a = a.abs();
    b = b.abs();
    while b != 0 {
        (a, b) = (b, a % b);
    }
    a
}

pub fn lcm(a: i64, b: i64) -> i64 {
    a / gcd(a, b) * b
}

pub fn pow_mod(mut base: i64, mut exp: i64, modulo: i64) -> i64 {
    let mut result = 1 % modulo;
    base %= modulo;
    while exp > 0 {
        if exp & 1 == 1 {
            result = (result as i128 * base as i128 % modulo as i128) as i64;
        }
        base = (base as i128 * base as i128 % modulo as i128) as i64;
        exp >>= 1;
    }
    result
}

/// `modulo` premier et `a != 0 mod modulo`.
pub fn mod_inverse(a: i64, modulo: i64) -> i64 {
    pow_mod(a, modulo - 2, modulo)
}

/// `prime[i]` indique si `i` est premier.
pub fn sieve_primes(n: usize) -> Vec<bool> {
    let mut prime = vec![true; n + 1];
    prime[0] = false;
    if n >= 1 {
        prime[1] = false;
    }
    let mut p = 2;
    while p * p <= n {
        if prime[p] {
            let mut k = p * p;
            while k <= n {
                prime[k] = false;
                k += p;
            }
        }
        p += 1;
    }
    prime
}

/* ------------------------------------------------------------------------ */
/* Géométrie                                                                 */
/* ------------------------------------------------------------------------ */

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Point {
    pub x: f64,
    pub y: f64,
}

impl Point {
    pub const fn new(x: f64, y: f64) -> Self {
        Self { x, y }
    }
}

impl std::ops::Add for Point {
    type Output = Self;
    fn add(self, rhs: Self) -> Self {
        Self::new(self.x + rhs.x, self.y + rhs.y)
    }
}

impl std::ops::Sub for Point {
    type Output = Self;
    fn sub(self, rhs: Self) -> Self {
        Self::new(self.x - rhs.x, self.y - rhs.y)
    }
}

impl std::ops::Mul<f64> for Point {
    type Output = Self;
    fn mul(self, k: f64) -> Self {
        Self::new(self.x * k, self.y * k)
    }
}

pub fn dot(a: Point, b: Point) -> f64 {
    a.x * b.x + a.y * b.y
}

pub fn cross(a: Point, b: Point) -> f64 {
    a.x * b.y - a.y * b.x
}

pub fn dist2(a: Point, b: Point) -> f64 {
    let d = a - b;
    dot(d, d)
}

/// Signe de l'orientation de `(a, b, c)` : > 0 à gauche, < 0 à droite.
pub fn orient(a: Point, b: Point, c: Point) -> f64 {
    cross(b - a, c - a)
}

pub fn segments_intersect(a: Point, b: Point, c: Point, d: Point) -> bool {
    let o1 = orient(a, b, c);
    let o2 = orient(a, b, d);
    let o3 = orient(c, d, a);
    let o4 = orient(c, d, b);
    ((o1 > 0.0) != (o2 > 0.0)) && ((o3 > 0.0) != (o4 > 0.0))
}

/* ------------------------------------------------------------------------ */
/* Trie                                                                      */
/* ------------------------------------------------------------------------ */

#[derive(Debug, Clone, Default)]
pub struct Trie {
    nodes: Vec<TrieNode>,
}

#[derive(Debug, Clone, Default)]
struct TrieNode {
    next: [Option<usize>; 26],
    count: usize,
}

impl Trie {
    pub fn new() -> Self {
        Self { nodes: vec![TrieNode::default()] }
    }

    pub fn insert(&mut self, word: &str) -> Result<(), DsaError> {
        let mut cur = 0;
        for c in word.chars() {
            let i = match c {
                'a'..='z' => (c as u8 - b'a') as usize,
                other => return Err(DsaError::InvalidCharacter(other)),
            };
            if self.nodes[cur].next[i].is_none() {
                let new_id = self.nodes.len();
                self.nodes[cur].next[i] = Some(new_id);
                self.nodes.push(TrieNode::default());
            }
            cur = self.nodes[cur].next[i].expect("noeud cree juste au-dessus");
            self.nodes[cur].count += 1;
        }
        Ok(())
    }

    pub fn contains(&self, word: &str) -> bool {
        self.descend(word).is_some_and(|node| node.count > 0)
    }

    pub fn count_with_prefix(&self, prefix: &str) -> usize {
        self.descend(prefix).map_or(0, |node| node.count)
    }

    fn descend(&self, text: &str) -> Option<&TrieNode> {
        let mut cur = 0;
        for c in text.chars() {
            let i = (c as u8).checked_sub(b'a')? as usize;
            if i >= 26 {
                return None;
            }
            cur = self.nodes[cur].next[i]?;
        }
        self.nodes.get(cur)
    }
}

/* `HashMap` reste la table de hachage idiomatique : aucun wrapper nécessaire.
 * Exemple : `let mut counts: HashMap<&str, i64> = HashMap::new();`
 *           *counts.entry("cle").or_insert(0) += 1;`
 */
pub type CountMap<K> = HashMap<K, i64>;

/* ------------------------------------------------------------------------ */
/* Tests                                                                     */
/* ------------------------------------------------------------------------ */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn union_find() {
        let mut dsu = Dsu::new(5);
        assert!(dsu.union(0, 1));
        assert!(dsu.union(1, 2));
        assert!(!dsu.union(2, 0));
        assert!(dsu.connected(0, 2));
        assert_eq!(dsu.count(), 3);
    }

    #[test]
    fn graph_traversals() {
        let mut g = Graph::new(4);
        g.add_undirected(0, 1, 1).unwrap();
        g.add_undirected(1, 2, 2).unwrap();
        g.add_undirected(2, 3, 1).unwrap();
        assert_eq!(g.bfs_distances(0).unwrap(), vec![0, 1, 2, 3]);
        assert_eq!(g.dijkstra(0).unwrap(), vec![0, 1, 3, 4]);
        assert_eq!(g.astar(0, 3, None).unwrap()[3], 4);
        assert!(g.topological_sort().is_none()); // cycle
        assert_eq!(g.add_edge(0, 9, 1), Err(DsaError::InvalidVertex { vertex: 9, len: 4 }));
    }

    #[test]
    fn topological_and_scc() {
        let mut g = Graph::new(3);
        g.add_edge(0, 1, 1).unwrap();
        g.add_edge(1, 2, 1).unwrap();
        assert_eq!(g.topological_sort(), Some(vec![0, 1, 2]));

        let mut cyclic = Graph::new(3);
        cyclic.add_undirected(0, 1, 1).unwrap();
        cyclic.add_edge(2, 0, 1).unwrap();
        let (comp, count) = cyclic.scc();
        assert_eq!(count, 2);
        assert_eq!(comp[0], comp[1]);
        assert_ne!(comp[0], comp[2]);
    }

    #[test]
    fn fenwick_and_segment_tree() {
        let mut fw = Fenwick::new(5);
        fw.add(2, 7);
        assert_eq!(fw.range_sum(0, 3), 7);

        let mut st = SegmentTree::new(&[1, 2, 3, 4, 5]);
        assert_eq!(st.query(1, 4), 9);
        st.add(1, 1);
        assert_eq!(st.query(1, 4), 10);
    }

    #[test]
    fn math_and_geometry() {
        assert_eq!(gcd(12, 18), 6);
        assert_eq!(lcm(4, 6), 12);
        assert_eq!(pow_mod(2, 10, 1000), 24);
        assert_eq!(3 * mod_inverse(3, 1_000_000_007) % 1_000_000_007, 1);
        assert_eq!(sieve_primes(10), vec![false, false, true, true, false, true, false, true, false, false, false]);

        let a = Point::new(0.0, 0.0);
        let b = Point::new(2.0, 2.0);
        let c = Point::new(0.0, 2.0);
        let d = Point::new(2.0, 0.0);
        assert_eq!(dist2(a, b), 8.0);
        assert!(segments_intersect(a, b, c, d));
    }

    #[test]
    fn trie_operations() {
        let mut trie = Trie::new();
        trie.insert("chat").unwrap();
        trie.insert("chien").unwrap();
        assert!(trie.contains("chat"));
        assert!(!trie.contains("chats"));
        assert_eq!(trie.count_with_prefix("ch"), 2);
        assert_eq!(trie.insert("CHAT"), Err(DsaError::InvalidCharacter('C')));
    }
}
