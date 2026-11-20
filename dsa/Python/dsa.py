"""Socle DSA pour CodinGame (Python 3.11+).

Conventions : PEP 8, typage, ``dataclass``, entrées/sorties regroupées dans
``main()``. Les noms publics sont identiques à ceux des autres langages du
socle (``Dsu``, ``Graph``, ``Fenwick``, ``SegmentTree``, ``Trie``, ...).

Aucune dépendance tierce : la bibliothèque standard suffit.
"""

from __future__ import annotations

import sys
from collections import Counter, defaultdict, deque
from dataclasses import dataclass, field
from heapq import heappop, heappush
from math import gcd as _gcd, isqrt
from typing import Callable, Iterable, Iterator, Optional


# ---------------------------------------------------------------------------
# Entrées rapides
# ---------------------------------------------------------------------------
def read_ints() -> Iterator[int]:
    """Yield tous les entiers de l'entrée standard, séparateurs quelconques."""
    for chunk in sys.stdin.buffer.read().split():
        yield int(chunk)


# ---------------------------------------------------------------------------
# Union-Find
# ---------------------------------------------------------------------------
@dataclass
class Dsu:
    """Union-Find avec compression de chemin et union par rang.

    >>> d = Dsu(5); _ = d.union(0, 1); _ = d.union(1, 2)
    >>> d.connected(0, 2), d.count
    (True, 3)
    """

    parent: list[int] = field(init=False)
    rank: list[int] = field(init=False)
    count: int = field(init=False)

    def __init__(self, n: int) -> None:
        self.parent = list(range(n))
        self.rank = [0] * n
        self.count = n

    def find(self, x: int) -> int:
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, a: int, b: int) -> bool:
        ra, rb = self.find(a), self.find(b)
        if ra == rb:
            return False
        if self.rank[ra] < self.rank[rb]:
            ra, rb = rb, ra
        self.parent[rb] = ra
        if self.rank[ra] == self.rank[rb]:
            self.rank[ra] += 1
        self.count -= 1
        return True

    def connected(self, a: int, b: int) -> bool:
        return self.find(a) == self.find(b)


# ---------------------------------------------------------------------------
# Graphe & parcours
# ---------------------------------------------------------------------------
@dataclass
class Graph:
    """Graphe dirigé pondéré ; ``adj[u] = [(v, poids), ...]``.

    >>> g = Graph(3); g.add_undirected(0, 1, 2); g.bfs_distances(0)
    [0, 1, -1]
    """

    adj: list[list[tuple[int, int]]] = field(default_factory=list)

    def __init__(self, n: int) -> None:
        self.adj = [[] for _ in range(n)]

    def __len__(self) -> int:
        return len(self.adj)

    def add_edge(self, u: int, v: int, weight: int = 1) -> None:
        self.adj[u].append((v, weight))

    def add_undirected(self, u: int, v: int, weight: int = 1) -> None:
        self.add_edge(u, v, weight)
        self.add_edge(v, u, weight)

    def bfs_distances(self, src: int) -> list[int]:
        """Distances en nombre d'arêtes ; ``-1`` si inatteignable."""
        dist = [-1] * len(self)
        queue = deque([src])
        dist[src] = 0
        while queue:
            u = queue.popleft()
            for v, _ in self.adj[u]:
                if dist[v] == -1:
                    dist[v] = dist[u] + 1
                    queue.append(v)
        return dist

    def dfs(self, src: int, visit: Callable[[int], None]) -> None:
        seen = [False] * len(self)

        def recurse(u: int) -> None:
            seen[u] = True
            visit(u)
            for v, _ in self.adj[u]:
                if not seen[v]:
                    recurse(v)

        recurse(src)

    def dijkstra(self, src: int) -> list[int]:
        """Plus courts chemins ; ``sys.maxsize`` si inatteignable.

        >>> g = Graph(3); g.add_undirected(0, 1, 4); g.add_undirected(1, 2, 3)
        >>> g.dijkstra(0)
        [0, 4, 7]
        """
        inf = sys.maxsize
        dist = [inf] * len(self)
        dist[src] = 0
        heap: list[tuple[int, int]] = [(0, src)]
        while heap:
            d, u = heappop(heap)
            if d != dist[u]:
                continue
            for v, w in self.adj[u]:
                if d + w < dist[v]:
                    dist[v] = d + w
                    heappush(heap, (dist[v], v))
        return dist

    def astar(
        self,
        src: int,
        goal: int,
        heuristic: Optional[list[int]] = None,
    ) -> list[int]:
        """``heuristic[v]`` : estimation admissible de ``v -> goal``."""
        inf = sys.maxsize
        dist = [inf] * len(self)
        closed = [False] * len(self)
        h = heuristic or [0] * len(self)
        dist[src] = 0
        heap: list[tuple[int, int]] = [(h[src], src)]
        while heap:
            _, u = heappop(heap)
            if closed[u]:
                continue
            if u == goal:
                break
            closed[u] = True
            for v, w in self.adj[u]:
                if closed[v]:
                    continue
                nd = dist[u] + w
                if nd < dist[v]:
                    dist[v] = nd
                    heappush(heap, (nd + h[v], v))
        return dist

    def topological_sort(self) -> Optional[list[int]]:
        """Ordre topologique (Kahn) ; ``None`` en cas de cycle."""
        indegree = [0] * len(self)
        for edges in self.adj:
            for v, _ in edges:
                indegree[v] += 1
        queue = deque(i for i, d in enumerate(indegree) if d == 0)
        order: list[int] = []
        while queue:
            u = queue.popleft()
            order.append(u)
            for v, _ in self.adj[u]:
                indegree[v] -= 1
                if indegree[v] == 0:
                    queue.append(v)
        return order if len(order) == len(self) else None

    def strongly_connected_components(self) -> tuple[list[int], int]:
        """Tarjan : ``(composante_de[i], nombre_de_composantes)``."""
        sys.setrecursionlimit(max(sys.getrecursionlimit(), len(self) * 2 + 100))
        index = [-1] * len(self)
        low = [0] * len(self)
        on_stack = [False] * len(self)
        stack: list[int] = []
        comp = [0] * len(self)
        counter = 0
        count = 0

        def visit(u: int) -> None:
            nonlocal counter, count
            index[u] = low[u] = counter
            counter += 1
            stack.append(u)
            on_stack[u] = True
            for v, _ in self.adj[u]:
                if index[v] == -1:
                    visit(v)
                    low[u] = min(low[u], low[v])
                elif on_stack[v]:
                    low[u] = min(low[u], index[v])
            if low[u] == index[u]:
                while True:
                    v = stack.pop()
                    on_stack[v] = False
                    comp[v] = count
                    if v == u:
                        break
                count += 1

        for u in range(len(self)):
            if index[u] == -1:
                visit(u)
        return comp, count


# ---------------------------------------------------------------------------
# Fenwick & Segment tree
# ---------------------------------------------------------------------------
class Fenwick:
    """Arbre de sommes cumulées (1-indexé en interne)."""

    def __init__(self, n: int) -> None:
        self.n = n
        self.bit = [0] * (n + 1)

    def add(self, i: int, delta: int) -> None:
        i += 1
        while i <= self.n:
            self.bit[i] += delta
            i += i & -i

    def prefix_sum(self, i: int) -> int:
        total = 0
        i += 1
        while i > 0:
            total += self.bit[i]
            i -= i & -i
        return total

    def range_sum(self, l: int, r: int) -> int:
        """Somme sur ``[l, r)``."""
        return self.prefix_sum(r - 1) - (self.prefix_sum(l - 1) if l else 0)


class SegmentTree:
    """Arbre de sommes, taille interne = prochaine puissance de 2."""

    def __init__(self, values: Iterable[int]) -> None:
        values = list(values)
        self.size = 1
        while self.size < max(len(values), 1):
            self.size *= 2
        self.tree = [0] * (2 * self.size)
        for i, v in enumerate(values):
            self.tree[self.size + i] = v
        for i in range(self.size - 1, 0, -1):
            self.tree[i] = self.tree[2 * i] + self.tree[2 * i + 1]

    def add(self, i: int, delta: int) -> None:
        p = self.size + i
        while p:
            self.tree[p] += delta
            p //= 2

    def query(self, l: int, r: int) -> int:
        """Somme sur ``[l, r)``.

        >>> st = SegmentTree([1, 2, 3, 4, 5]); st.query(1, 4)
        9
        """
        l, r = l + self.size, r + self.size
        result = 0
        while l < r:
            if l & 1:
                result += self.tree[l]
                l += 1
            if r & 1:
                r -= 1
                result += self.tree[r]
            l //= 2
            r //= 2
        return result


# ---------------------------------------------------------------------------
# Mathématiques modulaires
# ---------------------------------------------------------------------------
def lcm(a: int, b: int) -> int:
    return a // _gcd(a, b) * b


def pow_mod(base: int, exp: int, modulo: int) -> int:
    """Exponentiation rapide (équivalent à ``pow(base, exp, modulo)``)."""
    return pow(base, exp, modulo)


def mod_inverse(a: int, modulo: int) -> int:
    """Inverse modulaire ; ``modulo`` premier et ``a`` non nul mod ``modulo``."""
    return pow(a, modulo - 2, modulo)


def sieve_primes(n: int) -> list[bool]:
    """``prime[i]`` indique si ``i`` est premier.

    >>> sieve_primes(10)[2], sieve_primes(10)[9]
    (True, False)
    """
    prime = [True] * (n + 1)
    if n >= 0:
        prime[0] = False
    if n >= 1:
        prime[1] = False
    for p in range(2, isqrt(n) + 1):
        if prime[p]:
            for k in range(p * p, n + 1, p):
                prime[k] = False
    return prime


# ---------------------------------------------------------------------------
# Géométrie
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class Point:
    """Point 2D immuable.

    >>> Point(1, 2) + Point(3, 4)
    Point(x=4, y=6)
    """

    x: float
    y: float

    def __add__(self, other: "Point") -> "Point":
        return Point(self.x + other.x, self.y + other.y)

    def __sub__(self, other: "Point") -> "Point":
        return Point(self.x - other.x, self.y - other.y)

    def __mul__(self, k: float) -> "Point":
        return Point(self.x * k, self.y * k)


def dot(a: Point, b: Point) -> float:
    return a.x * b.x + a.y * b.y


def cross(a: Point, b: Point) -> float:
    return a.x * b.y - a.y * b.x


def dist2(a: Point, b: Point) -> float:
    d = a - b
    return dot(d, d)


def orient(a: Point, b: Point, c: Point) -> float:
    """Signe de l'orientation de ``(a, b, c)``."""
    return cross(b - a, c - a)


def segments_intersect(a: Point, b: Point, c: Point, d: Point) -> bool:
    o1, o2 = orient(a, b, c), orient(a, b, d)
    o3, o4 = orient(c, d, a), orient(c, d, b)
    return (o1 > 0) != (o2 > 0) and (o3 > 0) != (o4 > 0)


# ---------------------------------------------------------------------------
# Trie
# ---------------------------------------------------------------------------
@dataclass
class TrieNode:
    children: dict[str, "TrieNode"] = field(default_factory=dict)
    count: int = 0


@dataclass
class Trie:
    """Trie de mots en minuscules.

    >>> t = Trie(); t.insert("chat"); t.insert("chien")
    >>> t.contains("chat"), t.count_with_prefix("ch")
    (True, 2)
    """

    root: TrieNode = field(default_factory=TrieNode)

    def insert(self, word: str) -> None:
        node = self.root
        for char in word:
            if not "a" <= char <= "z":
                raise ValueError(f"caractère invalide dans le trie : {char!r}")
            node = node.children.setdefault(char, TrieNode())
            node.count += 1

    def contains(self, word: str) -> bool:
        node = self._descend(word)
        return node is not None and node.count > 0

    def count_with_prefix(self, prefix: str) -> int:
        node = self._descend(prefix)
        return node.count if node else 0

    def _descend(self, text: str) -> Optional[TrieNode]:
        node = self.root
        for char in text:
            node = node.children.get(char)  # type: ignore[assignment]
            if node is None:
                return None
        return node


# Table de hachage idiomatique : ``dict`` / ``Counter`` / ``defaultdict``.
# Exemple : ``Counter(mots)``, ``defaultdict(int)``.


def main() -> None:
    """Démonstration rapide, aussi utilisable comme test local."""
    dsu = Dsu(5)
    assert dsu.union(0, 1) and dsu.union(1, 2)
    assert dsu.connected(0, 2) and dsu.count == 3

    graph = Graph(4)
    graph.add_undirected(0, 1, 1)
    graph.add_undirected(1, 2, 2)
    graph.add_undirected(2, 3, 1)
    assert graph.bfs_distances(0) == [0, 1, 2, 3]
    assert graph.dijkstra(0) == [0, 1, 3, 4]
    assert graph.astar(0, 3)[3] == 4
    assert graph.topological_sort() is None  # cycle

    fenwick = Fenwick(5)
    fenwick.add(2, 7)
    assert fenwick.range_sum(0, 3) == 7

    tree = SegmentTree([1, 2, 3, 4, 5])
    assert tree.query(1, 4) == 9
    tree.add(1, 1)
    assert tree.query(1, 4) == 10

    trie = Trie()
    trie.insert("chat")
    trie.insert("chien")
    assert trie.contains("chat") and trie.count_with_prefix("ch") == 2

    counts: Counter[str] = Counter(["a", "b", "a"])
    assert counts["a"] == 2
    print("dsa Python : OK")


if __name__ == "__main__":
    main()
