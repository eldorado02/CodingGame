# Plan — Botters of the Galaxy

## Algorithme commun

### Init

Lire `team`, `bushAndSpawnPointCount`, puis N lignes `type x y radius`, puis
`itemCount` et `itemCount` lignes d'objets. Tout est ignoré (Wood 6).

### Par tour

1. Lire `gold`, `oppGold`, `roundType`, `entityCount`.
2. Si `roundType < 0` → écrire `HULK`, `flush`, continuer.
3. Sinon, lire `entityCount` lignes ; garder les champs
   `unitId team unitType x y attackRange health max_health shield damage speed stun gold`
   (les champs supplémentaires des HERO sont lus/ignorés).
4. Choisir la cible dans cet ordre :
   - héros ennemi (`team != myTeam`, `unitType == HERO`) ;
   - tour ennemie (`TOWER`) ;
   - unité ennemie (`UNIT`, `GROOT`) ;
   - à défaut `WAIT`.
5. Écrire `ATTACK <id>` (ou `WAIT`), une ligne par héros possédé (`roundType`).

### Cas de test

- `01_pick.in` : `roundType=-1` → `HULK`.
- `02_hero.in` : héros adverse → ATTACK sur lui.
- `03_tower.in` : plus de héros adverse → ATTACK sur la tour.
- `04_wait.in` : aucune cible ennemie → WAIT.
- `05_dead.in` : héros adverse à 0 PV → attaquer la tour.
- `06_order.in` : plusieurs héros ennemis → le plus proche de nous.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
