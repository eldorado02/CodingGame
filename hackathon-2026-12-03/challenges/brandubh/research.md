# Brandubh — recherche

## Sources consultées

1. Énoncé CodinGame (`statement.md`, récupéré via `cg.py show brandubh`).
2. Referee officiel :
   `https://github.com/n3l-ctrl/brandubh-codingame/blob/master/src/main/java/com/codingame/game/Referee.java`
   - `isValidMove` (lignes 281–307) : case d'arrivée vide, mouvement orthogonal,
     chemin dégagé, seul le roi peut finir sur le trône ou un coin.
   - `checkCaptures` / `isHostileSquare` (lignes 317–391) : capture par emprise
     sur un côté, hostile = adversaire, coin, ou trône dans certaines conditions.
   - `isKingCaptured` (lignes 393–404) : roi sur le trône = 4 voisins hostiles.
   - `hasValidMoves` (lignes 406–431) : les non-rois peuvent traverser le trône
     et les coins sans s'y arrêter.
   - `checkEncirclement` (lignes 433–478) : BFS depuis toutes les cases de bord
     non occupées par un attaquant ; si le roi ou un défenseur est atteint, pas
     d'encerclement.
   - `checkWinCondition` (lignes 480–499) : roi capturé / coin / encerclement.
3. `Board.java` : position initiale exacte et prédicats `isCorner` / `isThrone`.

## Faits exploitables

- Les attaquants ont 8 pièces contre 5 côté défense (roi inclus). Le matériel
  seul ne suffit pas : le roi peut traverser de longues lignes libres.
- Le BFS d'évasion doit traiter **toutes** les pièces comme des murs, y compris
  les défenseurs, sinon il sous-estime la liberté du roi.
- Le trône est une case d'arrivée légale pour le roi seulement ; y rester
  n'empêche pas la capture si les 4 voisins deviennent hostiles.
- Les adversaires Wood listés n'ont pas de boss officiel ; `jacek` (agent
  `6913632`) sert de repère de force.
