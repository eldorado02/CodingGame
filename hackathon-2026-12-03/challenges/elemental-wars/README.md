# Elemental Wars

État : **Wood 2**, boss `6697631` (`BOSS 2`). Nouvelle version v3 en cours de
validation des ports ; la version soumise `41786190` (Wood 3) timeout en Wood 2.

## Stratégie v3

Contrairement à Wood 3 (3 élémentaux), Wood 2 compte **6, 9 ou 12 élémentaux**
(2 à 4 par tribu). Le bot commande **tous** ses élémentaux non-prisonniers.

Pour chaque ennemi non-prisonnier, une carte BFS de distances est calculée depuis
sa case. Pour chaque élémental allié, on évalue 5 candidats (rester + 4 voisins) :

```
score = -100 * min(dp, 20) + 15 * min(dq, 8)
+10000 si dp == 0 (capture)
-12000 si dq == 0 (on se fait capturer)
-2000  si dq <= 1 et dp > 1
```

où `dp`/`dq` sont les distances minimales aux proies / prédateurs.
Départage : score, puis `x` croissant, puis `y` croissant.

## Fichiers

- `solutions/python/bot.py` — référence.
- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`.
- `tests/` — fuzz différentiel 200 cas (6/9/12 élémentaux, 11×11 ou 13×13).
- `statement.md`, `research.md`, `plan.md`, `critique.md`.

## Résultats

- `v3` C++ : **5/5** (`scores: 0=2..4 1=0 2=0`) contre `6697631,6699103`.
- Fuzz 200/200 (6/9/12 élémentaux) en cours de validation des ports.

## Commandes

```
python3 bot-programming/tools/cg.py test elemental-wars \
  hackathon-2026-12-03/challenges/elemental-wars/solutions/cpp/bot.cpp \
  --language C++ --opponents 6697631,6699103
```
