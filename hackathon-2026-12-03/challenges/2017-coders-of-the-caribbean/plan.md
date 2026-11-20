# Plan — Coders of the Caribbean

## Algorithme commun

Chaque tour : `myShipCount`, `entityCount`, puis `entityCount` lignes
`id type x y arg1 arg2 arg3 arg4`.

1. Repérer le navire à nous (`type=="SHIP" && arg4==1`) : (x,y).
2. Parmi les `BARREL`, choisir celui dont `d2 = (bx-x)²+(by-y)²` est minimal.
3. Output `MOVE bx by`, ou `WAIT` si aucun baril.

### Cas de test

- `01_one.in` : un baril → MOVE vers lui.
- `02_two.in` : deux barils, choisir le plus proche.
- `03_none.in` : aucun baril → WAIT.
- `04_opponent.in` : barils + navire adverse → ignorer l’adversaire.
- `05_at_position.in` : baril sur le navire → MOVE sur place.
- `06_far.in` : baril lointain → MOVE.

## Livrables

- `solutions/c/bot.c`, `solutions/cpp/bot.cpp`, `solutions/rust/bot.rs`,
  `solutions/python/bot.py`.
- `tests/gen_cases.py`, `tests/*.in`, `tests/run.sh`, `critique.md`.
