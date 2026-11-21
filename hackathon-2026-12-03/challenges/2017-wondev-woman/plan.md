# Plan — Wondev Woman

## Algorithme commun

Init : `size`, `unitsPerPlayer` (1). Puis chaque tour :

1. Lire `size` lignes de grille (caractères `0`–`3` ou `.`).
2. Lire position unité (`ux uy`), adversaire (`ox oy`).
3. Lire `legalActions`, puis les lignes `atype index dir1 dir2`.
4. Pour chaque action, calculer :
   - `dest = (ux,uy) + delta[dir1]`, `h_dest = grille[dest]` (`-1` si hors/trou) ;
   - `build = dest + delta[dir2]`, `h_build = grille[build]`.
5. Score :
   - `h_dest == 3` → +1 000 000 (victoire) ;
   - déni (`h_build == 3` et `build` adjacent à l'adversaire visible) → +50 000 ;
   - `+1000 * h_dest` (ou -100 000 si `-1`) ;
   - `h_build == 2` → +300, `== 1` → +100, `== 3` ailleurs → -50.
6. Sortir l'action au meilleur score : `MOVE&BUILD 0 dir1 dir2`, ou `WAIT`.

### Directions

`N(0,-1) NE(1,-1) E(1,0) SE(1,1) S(0,1) SW(-1,1) W(-1,0) NW(-1,-1)`

### Cas de test

- `01_win.in` : une action mène sur une case 3 → victoire.
- `02_climb.in` : deux montées possibles → choisir la plus haute.
- `03_deny.in` : adversaire peut gagner → détruire sa case 3.
- `04_build.in` : deux cases à construire → préférer élever une case 2.
- `05_hidden.in` : adversaire invisible (`-1 -1`) → pas de déni.
- `06_hole.in` : grille avec trous `.` → hauteur -1.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.

## itération 2 — correction du protocole (2026-11-21)

1. Lire `unitsPerPlayer` lignes pour nous **et** autant pour l'adversaire.
2. Lire `legalActions` puis les lignes `atype index dir1 dir2` (index d'unité).
3. Évaluer **toutes** les actions de **toutes** les unités avec une grille de
   score entière ; choisir la meilleure (égalité → première action du referee).
4. Émettre **une seule ligne** (le referee ignore les suivantes) ;
   `ACCEPT-DEFEAT` si aucune action.
5. Porter en C/C++/Rust et étendre le harnais à 10 cas déterministes + 200 fuzz.
