# Critique — Smash The Code

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Règles de score exactes inconnues en Wood | Moyen | On optimise un proxy (blocs effacés + chaînes) ; le score réel ne sert qu’au départage après 200 tours. |
| Crânes mal gérés | Élevé | BFS ignore `0` ; les crânes adjacents à un groupe effacé sont retirés dans la même passe. |
| Top out | Élevé | Coup illégal si `r-2 < 0` ; pénalité de hauteur sur les coups sans clear. |
| Timeout 100 ms | Faible | 6 × O(72) avec BFS → < 1 ms même en Python. |
| Divergence entre langages | Moyen | Même algorithme, mêmes cas de test, comparaison automatisée. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Test différentiel : 200 grilles aléatoires générées avec `random.seed(42)` →
  **0 divergence** entre C, C++, Rust et Python.
- Bugs réels trouvés et corrigés pendant la validation :
  - C/C++ : condition inversée dans `drop` (première case vide au lieu de la
    première case occupée) → coups illégaux partout.
  - Rust : `max_by_key` départageait vers la droite ; remplacé par `>` explicite.
  - Rust : gravité collectait bas→haut mais réécrivait haut→bas → ordre des
    blocs inversé, faux chains.
  - Python : parsing des lignes de grille (1 token = 1 ligne, pas 6 cases) ;
    crânes adjacents marqués depuis le mauvais tableau.
- Protocole CodinGame (découvert via un test game réel, pas visible dans les
  cas locaux) :
  - le nombre de paires de couleurs est **variable** (8 puis les restantes) ;
    la ligne de score est celle qui ne contient qu’un seul entier ;
  - le programme doit **boucler** : un tour = une entrée complète + une
    sortie, sinon timeout dès le tour suivant ;
  - la lecture globale (`read()` / `read_to_string()`) bloque car CodinGame
    envoie les tours progressivement : lecture ligne par ligne obligatoire.
  - Après correction : 2 test games réels gagnés contre CocoBoss
    (680–0 puis 1320–0).
- Conclusion : les 4 solutions sont fonctionnellement équivalentes et
  idiomatiques. Le proxy de score reste une heuristique Wood ; les leagues
  supérieures demanderont d’affiner nuisance/score réels.
