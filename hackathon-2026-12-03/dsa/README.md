# Socle DSA commun — Phase 1 ✅

> **Miroir du superviseur.** La copie de travail canonique est
> `codingGame/dsa/` (à la racine du dépôt). Toute modification doit y être
> faite puis recopiée ici.

Base réutilisable pour toutes les implémentations de challenges. Les mêmes
structures sont disponibles dans les 4 langages, avec le même niveau de
qualité et chacun selon ses conventions idiomatiques (pas de traduction mot à mot).

Recherche préalable : [`research.md`](research.md).

## Structures couvertes

| Structure | C | C++ | Rust | Python |
|---|---|---|---|---|
| Union-Find (DSU) | `Dsu`, `dsu_union` | `dsa::Dsu` | `Dsu` | `Dsu` |
| Graphe pondéré | `Graph`, `graph_add_edge` | `dsa::WeightedGraph` | `Graph` | `Graph` |
| BFS / DFS | `bfs_distances`, `dfs_from` | `bfs_distances`, `dfs` | `bfs_distances`, `dfs` | `bfs_distances`, `dfs` |
| Dijkstra | `dijkstra` | `dijkstra` | `dijkstra` | `dijkstra` |
| A* | `astar` | `astar` | `astar` | `astar` |
| Tri topologique (Kahn) | `topological_sort` | `topological_sort` | `topological_sort` | `topological_sort` |
| SCC (Tarjan) | `strongly_connected_components` | `strongly_connected_components` | `scc` | `strongly_connected_components` |
| Min-heap | `MinHeap` (tableau manuel) | `std::priority_queue` | `BinaryHeap` + `Reverse` | `heapq` |
| Fenwick | `Fenwick` | `dsa::Fenwick` | `Fenwick` | `Fenwick` |
| Segment tree (sommes) | `SegmentTree` | `dsa::SegmentTree` | `SegmentTree` | `SegmentTree` |
| Math modulaire | `gcd_ll`, `pow_mod`, `mod_inverse`, `sieve_primes` | `pow_mod`, `mod_inverse`, `sieve_primes` | `gcd`, `lcm`, `pow_mod`, `mod_inverse`, `sieve_primes` | `pow_mod`, `mod_inverse`, `sieve_primes`, `lcm` |
| Géométrie | `Point`, `point_cross`, `segments_intersect` | `Point` + opérateurs | `Point` + traits `Add`/`Sub`/`Mul` | `Point` (dataclass gelée) |
| Trie | `Trie` | `dsa::Trie` | `Trie` | `Trie` |
| Recherche binaire | `lower_bound_i`, `upper_bound_i` | `std::lower_bound` | `slice::binary_search` | `bisect` |
| Table de hachage | `StringMap` (open addressing) | `std::unordered_map` | `std::collections::HashMap` (`CountMap`) | `dict` / `Counter` / `defaultdict` |
| Entrées rapides | `scanf` / `fgets` | `cin` (`sync_with_stdio(false)`) | `stdin().read_to_string` | `read_ints()` |

## Conventions par langue

- **C** — `snake_case`, header unique `dsa.h`, `static` functions, structs +
  fonctions, allocation explicite, chaque `*_init` a son `*_free`, codes
  d’erreur `bool` ou `NULL`. Compile avec `gcc -O2 -std=c11`.
- **C++** — `namespace dsa`, STL, RAII, `const`, templates et `std::optional`,
  types forts (`std::size_t`, `std::int64_t`). Compile avec `g++ -O2 -std=c++20`.
- **Rust** — ownership explicite, `Result<DsaError>` pour les données invalides,
  `Option` pour les résultats absents, aucun `unwrap` dans la logique métier,
  6 tests unitaires `#[cfg(test)]`, `Cargo.toml` sans dépendance.
- **Python** — PEP 8, `typing`, `dataclass`, doctests, `if __name__ == "__main__"`,
  bibliothèque standard uniquement.

## Fichiers

```
dsa/
├── README.md              ← ce fichier
├── research.md            ← sources et choix de conception
├── run_tests.sh           ← lance les 4 suites de tests
├── C/dsa.h                ← socle header-only
├── C/dsa_selftest.c
├── CPP/dsa.hpp            ← socle header-only
├── CPP/dsa_selftest.cpp
├── Rust/Cargo.toml
├── Rust/src/lib.rs
└── Python/dsa.py
```

## Utilisation

```sh
./run_tests.sh
```

Résultat attendu :

```
dsa C : OK
dsa C++ : OK
dsa Python : OK
dsa Rust : 6 tests passed
```

Pour copier le socle dans un binder CodinGame :
- C/C++ : inclure le header unique (aucun éditeur de liens supplémentaire).
- Rust : coller `src/lib.rs` dans le fichier principal, ajouter `use ...`.
- Python : coller `dsa.py` ou importer le module.

## Limites connues

- Dijkstra/A* rejettent les poids négatifs (documenté dans chaque langue).
- Le trie n’accepte que les minuscules `a-z`.
- Le segment tree implémenté est une somme ; un min/max se dérive en changeant
  l’opérateur de fusion (à faire au cas par cas en Phase 2).
- Le inverse modulaire suppose un modulo premier.
- Tarjan est récursif : Python élève `sys.setrecursionlimit` en conséquence.
