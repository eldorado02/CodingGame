# Elemental Wars

État : **Wood 3**, soumission C++ `41786190` (en cours de classement).

## Stratégie

Un élémental par tribu en Wood 3. Le bot suit la proie (`W→F→P→W`) en fuyant
le prédateur :

```
score = -100 * dist(proie) + 20 * min(dist(prédateur), 6)
+10000 si capture immédiate
-10000 si case du prédateur
-2000  si prédateur à distance <= 1 et proie à distance > 1
```

Départage : score, puis `x` croissant, puis `y` croissant.

## Fichiers

- `solutions/python/bot.py` — référence.
- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/src/main.rs`.
- `tests/run.sh` — fuzz différentiel 200 cas.
- `statement.md`, `research.md`, `plan.md`, `critique.md`.

## Résultats

- Fuzz : **200/200** identiques dans 4 langages.
- Test games C++ : **5/5** (`scores: 0=1 1=0 2=0`) contre `6910638,6697588`.
- Soumission C++ `41786190`.

## Commandes

```
python3 bot-programming/tools/cg.py test elemental-wars \
  hackathon-2026-12-03/challenges/elemental-wars/solutions/cpp/bot.cpp \
  --language C++ --opponents 6910638,6697588
```
