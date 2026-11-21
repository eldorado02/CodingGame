# Plan — Tron Battle (Wood 2, 2 joueurs)

## Protocole exact
- Entrée tour : `N P`, puis `N` lignes `X0 Y0 X1 Y1`. `X1,Y1` = tête. `-1` = mort.
- Sortie : **1 seule ligne** `UP|DOWN|LEFT|RIGHT`, flush.
- Tracker : `grid[y][x] = 0` libre, `1` bande. On ne reçoit que (queue, tête) :
  on mémorise toutes les têtes vues et on **efface la bande** d'un joueur mort.
- On n'applique PAS sa propre sortie au tracker (le tour suivant donne déjà la
  nouvelle tête).

## Algorithme (Voronoi 1-ply + greedy primé)
1. Reconstruct : pour chaque joueur vivant, remplir le chemin de la queue à la
   tête (BFS le long des cases déjà marquées) ; marquer les bandes.
2. Pour chacun des 4 coups légaux (case libre) :
   - BFS multi-source depuis la tête de chaque joueur **dans l'ordre des tours**
     (P0 d'abord) : `dist[p][case]`.
   - `voronoi = Σ_cases (mon dist < dist adversaire) − (adversaire < mon dist)`,
     égalité = contestée (0).
   - score = `100000 * voronoi + 10 * reachable + mobilité` (pas de float).
3. Si aucune case adverse n'est atteignable (jeu ouvert séparé) → greedy primé :
   pour chaque premier coup, flood-fill glouton (choisir le voisin libre qui
   retire le moins d'arêtes), garder le plus long.
4. Règle de départage déterministe : `(score, -y, -x, ordre_direction)`.

## Complexité / budget
- 30×20 = 600 cases, 4 coups, BFS O(4×600) ≈ 2 400 opérations → < 5 ms Python.
- Un seul tour de recherche : pas d'alpha-beta en Python (3 playouts/100 ms).

## Invariants
- Les bandes sont reconstruites à partir des têtes mémorisées, pas de la sortie.
- Un joueur mort (`-1`) → bande effacée immédiatement.
- Aucune valeur flottante dans la décision (fuzz reproductible).

## Matrice de tests
- Nominal 2 joueurs (exemple officiel), mort d'un adversaire, plateau presque
  plein, tête au bord, coup unique, adversaire adjacent.

## Hypothèse de victoire
Je gagne si le Voronoi au premier « cut » est supérieur (96–99 % des duels).
Signal de falsification : scores `0=-1` ou pertes répétées contre `630143`
alors que le territoire initial est plus grand.
