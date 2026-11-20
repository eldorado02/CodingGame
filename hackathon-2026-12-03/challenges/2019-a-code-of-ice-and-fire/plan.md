# Plan — A Code of Ice and Fire

1. Lire l'initialisation (mines ignorées en Wood 3) et mémoriser les QG.
2. Chaque tour : lire or, revenus, carte 12×12, bâtiments, unités.
3. BFS depuis le QG adverse ; déplacer toutes nos unités vers lui.
4. Entraîner des unités niveau 1 sur les cases jouables les plus proches.
5. Émettre `MOVE...;TRAIN...` sur une seule ligne.
6. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
7. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.
