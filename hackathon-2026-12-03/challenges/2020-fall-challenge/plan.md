# Plan — Fall Challenge 2020

1. Lire les ordres et l'inventaire ; ignorers les champs non utilisés en Wood 4.
2. Filtrer les recettes réalisables, trier par `(-price, somme besoins, id)`.
   Le prix est le 7e champ de la ligne d'ordre.
3. Jouer `BREW id` immédiatement, sinon `WAIT`.
4. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
5. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame.
