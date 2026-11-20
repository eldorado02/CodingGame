# Plan — Cellularena (Wood)

1. Récupérer énoncé, stub et adversaires via `cg.py` ; lire le moteur officiel
   pour fixer l'économie des protéines, le protocole et les conditions de fin.
2. Écrire le bot Python :
   - lire la grille complète chaque tour dans un tableau de cases
     (`WALL` / organe / protéine) ;
   - BFS multi-source depuis tous les organes dont `owner == 1`, voisinage
     `N, E, S, W`, en mémorisant pour chaque case libre : id du parent, premier
     pas, distance ;
   - choisir une case contenant `A` (priorité), sinon la case libre la plus
     proche ; départage `(distance, y, x)` ;
   - émettre `GROW <parent> <x> <y> BASIC` puis des `WAIT` jusqu'à
     `requiredActionsCount`.
3. Porter en C, C++, Rust en gardant le même ordre de voisinage et le même
   départage, pour garantir des sorties bit à bit identiques.
4. Générer 10 scénarios déterministes + 200 cas de fuzz contenant des organes
   `BASIC` des deux camps et le convention `owner = 1` pour le joueur local ;
   comparer les 4 implémentations via `tests/run.sh`.
5. Jouer des test games officiels contre le boss Wood `5736694` dans les 4
   langages (5 parties chacun) puis mettre à jour l'index et committer.

Complexité : `O(width × height)` par tour en temps et en mémoire, soit ~162 cases
sur la grille Wood, très en deçà des 50 ms par tour.
