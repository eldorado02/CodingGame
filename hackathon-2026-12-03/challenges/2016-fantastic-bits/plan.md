# Plan — Fantastic Bits (Wood 2)

## Algorithme commun

Init : `myTeamId`. Puis chaque tour : `myScore myMagic`, `oppScore oppMagic`,
`entities`, puis `entities` lignes `id type x y vx vy state` (type en string).

### Par tour

1. Lire les entités. Séparer `mine` (WIZARD), `snaffles` (SNAFFLE).
2. But adverse = (16000,3750) si team 0, (0,3750) si team 1.
   But propre = l’inverse.
3. Pour chaque sorcier `w` (id croissant) :
   - si `w.state == 1` → `THROW goal_x goal_y 500` ;
   - sinon, choisir un Snaffle cible :
     - sorcier 0 (attaquant) : Snaffle libre le plus proche de `w` ;
     - sorcier 1 (défenseur) : Snaffle le plus proche de **notre but**, s’il
       est dans notre moitié, sinon le plus proche de `w` ;
     - exclure les Snaffles déjà visés par l’autre sorcier.
   - viser `pos + v*3`, avec thrust 150 si distance > 1200, sinon 60.
4. Écrire une ligne par sorcier, `flush`.

### Géométrie

Tout en entier. `anticipated = (x + 3*vx, y + 3*vy)`, borné au terrain.
Distance² pour le choix des cibles.

### Cas de test

- `01_grab.in` : un sorcier `state=1` → THROW vers but adverse.
- `02_chase.in` : Snaffle loin → MOVE thrust 150.
- `03_close.in` : Snaffle proche → MOVE thrust 60.
- `04_defense.in` : Snaffle dans notre camp → défenseur l’intercepte.
- `05_team1.in` : team 1 → THROW vers (0,3750).
- `06_two_grab.in` : les deux sorciers ont grab → deux THROW.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`.
- `critique.md` complété après validation + test game réel.
