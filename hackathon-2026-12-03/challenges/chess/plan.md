# chess — plan

1. Protocole : 1er tour constantes (`crazyHouse 0`, `maxMoves 125`), on répond
   `fen moves` ; ensuite par tour la FEN puis `n` coups UCI légaux. Chess960 :
   roque = roi sur sa propre tour (`e1h1`), colonnes dans le champ roque.
2. Invariant : aucun (FEN complète).
3. v1 : 1 pli sur les coups légaux ; plateau joué (roque, en passant,
   promotion) ; score = matériel + centralisation des pièces mineures + avance
   des pions - pire perte adverse (pièce attaquée : valeur, ou valeur -
   attaquant le moins cher si défendue) + 30 si échec.
4. Départage : premier coup légal.
5. Fuzz : 200 cas (positions aléatoires, roques, promotions, coups malformés).
6. Résultat : 1 victoire 2 nuls contre boss Wood 2. v2 : alpha-bêta avec
   générateur Chess960 (les nuls viennent de l'absence de mat).
