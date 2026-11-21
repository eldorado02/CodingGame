# 2020 Spring Challenge — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2020-spring-challenge`. Objectif : **Ligue Légende**.
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
### Initialization Input
Line 1: two integers `width` and `height` for the size of the map.
Next `height` lines: a string of `width` characters each representing one cell of this row: `' '` is a floor and `'#'` is a wall.
### Input for One Game Turn
First line: Two space-separated integers:
- `myScore` your current score
- `opponentScore` the score of your opponent
Second line: One integer:
- `visiblePacCount` for the amount of pacs visible to you
Next `visiblePacCount` lines:
- `pacId`: the pac's id (unique for a given player)
- `mine`: the pac's owner (1 if this pac is yours, 0 otherwise. Converted into a boolean type in most languages.)
- `x` & `y`: the pac's position
- `typeId`: unused in this league
- `speedTurnsLeft`: unused in this league
- `abilityCooldown`: unused in this league
Next line: one integer `visiblePelletCount` for the amount of pellets visible to you
Next `visiblePelletCount` lines: three integers:
- `x` & `y`: the pellet's position
- `value`: the pellet's score value
### Output
A single line with your action:
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Spring Challenge 2020
1. Lire la grille et prédéfinir trois BFS : depuis nous, depuis l'adversaire,
   et les voisins (pour l'évitement).
2. Chaque tour : lire scores, pacs et pellets.
3. Calculer le BFS depuis notre pac (wrap horizontal).
4. Trier les pellets accessibles par `(distance, -value, x, y)`.
5. Déduire le premier pas adverse probable (pellet le plus proche de lui) et
   choisir le meilleur pellet dont le premier pas ne collide pas ; sinon le
   meilleur reste le choix par défaut.
6. Émettre `MOVE 0 x y`.
7. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
8. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame (1 opposant).

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 10 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 8 tours → **0 divergence**.
- Deux bugs détectés puis corrigés pendant la validation :
  1. le calcul du premier pas cherchait un voisin à distance `d-1` depuis le
     **départ** au lieu du BFS inverse — cela figeait le pac sur place ;
  2. le port Rust n'excluait pas les pellets situés sur notre propre case.
- Validation officielle (agent 0) : **9 victoires / 9**.
  - Python : 5/5 vs BLINKY (143–87), Zoryx (103–8), EnderWiggins (103–15),
    gue (151–2), PulsarBF (146–5).
  - C : 2/2 vs BLINKY (120–41, 115–49).
  - C++ : 1/1 vs BLINKY (111–44).
  - Rust : 1/1 vs BLINKY (109–35).
- La stratégie « cible unique mémorisée + BFS + évitement de collision » collecte
  100 à 150 pellets par partie, largement de quoi battre les bots Wood 2.
- Amélioration possible : tenir compte du rayon de collecte et optimiser un
  chemin global (plusieurs pellets par tour) via un TSP approximé.

## Cartographie des 4 langages

`solutions/c/bot.c` — 178 lignes
`solutions/cpp/bot.cpp` — 154 lignes
`solutions/python/bot.py` — 148 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 233 lignes

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

1. Tester un horizon de recherche différent en mesurant le pire cas (attention au timeout).

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
