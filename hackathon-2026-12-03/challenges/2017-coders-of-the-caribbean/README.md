# 2017 Coders Of The Caribbean — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2017-coders-of-the-caribbean`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernières soumissions connues :
- `voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md``
- Ligues/rangs : interroger `python3 ../../bot-programming/tools/cg.py get <pretty_id> progress`.
- Journal global : [`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md)
- Règles du projet : [`../../bot-programming/PLAYBOOK.md`](../../bot-programming/PLAYBOOK.md)

## Ordre de lecture

1. `statement.md` — énoncé officiel (à **relire à chaque promotion de ligue**).
2. `research.md` — sources GitHub / arXiv / referee, mécanique exacte, décisions.
3. `plan.md` — algorithme, complexité, règle de départage.
4. `critique.md` — risques identifiés, puis bilan après validation.
5. `solutions/` — 4 implémentations.
6. `tests/` — cas déterministes et fuzz différentiel.

## Contrat d'entrée / sortie

```
# Coders of the Caribbean (2017-04-14)
This is a **league based** puzzle.
For this puzzle, multiple versions of the same game are available. Once you have proven your skills on this first version, you will access a higher league and extra rules will be unlocked.
## The Goal
In this game, you are in command of pirate ships and your goal is to avoid running out of rum. If all the rum dries up on a given ship, your crew will go mad and you will lose the ship. Barrels of rum are placed randomly across the map. You must control the movements of your ship in order to collect the barrels of rum before your opponent does.
## Rules
The game is played on a hexagonal grid `23` cells wide and `21` high.
Both players have one ship (and up to 3 in later leagues). Every ship is `3` cells long and `1` cell wide.
On the grid you will find barrels of rum (`BARREL`). The barrels contain between `10` and `20` units of rum. Each ship can carry a maximum of `100` units of rum (surplus will be lost).
Each turn, the players can decide to move their ship towards the cell of their choice using the `MOVE` command. Ships can gain speed, slow down, turn left (port) or right (starboard). The `MOVE` action uses a very simplified algorithm to reach the destination.
Note: you may choose to use the `WAIT` command, in which case your boat will retain the direction and speed it had last turn. Using the `SLOWER` command will slow your ship down.
**Game turns:**
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

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

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(19)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Test game réel : partie complète jouée sans timeout ni commande invalide,
  **rang 1/2** (score 0–0, victoire au départage). Les deux navires ont
  survécu jusqu’au tour 200.
- Limite : la stratégie « baril le plus proche » est minimale en Wood 3 ;
  avec 1 navire et `MOVE` automatique, il n’y a pas grand-chose d’autre à
  optimiser. Les ligues supérieures (3 navires, canons, mines, virage
  manuel) demanderont une refonte.

## Cartographie des 4 langages

`solutions/c/bot.c` — 39 lignes
`solutions/cpp/bot.cpp` — 41 lignes
`solutions/python/bot.py` — 37 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 49 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **7**.
- Harnais : `bash tests/run.sh` — #!/bin/sh # Compare les 4 implémentations sur les cas déterministes..
- Le générateur `tests/gen_cases.py` couvre les cas listés dans son docstring ;
  vérifier avant d'ajouter un cas qu'il est rejoué dans les 4 langages.
- Critère de soumission : **≥3/5 victoires** contre le boss de la ligue courante,
  en plus du fuzz intégralement vert.

## Pièges déjà rencontrés

Voir les entrées correspondantes dans
[`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md).
Sections présentes dans `critique.md` :
- Avant implémentation
- Après implémentation.

## Points d'amélioration à tester

1. Remplacer une heuristique par une recherche exacte sur un sous-problème et mesurer le gain en test games.
2. Ajouter au fuzz un cas limite non couvert (égalité, reset, entrée vide) puis vérifier les 4 langages.
3. Comparer les métadonnées de test game (scores, summary) pour trouver une cause mécanique de défaite.

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
