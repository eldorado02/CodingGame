# Phase 1 — Recherche des références DSA (20 novembre 2026)

Recherche faite localement avec `webfetch` (l’outil `websearch` renvoie 402), sans
sous-agent, le quota gratuit OpenCode étant épuisé pour toute nouvelle session.

## a) Sources retenues

| Source | Langue(s) | Licence (vérifiée) | Ce qu’on en tire |
|---|---|---|---|
| [krahets/hello-algo](https://github.com/krahets/hello-algo) — [README](https://raw.githubusercontent.com/krahets/hello-algo/main/README.md) | C, C++, Python, Java, Others | **CC BY-NC-SA 4.0** ([LICENSE](https://raw.githubusercontent.com/krahets/hello-algo/main/LICENSE), vérifié) | Référence pédagogique multilingue, structures expliquées et exemples exécutables. ⚠️ Licence **non commerciale** : on s’inspire de la *structure pédagogique*, on ne copie pas le code. |
| [cheran-senthil/PyRival](https://github.com/cheran-senthil/PyRival) — [README](https://raw.githubusercontent.com/cheran-senthil/PyRival/master/README.md), [template Py3](https://raw.githubusercontent.com/cheran-senthil/PyRival/master/templates/template_py3.py) | Python | **Apache 2.0** ([LICENSE](https://raw.githubusercontent.com/cheran-senthil/PyRival/master/LICENSE), vérifié) | Template de fast I/O (`os.read` + `BytesIO`) et snippets (algebra, graphs, strings, data structures). On reprend l’**idée** d’un template d’entrée rapide, réécrit. |
| [7oSkaaa/CP-Templates](https://github.com/7oSkaaa/CP-Templates) — [README](https://raw.githubusercontent.com/7oSkaaa/CP-Templates/main/README.md) | C++17 | Non vérifiée (fichier LICENSE absent du chemin testé) | 57 algorithmes « battle-tested », organisation par chapitres. Sert de **check-list de couverture**, pas de source de code. |
| [kenkoooo/competitive-programming-rs](https://github.com/kenkoooo/competitive-programming-rs) — [page lib.rs](https://lib.rs/crates/competitive-programming-rs) | Rust | « Custom license » (déclaré sur lib.rs, non vérifié en détail) | Bibliothèque Rust de CP (graphes, géométrie, mentions `graph`/`geometry` sur lib.rs). Sert de référence d’**API idiomatique** (`BinaryHeap`, `Reverse`, traits). |
| [Rajveer100/Rust-CP-Templates](https://github.com/Rajveer100/Rust-CP-Templates) (trouvé via [DuckDuckGo lite](https://lite.duckduckgo.com/lite/?q=rust+competitive+programming+template+github+library+graph+segment+tree)) | Rust | Non vérifiée (README inaccessible aux chemins `main`/`master` testés) | Candidat de templates Rust ; **non vérifié**, cité seulement comme piste. |

**Décision licence :** aucun code n’est copié depuis ces dépôts. Le socle ci-dessous est
écrit from scratch ; les sources servent uniquement à choisir la couverture et le style.
hello-algo étant CC BY-NC-SA, citer le dépôt dans la documentation suffit amplement.

## b) Liste commune priorisée (implémentée dans les 4 langages)

| Priorité | Structure / algorithme | Justification CodinGame |
|---|---|---|
| 1 | **Union-Find (DSU)** — compression + union par taille | Composantes connexes, kruskal, « fusions » de groupes : récurrent dans les challenges. |
| 2 | **BFS / DFS sur graphe** (liste d’adjacence) | Plus court chemin en nombre d’arêtes, exploration d’état, détection de cycles. |
| 3 | **Dijkstra** (file de priorité) | Plus court chemin pondéré, dès que les coûts ne sont pas uniformes. |
| 4 | **Min-heap / file de priorité** | File d’événements, top-K, Dijkstra, A*. |
| 5 | **Fenwick (BIT)** | Préfixes cumulatifs dynamiques, inversion counts, budgets. |
| 6 | **Segment tree** (somme/min, update ponctuel) | Requêtes sur intervalles avec mises à jour ; plus général que Fenwick. |
| 7 | **Tri topologique (Kahn)** | Ordonnancement de dépendances, parties « construction » de puzzle. |
| 8 | **SCC (Tarjan)** | Graphes dirigés, conditions de victoire, regroupement de states. |
| 9 | **A*** (heuristique) | Déplacement sur grille/ carte avec heuristique admissible. |
| 10 | **Math modulaire** — pgcd/ppcm, `pow_mod`, inverse modulaire, crible | Comptage, combinatorics, nombres premiers. |
| 11 | **Géométrie de base** — Point, distance², dot/cross, orientation, intersection de segments | Déplacement, collision, visibilité (très fréquent en bot programming). |
| 12 | **Trie** (insert / search / prefix count) | Mots, préfixes, autocomplete, codes. |
| 13 | **Recherche binaire** (`lower_bound`, `upper_bound`) | Arbitrage de seuils, interpolation. |
| 14 | **Table de hachage** (chaînage ou open addressing en C) | Compteurs, memoïsation, visite d’états. |

Non inclus volontairement (trop spécialisé ou déjà dans la STL) : flux réseau,
matchings, polynômes/NTT, structures de données avancées (treap, LCT). À ajouter
au cas par cas pendant la Phase 2.

## c) Approche idiomatique par langue

| Structure | C | C++ | Rust | Python |
|---|---|---|---|---|
| Graphe | `size_t*` + `int**` voisins, `graph_free()` | `vector<vector<int>>` | `Vec<Vec<usize>>` | `list[list[int]]` |
| BFS / DFS | file circulaire `size_t*` ; récursion | `std::queue`, `std::stack` | `VecDeque` | `collections.deque` |
| Dijkstra / A* | min-heap manuel (tableau de paires) | `priority_queue<T, vector<T>, greater<T>>` | `BinaryHeap<Reverse<(cost,node)>>` | `heapq` (min-heap natif) |
| Union-Find | `int* parent`, `int* size` | `vector<size_t>`, `iota` | `Vec<usize>` derrière un `struct` | `list[int]` dans une `@dataclass` |
| Fenwick / Segment | tableaux `long long*` alloués | `vector<long long>` | `Vec<i64>` | `list[int]` |
| Tri topo | files + compteurs de degrés | `std::queue`, `vector<int>` | `VecDeque`, `Vec<usize>` | `deque` |
| SCC | tableaux `index/low/depth` + pile | `vector` + récursion lambda | `Vec` + fonction récursive | `list` + récursion |
| Modulaire | `long long` + multiplication contrôlée | `long long`, `constexpr` MOD | `i128` pour éviter l’overflow | `int` (arbitraire) |
| Géométrie | `struct { double x, y; }` | `struct Point` + opérateurs | `#[derive(Clone, Copy)] struct Point` | `@dataclass(frozen=True) class Point` |
| Trie | nœuds alloués, `trie_free()` | `vector<array<int,26>>` | `Vec<[Option<usize>; 26]>` | `dict` imbriqués |
| Table de hachage | open addressing maison (clé chaîne → int) | `unordered_map` | `HashMap` | `dict` |

## d) Recommandation finale

Implémenter **d’abord le bloc 1–6** (DSU, BFS/DFS, Dijkstra, heap, Fenwick,
segment tree) : il couvre la majorité des challenges algorithmiques et sert de
base aux autres. Puis le bloc 7–10 (topo, SCC, A*, math), puis géométrie/trie/
recherche binaire/hash. Estimation : une session de travail pour écrire les 4
langages, plus une passe de compilation/tests. Chaque fichier est autonome afin
d’être copiable dans un binder CodinGame sans gestion de paquets.

## Vérification

Les URLs ci-dessus ont été ouvertes le 20/11/2026. Deux dépôts (Rust-CP-Templates,
CP-Templates LICENSE) n’ont pas pu être vérifiés plus avant ; ils sont marqués
comme tels et n’influencent pas le code produit.
