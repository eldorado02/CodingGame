# blocking — plan

1. Protocole : init formes (`lettre cols rows définition`), joueurs, id, N,
   lettres ; par tour N lignes (`x` = coin libre pour nous), coups adverses,
   liste des coups valides `col row Q013`.
2. Invariant : tailles des formes (nombre de `#`).
3. v1 : plus grande pièce d'abord, ancre la plus proche du centre.
4. Départage : premier coup de la liste.
5. Fuzz : 200 cas aléatoires (formes, plateaux, listes de coups).
6. Résultat : 0/4 contre boss Wood 2 mais 46-55 contre 49-55. v2 : géométrie
   des orientations (flip puis rotation, carrés numérotés ligne par ligne) pour
   maximiser nos coins `x` et réduire ceux de l'adversaire.
