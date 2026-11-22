# Othello — recherche

## Sources

- Énoncé officiel via `cg.py show othello-1` → `statement.md`.
- Referee public : `github.com/MultiStruct/Othello` (`Referee.java`,
  `Board.java`, `Cell.java`, `config/Boss.java`).
- Le boss Wood 2 (`6797560`) joue aléatoirement (seed 13). Il active EXPERT
  pour recevoir l'historique, mais notre bot ne le fait pas : une seule ligne
  d'action par tour.
- Aucun post-mortem exploitable trouvé ; la mécanique du referee et les test
  games restent la source primitive.

## Faits mécaniques retenus

- Plateau 8×8, noir (0) commence. Action `a1`..`h8` (`(97+x)` puis `(y+1)`).
- Passage automatique si aucune action légale ; fin si aucun joueur n'a de coup.
- Timeout ou action invalide = défaite immédiate.
- `id` et `boardSize` ne sont envoyés que lors des 3 premiers tours
  (`turn <= 2` dans `Referee.java`) : le parseur doit être tolérant.
- 2 000 ms au premier tour, 150 ms par tour ensuite.

## Hypothèses validées

- Ancien moteur tableau : évaluation « position + mobilité + frontier », budget
  temps 115 ms **avec élagage alpha-bêta à la racine** → 9V/1D/10. Sans élagage
  à la racine, 3–4 plis seulement ; avec budgets de nœuds fixes, timeout.
- Nouveau moteur bitboard : mêmes poids d'évaluation, recherche à profondeur
  fixe 6 avec ordre de coups par table positionnelle. 10 000 positions aléatoires
  validées contre l'implémentation tableau (0 écart `moves`/`flips`), max
  34,9 ms sur les 200 cas de fuzz, **5V/5** contre `6797560`.
- La forme correcte de la dilatation est `t = sh(P)&O` puis 6 étages de
  `t |= sh(t)&O`, et non l'inverse.
