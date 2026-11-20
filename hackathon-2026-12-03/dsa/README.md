# Socle DSA commun — Phase 1

Base réutilisable pour toutes les implémentations de challenges. Mêmes structures
dans les 4 langages, même niveau de qualité.

| Structure | C | C++ | Rust | Python |
|---|---|---|---|---|
| *(à définir après la recherche Phase 1)* | | | | |

## Conventions par langue

- **C** — `snake_case`, structs + fonctions, codes d’erreur explicites, pas de
  macros opaques, allocation documentée.
- **C++** — `snake_case` ou `camelCase` constant, STL, RAII, `const`, templates
  quand pertinent.
- **Rust** — `snake_case`, ownership explicite, `Result`/`Option`, erreurs typées,
  tests unitaires `#[cfg(test)]`.
- **Python** — PEP 8, `typing`, `dataclass`, entrées/sorties dans `main()`.

## Sources

*(à renseigner après le deep search Phase 1)*
