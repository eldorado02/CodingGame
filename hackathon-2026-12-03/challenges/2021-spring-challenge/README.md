# 2021 Spring Challenge — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2021-spring-challenge`. Objectif : **Ligue Légende**.
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
## Game Protocol
### Initialization Input
First line: `numberOfCells` equals `37`.
Next `numberOfCells` lines: `8` space-separated integers:
- `index` for the cell's index.
- `richness` for its richness.
- `6` `neigh` variables: *Ignore for this league.*
### Input for One Game Turn
First line: An integer `day`: the current day. Equals `0` for this league.
Next line: An integer `nutrients`: the current nutrient value of the forest.
Next line: 2 space-separated integers:
- `mySun`: your current sun points.
- `myScore`: your current score.
Next line: 3 space-separated integers:
- `oppSun`: your opponent's sun points.
- `oppScore`: your opponent's score.
- `oppIsWaiting`: equals `1` if your opponent is asleep, `0` otherwise.
Next line: An integer `numberOfTrees` for the current number of trees in the forest.
Next `numberOfTrees` lines: 4 space-separated integers to describe each tree:
- `cellIndex`: the index of the cell this tree is on.
- `size`: the size of the tree. From `0` (seed) to `3` (large tree).
- `isMine`: `1` if you are the owner of this tree, `0` otherwise.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Spring Challenge 2021
1. Lire la `richness` des 37 cases.
2. Chaque tour : lire soleil, arbres et actions possibles.
3. Si `sun >= 4`, compléter l'arbre mûr de plus haute `richness` (départage par index).
4. Sinon `WAIT`.
5. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
6. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 8 tours × 4 langages → OK.
- Fuzz différentiel : 200 cas de 6 tours → **0 divergence**.
- Validation officielle (agent 0) : **8 victoires / 8**.
  - Python : 72–70 et 85–24.
  - C : 78–74 et 78–64.
  - C++ : 78–74 et 76–76 (départage au nombre d'arbres).
  - Rust : 76–72 et 82–6.
- La victoire contre le boss est serrée (72–70) : la règle de départage au
  nombre d'arbres peut sauver une partie à score égal, ce qui s'est produit en C++.
- Amélioration possible : modéliser le nombre optimal de complétions en
  comparant le gain d'un point de nutrient vs le bonus de fin de soleil.

## Cartographie des 4 langages

`solutions/c/bot.c` — 67 lignes
`solutions/cpp/bot.cpp` — 60 lignes
`solutions/python/bot.py` — 52 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 59 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **10**.
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

1. Affiner le choix des sources déjà harvestées (bloquées dans le BFS).

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
