# Plan — Code of Kutulu

## Algorithme commun

### Init

Lire `width`, `height`, les `height` lignes de la carte (`#` mur, `w` spawn,
`.` vide), puis la ligne des 4 entiers ignorés.

### Par tour

1. Lire `entityCount`, puis les lignes
   `entityType id x y param0 param1 param2`.
   - Première ligne = nous (`mx, my`, sanity).
   - `EXPLORER` autres → liste des autres explorateurs (avec sanity).
   - `WANDERER` avec `param1 == 1` (WANDERING) → liste des menaces.
2. Candidats : `(mx, my)` + 4 voisins dans la carte et non-murs.
3. Pour chaque candidat `p` :
   - `reassured` = ∃ autre explorateur avec `|dx|+|dy| ≤ 2` ;
   - `dw` = distance Manhattan minimale à un wanderer actif (100 si aucun) ;
   - `de` = distance Manhattan minimale à un autre explorateur (100 si aucun) ;
   - `score = (100000 si reassured sinon 0) + 100*dw - de`.
4. Choisir le meilleur score (égalité → premier dans l'ordre :
   rester, haut, bas, gauche, droite), écrire `WAIT` ou `MOVE x y`.

### Cas de test

- `01_reassure.in` : déjà à 2 d'un explorateur → rester (pas de wanderer).
- `02_alone.in` : seul → se rapprocher du plus proche explorateur.
- `03_flee.in` : réassuré mais wanderer à distance 1 → fuir tout en restant réassuré.
- `04_wall.in` : voisin mur → ne pas y aller.
- `05_dead.in` : autres explorateurs à 0 sanity → les ignorer.
- `06_spawning.in` : minion `SPAWNING` (param1=0) → ignoré.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
