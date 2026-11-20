# Plan — CodeBusters

## Algorithme commun

Entrée : `bustersPerPlayer`, `ghostCount`, `myTeamId`. Puis, chaque tour :
`entities`, puis `entities` lignes `id x y type state value`.

Base : `(0,0)` si team 0, `(16000,9000)` sinon.

### Par tour

1. Lire toutes les entités. Séparer `mine` (type == teamId), `ghosts`
   (type == -1). Ignorer les busters adverses (non nécessaires en Wood).
2. **Réservations** : ensemble des ids de fantômes déjà choisis par un buster
   ce tour.
3. Pour chaque buster `b` (id croissant):
   - s’il porte un fantôme (`state == 1`) :
     - si `dist(b, base) <= 800` → `RELEASE` ;
     - sinon `MOVE` vers un point à 800 de `b` sur la droite vers la base.
   - sinon, choisir le fantôme visible non réservé minimisant
     `dist(b, ghost)` ; s’il n’y en a pas, `MOVE` vers le centre (800).
   - avec le fantôme choisi `g` et `d = dist(b, g)` :
     - `d < 900` → `MOVE` vers un point à 800 de `b` **en sens opposé** à `g`
       (recul pour revenir dans la bande) ;
     - `900 <= d <= 1760` → `BUST g.id` (réserver `g`) ;
     - `d > 1760` → `MOVE` vers un point à 800 de `b` vers `g`.
   - Arrondi des coordonnées à l’entier le plus proche ; bornes [0,16000]
     et [0,9000].
4. Écrire une ligne par buster, `flush`.

### Géométrie entière

`dist² = dx² + dy²` en `long long` (max ~3.4e8, carré ~1.1e17 > int32).
Comparaisons de distances en carré : `900²`, `1760²`, `800²`, `1600²`.
MOVE : `nx = x + round(800 * dx / sqrt(d²))`. Pour éviter `sqrt`, on calcule
`d = (long long) sqrt((double)d2)` puis `nx = x + (800*dx + d/2)/d` (arrondi
au plus proche, positif comme négatif via troncature vers zéro ajustée).

### Cas de test

- `01_init_empty.in` : aucun fantôme visible, 2 busters → MOVE centre/base.
- `02_bust_band.in` : fantôme à 1300 → BUST.
- `03_too_close.in` : fantôme à 500 → recul.
- `04_far.in` : fantôme à 5000 → approche de 800.
- `05_carry.in` : buster porteur loin de la base → approche base.
- `06_release.in` : buster porteur à ≤ 800 de la base → RELEASE.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`.
- `critique.md` complété après validation + test game réel.
