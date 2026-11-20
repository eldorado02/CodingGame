# Plan — Mean Max

## Algorithme commun

Pas d'init. Chaque tour :

1. Lire 3 scores, 3 lignes « rage » (placeholders), `unitCount`, puis les
   unités : `id type playerId mass radius x y vx vy extra extra2`.
2. Repérer le Faucheur (`type==0 && playerId==0`) et les épaves (`type==4`,
   `extra` = eau restante).
3. Générer les candidats `(tx, ty, power)` :
   - `(w.x, w.y, p)` pour chaque épave et `p ∈ {0, 100, 200, 300}` ;
   - `(x-vx, y-vy, 150)` et `(x-vx, y-vy, 300)` (freinage) ;
   - `WAIT` (pas de poussée).
4. Pour chaque candidat, simuler un tour :
   - `accél = 2*p * (cible - pos) / distance` (0 si `p==0` ou distance nulle) ;
   - `nv = v + accél` ;
   - `np = round_half_away(pos + nv)` ; `nv = round_half_away(nv * 0.8)`.
5. Score = `1e6 * épaves(np dans rayon) + 10 * eau cumulée - distance_min(np) - |nv|`.
6. Sortir le meilleur candidat (`X Y p` ou `WAIT`), puis 2 lignes `WAIT`.

### Cas de test

- `01_reach.in` : une épave proche → accélérer vers elle.
- `02_stop.in` : déjà dans l'épave mais vitesse forte → freiner.
- `03_stay.in` : dans l'épave, vitesse nulle → WAIT.
- `04_choose.in` : deux épaves → privilégier la plus proche/riche.
- `05_none.in` : aucune épave → WAIT.
- `06_overlap.in` : deux épaves superposées → rester dans les deux.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
