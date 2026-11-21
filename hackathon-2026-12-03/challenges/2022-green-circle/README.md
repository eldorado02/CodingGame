# 2022 Green Circle — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2022-green-circle`. Objectif : **Ligue Légende**.
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
# Green Circle (2022-06-16)
This is a **league based** challenge.
For this challenge, multiple leagues for the same game are available. Once you have proven yourself against the first Boss, you will access a higher league and extra rules will be available.
Starter bots are available in the [Starter Kit](https://github.com/societe-generale/GreenCircle/tree/master/starterAIs). They can help you get started with your own bot.
*Introductory video by **Sebastien and Loïc**: https://youtu.be/OJxV_zhICls*
This challenge is inspired from the board game [Samsara](https://okaluda.fr/samsara-le-jeu/)
## Goal
The game takes place in a company IT department. Management is organizing a hackathon on the Green IT theme. Two development teams are competing to be the most efficient team. Release the required applications before the opponent team to have more points, but respect the Green IT requirements or you'll get drowned in the Technical Debt.
## Rules
Each player has an IT development team. The game takes place in an office with 8 desks. The hackathon will last many turns. Each turn, the players will play one after the other.
This game is based on **Deck Building**. Each team will have its own deck of cards that will get bigger during the game.
Each team starts the game with `4` **BONUS skill** cards and `4` **Technical Debt** cards. Those cards will be their personal draw pile. And their personal discard pile will be reshuffled into the draw pile when it gets empty.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Green Circle
1. Lire les applications restantes, la main et les actions possibles.
2. En `MOVE`, ajouter virtuellement chaque carte de bureau possible à la main.
3. Classer les applications par nombre de compétences propres utilisables.
4. Choisir le bureau offrant le meilleur score, départage par id croissant.
5. En `RELEASE`, choisir la meilleure application parmi celles proposées par le moteur.
6. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
7. Validation : 10 cas déterministes + 1200 cas de fuzz + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 12 phases × 4 langages → OK.
- Fuzz différentiel : 200 cas simples + 1000 cas avec mains multiples → **0 divergence**.
- Une test game de diagnostic a révélé que les bureaux ne figurent pas dans
  `cardLocations` ; le bot s’appuie donc sur `possibleMoves` plutôt que sur une
  lecture directe des cartes de bureau.
- Validation officielle (agent 0) : **17 victoires / 18**.
  - Python : 8/8 (boss et trois joueurs Wood 2, plus 4 revanches contre le boss).
  - C : 2/2, C++ : 2/2.
  - Rust : 5/6 (défaite isolée 4–5 contre le boss, puis 4 victoires 5–4).
- Toutes les victoires se terminent sur le score `5–4` : la stratégie gagne une
  application d’avance plutôt que par départage de Technical Debt.
- Amélioration possible : estimer l’épuisement des bureaux en suivant les
  déplacements des deux joueurs, et pénaliser les destinations déjà visitées.

## Cartographie des 4 langages

`solutions/c/bot.c` — 140 lignes
`solutions/cpp/bot.cpp` — 131 lignes
`solutions/python/bot.py` — 86 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 132 lignes

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

1. Remplacer une heuristique par une recherche exacte sur un sous-problème et mesurer le gain en test games.
2. Ajouter au fuzz un cas limite non couvert (égalité, reset, entrée vide) puis vérifier les 4 langages.
3. Comparer les métadonnées de test game (scores, summary) pour trouver une cause mécanique de défaite.

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
