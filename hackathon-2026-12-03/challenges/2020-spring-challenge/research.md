# Recherche — Spring Challenge 2020 (2020-05-07)

## Sources
- Énoncé officiel : `statement.md`.
- Referee open-source : https://github.com/CodinGameCommunity/SpringChallenge2020
  (`src/main/java/com/codingame/game/`). **Lecture seule, aucun code copié.**

## Mécanique (Wood 4 = première ligue)
- Grille `width × height` variable, symétrique par rapport à l'axe vertical.
- Cases : `#` mur, ` ` plancher. Wrap horizontal : sortir à gauche revient à droite.
- Un pac par joueur, id toujours `0`. 200 tours max.
- Début : la grille est remplie de pellets (`1`) et de super-pellets (`10`).
- Chaque tour sont fournis : scores, tous les pacs (id, mine, x, y, …),
  tous les pellets (x, y, value).
- `MOVE 0 x y` : le pac calcule un plus court chemin **sans tenir compte des
  pellets ni des pacs**, puis avance d'une case. Cible hors de portée ou sur un
  mur = commande invalide (défaite).
- Collision : si deux pacs se croisent ou arrivent sur la même case, les deux
  mouvements sont annulés.
- Victoire : plus grand score ; l'arbitre arrête la partie quand le score
  restant ne peut plus changer l'issue.

## Conséquences pour le bot
- Un BFS donne le plus court chemin et le **premier pas** vers n'importe quelle
  cible atteinte. Le wrap horizontal doit être modélisé dans le BFS.
- Stratégie : maximiser `valeur / distance` parmi tous les pellets, départage
  par distance puis coordonnées. Le super-pellet vaut 10 déplacements de pellet
  normal : il est prioritaire sauf s'il est très éloigné.
- Éviter la collision : prédire la case voisine de l'adversaire la plus
  intéressante ; si notre premier pas y mène, choisir la deuxième meilleure
  cible. Les informations adverses ne donnent pas son objectif, seulement sa
  position.
- Les cibles doivent être des cases plancher ; le BFS garantit l'accessibilité.

## Risques
| Risque | Mitigation |
|---|---|
| Cible mur / inaccessible | BFS complet avant d'émettre `MOVE`. |
| Collision | Prédiction du meilleur pas adverse, évitement si conflit. |
| Timeout | BFS O(w·h) par tour, caches des données de la grille. |
| Divergence 4 langages | Même ordre de départage `(distance, -value, x, y)`. |
