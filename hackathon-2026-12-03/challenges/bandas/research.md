# bandas — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- https://github.com/Oli8/CG-bandas — referee officiel (Java). Lu : https://raw.githubusercontent.com/Oli8/CG-bandas/master/src/main/java/com/codingame/game/Referee.java (placement initial équilibré au centre, déplacements, rognage, fin à 200 tours). Retenu : port exact.
- `bash bot-programming/tools/gh_search.sh "bandas codingame"` → uniquement Oli8/CG-bandas.
- Recherche OpenCode : voir section ci-dessous (ajoutée quand le rapport est disponible).

## Faits du referee / contradictions avec l'énoncé
- Placement : 8 pions du joueur 0 tirés dans le carré central 4×4, le reste du carré au joueur 1 ; puis 24 pions du joueur 0 au hasard, le reste au joueur 1.
- Un pion poussé dans un trou disparaît ; sa case d'origine devient vide.
- `checkWinner` teste d'abord le joueur 0 : 0 pion chacun → le joueur 1 gagne.
- Fin à `turn == 199` : plus de pions gagne, égalité possible.

## Classe de problème (ALGO.md) et approche retenue
B, branching 4 : negamax αβ profond.
