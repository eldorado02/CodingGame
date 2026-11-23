# hex — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- Énoncé officiel (`cg.py show hex`) — règles, swap, coordonnées, 100 ms.
- https://www.hexwiki.net/index.php/Main_Page — convention de voisinage, ponts (bridge), swap map 11×11. Retenu : voisinage `(-1,+1)/(+1,-1)`.
- https://api.github.com/search/repositories?q=hex%20codingame — 2 dépôts (`nilthehuman/Hex`, hors CG). Aucun referee public trouvé ; la recherche de code GitHub exige une authentification (HTTP 401).
- Test games sondes `games/hex/logs/probe*-*.log` — vérité terrain de la géométrie (voir critique v0).
- Littérature (connue, non relue ici) : MoHex (Arneson, Hayward, Henderson, « Monte Carlo Tree Search in Hex », IEEE TCIAIG 2010) — MCTS + RAVE + playouts qui sauvent les ponts ; Gelly & Silver 2007 (RAVE). [NON VÉRIFIÉ dans cette session]

## Faits du referee / contradictions avec l'énoncé
- Le swap recopie la pierre en transposant `[ligne,col] → [col,ligne]` ; après swap, le plateau reçu montre la pierre bleue transposée.
- Le bot rang 61 swappe une ouverture centrale (`f6`).
- Victoire : score `0=1 1=0` (le perdant 0), timeout adverse : `1=-1`.

## Classe de problème (ALGO.md) et approche retenue
Connexion (BL/C), branching ~120 → MCTS + RAVE (pas d'αβ). Prochaine amélioration : réponse « sauvegarde de pont » dans les playouts.
