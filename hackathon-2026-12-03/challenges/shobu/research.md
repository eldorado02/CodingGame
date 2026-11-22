# Shobu — recherche

## Sources

- Énoncé officiel relu après promotion en Wood 3 via `cg.py show shobu` →
  `statement.md`. Le boss Wood 3 est `BOSS 2` (`6906535`).
- Referee public : `github.com/Waffle3z/CG-Shobu`, fichiers
  `src/main/java/com/codingame/game/shobu/Board.java` et
  `src/main/java/com/codingame/game/Referee.java`, consultés via l'API GitHub.
  `Board.legalMoves()` définit l'ordre exact des actions, utilisé comme
  départage dans la recherche.
- La description officielle des ligues indique :
  Wood 4 = greedy, Wood 3 = MCTS avec playouts courts et positions résolues,
  Wood 2 = minimax alpha-beta 3 plis. Le boss ne swap jamais.
- Recherche GitHub/arXiv complémentaire : aucun post-mortem public exploitable
  pour cette variante 4×4×4 avec pie rule ; la mécanique du referee et les logs
  de test games restent la source primitive.

## Faits mécaniques retenus

- Un coup est une paire de déplacements identiques : un sur un plateau
  « home » du joueur, un sur un plateau de l'autre couleur. Le second peut
  pousser exactement un pion adverse d'une case ; hors plateau = éliminé.
- Victoire de manche dès qu'un plateau ne contient plus aucun pion adverse ;
  match en deux manches, 2 points par victoire, 1 par nulle.
- Le `SWAP` n'est offert qu'au premier tour blanc ; le boss ne l'utilise pas.
- 100 ms par tour (1000 ms au premier tour), jusqu'à 1025 actions légales.
- La partie est une course aux pushes : les logs Wood 3 montrent une élimination
  dès le tour 3 et une manche terminée en 18 à 43 tours.

## Hypothèse validée

Une recherche minimax à **budget d'évaluations** (20 000, pas de chronomètre,
donc reproductible dans les 4 langages) avec ordre des coups par évaluation
immédiate, alpha-beta, beam 24 à la racine / 16 aux nœuds, et évaluation axée
sur le plateau adverse le plus faible bat le boss MCTS : **4 victoires + 1
nulle / 5**. La version chronométrée (82 ms) a gagné 5/5 mais n'était pas
reproductible ; un budget de nœuds (200) timeoutait car il ne bornait pas le
coût réel de génération/tri/évaluation des coups.
