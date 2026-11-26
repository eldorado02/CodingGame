# vindinium — plan

1. Protocole : init `size`, size lignes (`.#TM0123`), `myId` ; par tour
   `entityCount` puis `HERO|MINE id x y life gold`. Sortie NORTH/EAST/SOUTH/WEST/
   WAIT/MOVE x y. Axe de x (colonne ou ligne) déduit au 1er tour (case de spawn).
2. Invariant : grille fixe, axe détecté.
3. v2 : achever un ennemi <= 20 PV possédant des mines (<= 2 pas) ; éviter les
   cases voisines d'un ennemi au moins aussi sain ; bière si PV <= 50 ou taverne
   voisine et PV <= 80 ; mine la plus proche si PV - trajet - 20 > 15 ; taverne.
4. Départage : coût, case cible (ligne, colonne), N E S W.
5. Fuzz : 200 cartes aléatoires, deux conventions d'axes.
6. Résultat : 1/4 contre 3 boss (une partie 461, les autres 4 à 91) ; v3 :
   moins de bières (50 bières = 100 or), garder les mines près des tavernes,
   simuler les combats (qui frappe en premier).
