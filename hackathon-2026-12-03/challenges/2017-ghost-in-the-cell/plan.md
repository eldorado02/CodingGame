# Plan — Ghost in the Cell

## Algorithme commun

Init : `factoryCount`, `linkCount`, puis `linkCount` lignes `f1 f2 dist`.
Floyd-Warshall → `dist[i][j]` (INF si pas de lien direct).

### Par tour

1. Lire `entityCount`, puis les entités :
   - `FACTORY id owner cyborgs prod _ _`
   - `TROOP id owner src dst count eta`
2. `force[i]` = cyborgs de l’usine i (owner connaissable) ; pour chaque troupe :
   `force[dst] += count` si elle est à nous, `force[dst] -= count` sinon.
   Usines sans entité = 0.
3. Défense : pour chaque usine `i` à nous avec `force[i] < 0`, chercher une
   usine `j` à nous avec `force[j] > -force[i] + 5` ; envoyer
   `min(force[j]-5, -force[i]+2)` vers i. Garder le meilleur (force la plus
   négative).
4. Expansion : pour chaque usine `src` à nous (force > 5) et chaque cible
   `dst` neutre ou ennemie :
   - `need = (owner[dst] == 0 ? force[dst] : force[dst]) + 1`
   - si `force[src] - 5 >= need`, envoyer `need`.
   - score : `100` si neutre + `prod[dst]*20` - `dist[src][dst]*2` - `need`.
5. Output le meilleur `MOVE src dst n`, sinon `WAIT`, `flush`.

### Cas de test

- `01_two.in` : 2 usines, neutre proche → MOVE capture.
- `02_defense.in` : troupe ennemie arrive → renfort.
- `03_wait.in` : aucune cible abordable → WAIT.
- `04_attack.in` : usine ennemie faible → attaque.
- `05_multi.in` : 4 usines, choix du meilleur couple.
- `06_no_route.in` : graphe déconnecté → WAIT.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
