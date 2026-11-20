# Plan — Spring Challenge 2022

1. Lire la base, les 3 héros et les entités du tour.
2. Ne conserver que les monstres avec `threatFor == 1`.
3. Estimer leur prochaine position (400 vers notre base si `nearBase == 1`,
   sinon `x + vx`, `y + vy`).
4. Trier par `(distance estimée à la base, pv, x, y)`, puis assigner les héros
   au plus proche disponible.
5. `MOVE` vers la cible, ou rester sur place si déjà à portée de coup.
6. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
7. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame.
