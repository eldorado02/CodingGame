# Critique — Xmas Rush

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Processus tué après le premier tour | Élevé | Boucle `while` sur les tours ; stdin reste ouvert. |
| Lecture bloquante (`read()` global) | Élevé | `readline()` / `getline()` exacts par tour. |
| Item sur tuile joueur (`-1`) | Moyen | Poussée = insertion sur la case d'entrée. |
| Item éjecté (`pop`) | Moyen | Revient sur la tuile du joueur. |
| Déplacement sans connexion des deux tuiles | Élevé | BFS vérifie `current[d]` **et** `next[opp(d)]`. |
| Poussée simultanée même ligne | Faible | Non modélisée (adversaire inconnu). |
| Divergence 4 langages | Moyen | Départage `(distance, ligne, direction)` identique. |

## Après implémentation

- `tests/run.sh` : 10 cas × 4 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 4 tours → **0 divergence**.
- Deux bugs corrigés en chemin :
  - `sys.stdin.read()` bloquait sur stdin non fermé → passage aux lignes ;
  - le bot quittait après un tour → timeout dès le tour suivant ; boucle ajoutée.
- Validation officielle : **4 test games gagnés** contre St Nicolas (boss Wood 2,
  agent `2194292`), M Qasim Aslam (rang 1), aymané212 (rang 2) et vexilla72
  (rang 3) ; scores `0=1 1=0` à chaque fois.
- Le port C++ a également gagné contre le boss (`0=1 1=0`).
- Conclusion : un BFS court + une poussée greedy suffisent en Wood. Le vrai
  gain viendrait d'une évaluation à 2 coups (poussée adverse puis la nôtre),
  mais la complexité de portage est élevée pour un gain marginal à ce niveau.
