# 2019 A Code Of Ice And Fire — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2019-a-code-of-ice-and-fire`. Objectif : **Ligue Légende**.
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
### Initialization input
Line 1: one integer `numberMineSpots`: the number of mine spots on the map. Mine spots will be used from Wood1 league onwards.
Next `numberMineSpots` lines: two integers
- `x` and `y`: coordinates of the mine spot.
### Game turn input
Line 1: an integer `gold`, the player's amount of gold.
Line 2: an integer `income`, the player's income.
Line 3: an integer `opponentGold`, the opponent's amount of gold.
Line 4: an integer `opponentIncome`: the opponent's income.
Next `12` lines: `12` characters, on for each cell:
- `#`: void
- `.`: neutral
- `O`: owned and active cell
- `o`: owned and inactive
- `X`: active opponent cell
- `x`: inactive opponent cell
Next line: `buildingCount`: the amount of buildings on the map.
Next `buildingCount` lines: four integers
- `owner`
  - `0`: Owned
  - `1`: Enemy
- `buildingType`: the type of building
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — A Code of Ice and Fire
1. Lire l'initialisation (mines ignorées en Wood 3) et mémoriser les QG.
2. Chaque tour : lire or, revenus, carte 12×12, bâtiments, unités.
3. BFS depuis le QG adverse ; déplacer toutes nos unités vers lui.
4. Entraîner des unités niveau 1 sur les cases jouables les plus proches.
5. Émettre `MOVE...;TRAIN...` sur une seule ligne.
6. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
7. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 3 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 4 tours → **0 divergence**.
- Validation officielle (agent 0) :
  - Python vs boss Snow et 6 joueurs Wood 3 classés 1 à 6 : **7 victoires / 7**,
    scores adversaires toujours `-1` (QG capturé).
  - Ports C (`568`), C++ (`807`) et Rust (`624`) : **3 victoires / 3** vs boss Snow.
- Le bot gagne en capturant le QG adverse avant la limite de 100 tours, sans
  jamais dépendre du score militaire final.
- Améliorations possibles : éviter les `MOVE` qui « restent sur place » quand
  un voisin est occupé (cas observé dans les logs, non bloquant), et coordonner
  les unités pour réduire les files d'attente sur le chemin du QG.
- Conclusion : en Wood 3, une stratégie de rush massif d'unités niveau 1 suffit
  largement ; la complexité des ligues supérieures (niveaux 2/3, mines, tours)
  n'est pas nécessaire ici.

## Cartographie des 4 langages

`solutions/c/bot.c` — 145 lignes
`solutions/cpp/bot.cpp` — 125 lignes
`solutions/python/bot.py` — 105 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 150 lignes

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
