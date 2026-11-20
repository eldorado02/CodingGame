# Recherche — A*Craft (2018-10-19)

## Sources

- Énoncé officiel : `statement.md` (API `/api/v1/puzzles/a-star-craft`).
- Referee officiel : https://github.com/CodinGameCommunity/A-Star-Craft
  - `src/main/java/com/codingame/game/Referee.java`
  - `src/main/java/com/codingame/astarcraft/game/Engine.java`
  - `src/main/java/com/codingame/astarcraft/Constants.java`
  - `src/test/java/com/codingame/astarcraft/RefereeTest.java`
  - **Lecture seule, aucun code copié.**
- Socle DSA : `../../dsa/README.md` (graphe, BFS/DFS).

## Mécanique exacte (vérifiée dans le referee)

- Grille torique 19 × 10. `#` = vide, `.` = plateforme libre, `U/R/D/L`
  dans l'entrée joueur = flèche **préexistante** (non modifiable).
- Les robots ne sont PAS dans les 10 lignes de grille : ils sont donnés
  après, sous forme `x y dir`. Leurs cases apparaissent comme `.`.
- **Une seule phase d'output** : tour 0. Le referee applique les triplets
  `x y d` dans l'ordre ; une flèche sur une case déjà pourvue ou sur `#`
  est ignorée. Ensuite, plus aucun output n'est lu (1 ms par tour).
- Tour ≥ 1 : `score += robots vivants`, puis chaque robot avance d'une
  case ; si la case d'arrivée porte une flèche, sa direction est remplacée ;
  le robot meurt s'il arrive sur `#` ou si l'état `(case, direction)` a déjà
  été enregistré. L'état initial est enregistré **après** application des
  flèches du tour 0 (donc poser une flèche sous un robot change son départ).
- `maxTurns = 2000`. Le score maximum théorique par robot est `4 × 190 = 760`
  états distincts, mais une case avec flèche force une seule direction de
  sortie : les chemins longs passent donc par des flèches **parcimonieuses**.

## Décisions de conception

1. Ne poser une flèche que lorsque continuer tout droit mènerait à un état
   déjà visité ou à un trou noir. Les cases sans flèche gardent la direction
   d'entrée : un même `(x, y)` peut alors être traversé dans 4 directions
   différentes = 4 états distincts.
2. Choisir la direction de rechange par lookahead : minimiser le nombre
   d'états déjà visités sur la case suivante, départageage `U,R,D,L`,
   priorité à « ne pas poser de flèche » en cas d'égalité.
3. Traiter les robots dans l'ordre d'entrée, puis en ordre inverse, et
   garder la construction avec le meilleur score total (déterministe).
4. PrNG non nécessaire : tout est entier et déterministe → fuzz 4 langages.

## Risques

| Risque | Mitigation |
|---|---|
| Flèche sur case préexistante | `fixed[y][x]` interdit tout remplacement. |
| Flèche sur `#` | Jamais émise. |
| Direction initiale modifiée | Simulée : on lit la flèche de la case de départ. |
| Cycle infini dans la simulation | Plafond `4*W*H = 760` pas par robot. |
| Score sous-optimal | Deux ordres de robots, meilleur des deux. |
| Divergence inter-langages | Arithmétique entière, ordres de départage identiques. |
