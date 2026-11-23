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

## 2026-11-23 — referee lu (github.com/n3l-ctrl/brandubh-codingame, Referee.java + Board.java)
- Position initiale : attaquants d1 d2 a4 b4 f4 g4 d6 d7 ; défenseurs d3 c4 e4 d5 ; roi d4.
- Joueur 0 = attaquants (joue en premier), joueur 1 = défenseurs. 1000 ms / 50 ms, 200 tours max (nul).
- Déplacement en tour ; seuls trône et coins sont interdits aux non-rois ; on peut traverser le trône vide.
- Capture : seulement par la pièce déplacée, sur 2 côtés opposés ; coin toujours hostile ; trône vide hostile sauf pour capturer le roi ; trône occupé (roi) hostile aux attaquants uniquement ; le roi capture.
- Roi sur le trône : 4 côtés hostiles ; ailleurs 2.
- Fin : roi capturé ou défenseurs encerclés (aucun D/K atteignable depuis le bord par des cases non-attaquants) → attaquants ; roi sur un coin → défenseurs ; joueur sans coup → perd ; position (plateau + camp au trait) répétée 3 fois → le joueur qui vient de jouer perd.
