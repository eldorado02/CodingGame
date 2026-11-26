# clash-of-bots — plan

1. Protocole : par tour `k`, puis k mini-cartes 5x5 (ligne 0 = haut, 0 vide,
   >0 allié, <0 ennemi, valeur = PV), ordre mélangé. Sortie : k lignes
   `GUARD | MOVE d | ATTACK d | SELFDESTRUCTION`, d dans UP/DOWN/LEFT/RIGHT.
2. Invariant : aucun (pas de position absolue, ordre mélangé) — décision locale.
3. Algorithme : règles par robot, O(25) : explosion rentable (>= 2 tués, aucun
   allié dans le 3x3) > attaque ennemi adjacent le plus faible > embuscade sur
   la case où entrent des ennemis à distance 2 > approche > dérive vers le haut.
4. Départage : ordre UP, DOWN, LEFT, RIGHT ; ennemi le plus proche puis ligne,
   colonne.
5. Fuzz : 200 flux de 1 à 30 tours, 0 à 10 robots, densité 0 à 0,6.
6. Hypothèse : celui qui s'approche se fait frapper en premier (mouvements
   avant attaques) ; l'embuscade punit les bots agressifs.
