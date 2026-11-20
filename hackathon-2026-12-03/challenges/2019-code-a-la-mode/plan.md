# Plan — Code a la Mode

1. Lire les 20 clients et la cuisine (11×7), repérer `D`, `W`, `B`, `I`.
2. Boucle par tour : lire `turnsRemaining`, nous, partenaire, tables, four,
   clients.
3. Choisir une cible par machine à états (plat complet → livraison, etc.).
4. Émettre `USE x y` (le referee rapproche si besoin).
5. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
6. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.
