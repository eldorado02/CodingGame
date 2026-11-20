# Plan — Code Royale

## Algorithme commun

### Init

Lire `numSites`, puis les N lignes `siteId x y radius` dans un tableau indexé
par `siteId`.

### Par tour

1. Lire `gold touchedSite`.
2. Lire N lignes `siteId i1 i2 structureType owner param1 param2` ; mettre à
   jour l'état de chaque site.
3. Lire `numUnits`, puis les lignes `x y owner unitType health` ; repérer les
   deux reines (`unitType == -1`).
4. **Cible de construction** : parmi les sites `structureType == -1` ou nos
   barracks non en entraînement, choisir celui qui minimise
   `dist(site, reineEnnemie) + 0.5 * dist(site, notreReine)`, avec
   `dist(site, reineEnnemie) > 150` si possible.
5. **Action reine** :
   - si un barracks ennemi est à moins de 250 de notre reine →
     `MOVE <x du site> <y du site>` (destruction par contact) ;
   - sinon → `BUILD <siteId> BARRACKS-KNIGHT` vers la cible.
6. **TRAIN** : pour chaque site avec `structureType == 2`, `owner == 0`,
   `param1 == 0`, `param2 == 0` (KNIGHT), tant que `gold >= 80`, ajouter
   l'id au `TRAIN` et décrémenter `gold` de 80.
7. Écrire les 2 lignes.

### Cas de test

- `01_build.in` : site libre, pas de barracks → `BUILD ... BARRACKS-KNIGHT`.
- `02_train.in` : barracks KNIGHT prêt, 100 or → `TRAIN <id>`.
- `03_notrain.in` : barracks en cours (`param1=3`) → `TRAIN` seul.
- `04_archer.in` : barracks ARCHER prêt → pas de TRAIN sur lui.
- `05_destroy.in` : barracks ennemi proche → `MOVE`.
- `06_nosite.in` : tous les sites à nous → `WAIT`.
- `07_multi.in` : 2 barracks prêts, 160 or → `TRAIN id1 id2`.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
