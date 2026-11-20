# Plan — Smash The Code

## Algorithme (commun aux 4 langages)

```
read A, B (8 fois), my_score, 12 lignes, opp_score, 12 lignes
for x in 0..5:
    g = copie(grille)
    r = premiere ligne libre depuis le bas dans la colonne x
    if r - 1 < 0: continue              // top out -> coup illégal
    g[r-1] = B ; g[r-2] = A
    (cleared, chains) = resoudre(g)
    if cleared == 0:
        score = -hauteur_colonne(x) * 3 - voisins_incoherents
    else:
        score = cleared*10 + (chains-1)*25
                - hauteur_moyenne*0.5 - trous*1
    garder le meilleur x
print x
```

`resoudre(g)` :

```
chains = 0 ; total = 0
loop:
    visited = 6x12 faux
    a_clear = 6x12 faux ; found = false
    for chaque cellule coloree non visite:
        comp = BFS 4-voisins de meme couleur
        if |comp| >= 4: found = true ; marquer toutes les cellules
    if not found: break
    for chaque crâne 0 adjacent a une cellule marquee: marquer aussi
    total += nb cellules marquees ; chains += 1
    supprimer les cellules marquees ; gravite par colonne
```

## Détails d’implémentation

- Stockage : `grid[row][col]`, `row=0` en haut. Gravité = recopier les
  caractères non `.` de bas en haut dans une nouvelle colonne, puis combler
  le haut par `.`.
- Dépôt : chercher `r` = plus petit `row` tel que `grid[r][x] != '.'`
  (12 si colonne vide) ; poser `B` en `r-1`, `A` en `r-2`.
- Égalité `A == B` en Wood ; le code gère quand même `A != B` (leagues
  supérieures) en posant `A` au-dessus de `B`.
- En cas d’égalité de score, choisir la colonne qui minimise la hauteur
  maximale, puis la plus à gauche (déterministe).
- Entrée robuste : lire les 8 paires même si certaines lignes sont vides.

## Livrables

- `solutions/{c,cpp,rust,python}` : un fichier exécutable unique par langue.
- `tests/cases.txt` : cas déterministes (entrée + sortie attendue).
- `tests/run.sh` : compare la sortie des 4Implémentation sur tous les cas.
