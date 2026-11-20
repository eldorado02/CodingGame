# Plan — Ocean of Code

1. Lire la grille 15×15 et choisir une case eau très connexe, proche du centre.
2. Maintenir `visited` (notre chemin) et `bel` (positions adverses possibles).
3. Chaque tour : lire position/PV/cooldowns/sonar/ordres adverses.
4. Appliquer le retour de notre dernier torpedo à `bel`, puis les ordres adverses.
5. Choisir un déplacement sûr (Warnsdorff + centroïde de `bel`), charger le torpedo.
6. Si le torpedo est prêt, choisir la meilleure cible à portée 4 et tirer.
7. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
8. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.
