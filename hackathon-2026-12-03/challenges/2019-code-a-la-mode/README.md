# 2019 Code A La Mode — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2019-code-a-la-mode`. Objectif : **Ligue Légende**.
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
# Code a la Mode
This is a **league based** challenge.
Wood leagues should be considered as a tutorial which lets players discover the different rules of the game.
In Bronze league, all rules will be unlocked and the real challenge will begin.
Starter bots are available in the [Starter Kit](https://github.com/csj/code-a-la-mode/tree/master/src/test/starterkit). They can help you get started with coding your own bot.
## The Goal
Control a chef and prepare food for customers as quickly as possible to earn more points than the other players.
## The Game
This is a three-player game played on a grid `11` cells wide and `7` cells high. A match is played in 3 rounds, each round with only 2 of the players.
- **Round 1:** player A with player B.
- **Round 2:** player C with player A.
- **Round 3:** player B with player C.
Each player will thus play two rounds in each match. A player's total points is the sum of both rounds' points.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Code a la Mode
1. Lire les 20 clients et la cuisine (11×7), repérer `D`, `W`, `B`, `I`.
2. Boucle par tour : lire `turnsRemaining`, nous, partenaire, tables, four,
   clients.
3. Choisir une cible par machine à états (plat complet → livraison, etc.).
4. Émettre `USE x y` (le referee rapproche si besoin).
5. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
6. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 6 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 5 tours → **0 divergence**.
- Un bug de décalage de ligne a été détecté par les tests dans les trois
  ports compilés (ligne `turnsRemaining` non consommée) et corrigé.
- Validation officielle : **4 test games gagnés** sur 4 (agent 0) :
  - vs Bossyardee (boss) + FabDev06 (rang 1) : `15889 / 14587 / 13940` ;
  - vs hez99 + Elektordi : `13293 / 10678 / 8741` ;
  - vs janfyl + DoudouTouDou : `9372 / 5014 / 4358` ;
  - vs Uhut + am0ka : `10696 / 8089 / 6145`.
- Le port C++ a aussi gagné : `13275 / 12620 / 11973`.
- Conclusion : la stratégie « plat BLUEBERRIES + ICE_CREAM, livraison » suffit
  largement en Wood 4. Les nombreux `WAIT` correspondent à la vaisselle
  épuisée ; un dépôt d'ingrédients sur table quand il n'y a plus de plat
  pourrait encore améliorer le score.

## Cartographie des 4 langages

`solutions/c/bot.c` — 119 lignes
`solutions/cpp/bot.cpp` — 101 lignes
`solutions/python/bot.py` — 95 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 108 lignes

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
