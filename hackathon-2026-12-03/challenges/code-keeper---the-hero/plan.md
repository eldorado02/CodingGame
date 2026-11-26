# code-keeper---the-hero — plan

1. Protocole : chaque tour `x y health score hammer scythe bow`, puis `n` entités
   `ex ey etype evalue` (Chebyshev <= 3). Sortie `MOVE x y` ou `ATTACK w x y`.
   150 tours, 50 ms/tour. Jeu solo : 4 héros sur des labyrinthes identiques.
2. Invariant d'état : mémoire des cases (inconnue / sol / mur), des objets, de la
   sortie et des monstres. Un monstre ne bouge que s'il voit le héros, donc
   seulement dans notre champ de vision : sa dernière position connue reste vraie.
3. Algorithme : règles + BFS (16x12, négligeable). Combat : épée sur un voisin
   cardinal, arc si le tir tue, faux / marteau sur une diagonale, sinon se placer
   sur un côté cardinal. Déplacement : sortie si le temps ou la santé l'impose,
   sinon l'objet ou la frontière la plus proche.
4. Départage : distance BFS minimale, puis plus petit index de case ; cibles de
   combat : tuable > dégâts du monstre > moins de PV, puis ordre des cases.
5. Fuzz : 200 flux de 1 à 40 instantanés sur un labyrinthe aléatoire fixe
   (monstres hors plateau, plusieurs entités sur une case, 0 charge, PV bas).
6. Hypothèse : trouver la sortie (+10000) vaut plus que tout le reste. Signal de
   réfutation : sortie non trouvée au tour 150, ou mort (-1000) en début de partie.
