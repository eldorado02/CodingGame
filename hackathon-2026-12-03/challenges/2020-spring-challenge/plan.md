# Plan — Spring Challenge 2020

1. Lire la grille et prédéfinir trois BFS : depuis nous, depuis l'adversaire,
   et les voisins (pour l'évitement).
2. Chaque tour : lire scores, pacs et pellets.
3. Calculer le BFS depuis notre pac (wrap horizontal).
4. Trier les pellets accessibles par `(distance, -value, x, y)`.
5. Déduire le premier pas adverse probable (pellet le plus proche de lui) et
   choisir le meilleur pellet dont le premier pas ne collide pas ; sinon le
   meilleur reste le choix par défaut.
6. Émettre `MOVE 0 x y`.
7. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
8. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame (1 opposant).
