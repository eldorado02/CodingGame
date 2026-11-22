# 2018 Xmas Rush — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2018-xmas-rush`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernière soumission retenue : Silver `41785676` (C++), score 12.16 contre
  boss Silver `2215206` à 20.76 (ligue Silver, pas encore promu).
- Prototype Silver `v17` retenu : évaluation des poussées avec distance de
  Manhattan même quand la cible est déconnectée (`500 - 30*d`),
  anti-répétition de ligne, correction de la transposition de `simulate()` et
  du collect instantané. **3/5 puis 3/5** en Python, **9 victoires / 15** en
  C++ contre le boss Silver.
- Port 4 langages terminé : fuzz **210/210** identique.
- Voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md`.
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
# Xmas Rush (2018-12-07)
This is a **league based** challenge.
Wood leagues should be considered as a tutorial which lets players discover the different rules of the game.
In Bronze league, all rules will be unlocked and the real challenge will begin.
## The Goal
Make your way to the items on the board and be the first to complete your quests!
## Rules
The game is played by 2 players on a 7x7 board with 49 square tiles. The (0,0) coordinate corresponds to the top left corner.
Each player has 1 tile placed outside the board which they can use to push a row or a column on the board, trying to make a path toward their quest items. When a user pushes a tile into the board, the tile that gets pushed out will become the new player's tile.
**The board**
- The board contains square tiles with paths on them. A path can lead to one of the four directions (`UP`, `RIGHT`, `DOWN` and `LEFT`).
- Some tiles have an item on them.
**Quest**
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Xmas Rush (2018-12-07)
## Algorithme
1. **Lecture** : `turnType`, 7×7 masques `URDL`, 2 joueurs, items, quêtes.
2. **Cible** : item dont `itemPlayerId == 0` et dont le nom est la quête révélée.
3. **Tour MOVE** : BFS sur les 49 tuiles (deux masques doivent se connecter),
   sortir jusqu'à 20 directions, ou `PASS`.
4. **Tour PUSH** : simuler 14 poussées (7 lignes × 2 sens, 7 colonnes × 2 sens),
   décaler joueur et item, recalculer la distance BFS ; choisir le minimum.
5. **Livrables** : `statement.md`, `research.md`, `plan.md`, `critique.md`,
   `solutions/{c,cpp,rust,python}`, `tests/{gen_cases.py,run.sh}`.
6. **Validation** : cas déterministes + fuzz différentiel 200 cas + test games
   CodinGame (Wood 3).

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 4 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 4 tours → **0 divergence**.
- Deux bugs corrigés en chemin :
  - `sys.stdin.read()` bloquait sur stdin non fermé → passage aux lignes ;
  - le bot quittait après un tour → timeout dès le tour suivant ; boucle ajoutée.
- Validation officielle : **4 test games gagnés** contre St Nicolas (boss Wood 2,
  agent `2194292`), M Qasim Aslam (rang 1), aymané212 (rang 2) et vexilla72
  (rang 3) ; scores `0=1 1=0` à chaque fois.
- Le port C++ a également gagné contre le boss (`0=1 1=0`).
- Conclusion : un BFS court + une poussée greedy suffisent en Wood. Le vrai
  gain viendrait d'une évaluation à 2 coups (poussée adverse puis la nôtre),
  mais la complexité de portage est élevée pour un gain marginal à ce niveau.

## Cartographie des 4 langages

`solutions/c/bot.c` — 220 lignes
`solutions/cpp/bot.cpp` — 179 lignes
`solutions/python/bot.py` — 170 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 190 lignes

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
