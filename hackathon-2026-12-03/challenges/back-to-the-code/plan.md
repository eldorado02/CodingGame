# back-to-the-code — plan

1. Protocole : init `opponentCount` ; par tour `gameRound`, `x y back`, une ligne
   par adversaire (`-1 -1 0` s'il est sorti), 20 lignes de 35 caractères
   (`.` neutre, `0` nous, `1..3` adversaires). Sortie `X Y` ou `BACK n`.
2. Invariant : rectangle suivi d'un tour à l'autre, revalidé à chaque tour.
3. Algorithme : sommes préfixes 2D (neutre / nous / adversaires) ; candidats =
   rectangles <= 12x12 sans case adverse, nous sur le bord ; score gain / coût
   (cases neutres / cases du bord pas encore à nous) ; rejet si un adversaire est
   à moins de coût / 2 du rectangle. Parcours horaire coin par coin.
4. Départage : ratio (produits entiers), puis gain, puis ordre y0, y1, x0, x1.
5. Fuzz : 200 parties de marcheurs aléatoires (1 à 3 adversaires, grille à moitié
   possédée, abandons).
6. Hypothèse : de petits rectangles sûrs battent le milieu de tableau.
   Réfutation : < 3/5 contre le rang 500.
