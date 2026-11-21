# Olymbits — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2024-summer-challenge-olymbits`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernières soumissions connues :
- `41773894 (Wood 1)`
- `41776913 (Bronze)`
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
First line: `playerIdx` an integer to indicate which agent you control in the mini-games.
Next line: the number of simultaneously running mini-games. For this league it's `1`.
### Input for One Game Turn
Next `3` lines: one line per player, ordered by `playerIdx`. A string `scoreInfo` containing a breakdown of each player's final score. In this league, it contains `4` integers. The first integer representing the player's current **final score points** followed by: `nb_gold_medals`, `nb_silver_medals`, `nb_bronze_medals`.
Next `nbGames` lines: one line for each mini-game, containing the eight space-separated registers:
- `gpu` a string
- `reg0` an integer
- `reg1` an integer
- `reg2` an integer
- `reg3` an integer
- `reg4` an integer
- `reg5` an integer
- `reg6` an integer
Unused registers will always be `-1`.
### Output
One of the following strings:
- `UP`
- `RIGHT`
- `DOWN`
- `LEFT`
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Olymbits
1. Récupérer l'énoncé, le stub et la liste des adversaires via l'API.
2. Lire le moteur officiel (`HurdleRace.java`, `Game.java`) pour fixer la
   mécanique exacte : arrivée en 29, étourdissement de 2 tours, fin anticipée,
   classement par position.
3. Écrire le bot Python : BFS sans collision, relancé à chaque tour.
4. Porter en C, C++ et Rust en conservant l'ordre des actions du BFS
   (`LEFT`, `DOWN`, `RIGHT`, `UP`) pour garantir des sorties identiques.
5. Générer 10 scénarios déterministes (pistes officielles + resets) et 200 cas
   de fuzz, comparer les 4 implémentations.
6. Jouer des test games contre le boss Wood et les meilleurs adversaires dans
   les 4 langages.

## Dernier bilan de validation (extrait de `critique.md`)

## Montée en Bronze — correction du crash sur registres négatifs (soumission `41776913`)
En Bronze, les 4 mini-games tournent en parallèle et le bot Wood 1 interprète
tous les registres comme des positions de course d'obstacles. L'archerie stocke
cependant une **coordonnée x signée** dans `reg[player]` : le bot indexait la
piste avec une valeur négative et plantait (`IndexError: string index out of
range`) dans ~30 % des parties, ramenant le score à `-1`.

Correction : ramener toute position négative à `0` avant la simulation, dans
les 4 langages (Rust parsem déjà les valeurs invalides à 0 via `unwrap_or`). Le
générateur de fuzz couvre désormais explicitement les registres négatifs.

Validation : **210/210 cas différentiels** identiques dans 4 langages (générateur
étendu), puis **13/13 parties gagnées** contre le boss Bronze `5507123`
(adversaire `5507574`), scores 0 contre 0 pour le boss. Soumission officielle :
`41776913`.

## Cartographie des 4 langages

`solutions/c/bot.c` — 113 lignes
`solutions/cpp/bot.cpp` — 111 lignes
`solutions/python/bot.py` — 87 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 10 lignes
`solutions/rust/bot.rs` — 122 lignes
`solutions/rust/target` — 0 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **10**.
- Harnais : `bash tests/run.sh` — #!/usr/bin/env bash # Différentiel 4 langages : 10 cas déterministes + 200 cas de fuzz..
- Le générateur `tests/gen_cases.py` couvre les cas listés dans son docstring ;
  vérifier avant d'ajouter un cas qu'il est rejoué dans les 4 langages.
- Critère de soumission : **≥3/5 victoires** contre le boss de la ligue courante,
  en plus du fuzz intégralement vert.

## Pièges déjà rencontrés

Voir les entrées correspondantes dans
[`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md).
Sections présentes dans `critique.md` :
- Avant implémentation
- Après implémentation
- Montée en ligue Wood 1 — 4 courses simultanées (soumission `41773894`)
- Montée en Bronze — correction du crash sur registres négatifs (soumission `41776913`).

## Points d'amélioration à tester

1. Tester un horizon de recherche différent en mesurant le pire cas (attention au timeout).
2. Étendre le générateur de fuzz aux registres signés et aux bornes.

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
