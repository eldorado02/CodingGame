Tu es le sous-agent d'IMPLÉMENTATION de la Phase 1 : construire le SOCLE DSA COMMUN réutilisable pour tous les challenges du hackathon. Réponds et commente en FRANÇAIS.

BASE : lis d'abord /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/research.md (recherche Phase 1) et /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/README.md.

À FAIRE : implémenter les MÊMES structures dans les 4 langages, avec le même niveau de qualité, chacun selon ses conventions idiomatiques (PAS de traduction mot à mot) :
- C : `snake_case`, structs + fonctions, codes d'erreur explicites, allocation documentée, en-tête unique.
- C++ : STL, RAII, `const`, templates, types forts.
- Rust : ownership/borrowing, `Result`/`Option`, erreurs typées, pas d'`unwrap` dans la logique métier, tests `#[cfg(test)]`, Cargo.toml.
- Python : PEP 8, `typing`, `dataclass`, entrées/sorties dans `main()`, doctests ou tests.

Couvrir au minimum (adapter selon research.md) : graphes (BFS/DFS, Dijkstra, A*, ordre topologique, SCC), union-find, file de priorité / min-heap, segment tree + Fenwick, trie, math modulaire (pgcd, exponentiation rapide, inverse modulaire), géométrie de base (distance, angle, intersection), DP utiles, tri/recherche, table de hachage.

FICHIERS À ÉCRIRE (chemins absolus) :
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/README.md (mettre à jour : structures couvertes, tableau de correspondance, conventions, sources)
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/C/dsa.h
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/CPP/dsa.hpp
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/Rust/src/lib.rs (et Cargo.toml si besoin)
- /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/Python/dsa.py

Chaque implémentation doit être autonome, compilable/importable et contenir un petit test ou exemple d'usage. Ne modifie aucun autre fichier. Ne lance pas d'autre sous-agent.
