# volcanoes — plan

1. Protocole : init 80 lignes `nom v1 v2 v3` ; par tour la ligne des niveaux
   (nous > 0, adversaire < 0) puis la ligne des coups valides (noms).
2. Invariant : graphe fixe, paires N<k>/S<k>.
3. v1 : simulation du coup (éruptions en cascade, une fois par tuile) ;
   distance de connexion = min sur k du BFS 0-1 N<k> -> S<k> (nous 0, vide 1,
   adverse bloqué) ; score = 100 x (distance adverse - la nôtre) + niveaux.
4. Départage : premier coup valide.
5. Fuzz : 200 graphes de 80 tuiles, positions aléatoires.
6. Résultat : 2/4 contre boss Wood 3. v2 : croissance générale (phase Growth)
   et éruptions adverses à anticiper.
