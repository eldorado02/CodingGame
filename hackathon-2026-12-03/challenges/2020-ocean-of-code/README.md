# 2020 Ocean Of Code — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2020-ocean-of-code`. Objectif : **Ligue Légende**.
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
# Ocean of Code (2020-03-20)
This is a **league based** challenge.
Wood leagues should be considered as a tutorial which lets players discover the different rules of the game.
In Bronze league, all rules will be unlocked and the real challenge will begin.
## The Goal
This game is based on the board game Captain Sonar.
You pilot a submarine and you know that an enemy is present near you because you are listening to its radio frequency communication. You don't know exactly where it is but you can hear all its orders. You and your opponent have `6` hit points. When a player's hit points reach `0`, the player loses.
## Rules
## Definitions
- Submarines move on a map constituted of water and islands. They can only move on cells with water. They can share the same cell without colliding.
- The map is `15` cells in width and `15` cells in height. Coordinates start at (0,0) which is the top left cell of the map.
- The map is split in `9` sectors, which contain `25` cells each (5x5 blocks of cells). The top left sector is `1`. The bottom right sector is `9`.
## Beginning of the game
At the beginning of the game, you'll receive a map (15x15 cells) that indicates the position of islands. Islands are obstacles. You cannot move or fire through islands. Then, you will decide where you want to place your submarine by indicating a coordinate (x,y).
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Ocean of Code
1. Lire la grille 15×15 et choisir une case eau très connexe, proche du centre.
2. Maintenir `visited` (notre chemin) et `bel` (positions adverses possibles).
3. Chaque tour : lire position/PV/cooldowns/sonar/ordres adverses.
4. Appliquer le retour de notre dernier torpedo à `bel`, puis les ordres adverses.
5. Choisir un déplacement sûr (Warnsdorff + centroïde de `bel`), charger le torpedo.
6. Si le torpedo est prêt, choisir la meilleure cible à portée 4 et tirer.
7. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
8. Validation : cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 10 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 6 tours → **0 divergence**.
- Un bug de parsing C (imbrication de `strtok` pour les ordres séparés par `|`)
  a été détecté par les tests puis corrigé.
- Validation officielle (agent 0) :
  - Python : **5 victoires / 5** (boss Cpt. Levine, XSWL, Tyir, kvvck,
    Zefir13000), scores `6/6/6/6/5` contre `0/-1/0/-1/-1`.
  - Ports C (`4–0`), C++ (`4–0`) et Rust (`5–0`) : **3 victoires / 3**.
- Le bot gagne en restant à 4–6 PV et en coulant l'adversaire avant la fin ;
  le `SURFACE` de secours n'a jamais été nécessaire lors des test games.
- Amélioration possible : conserver un historique de chemin adverse par
  position pour exploiter l'interdiction de revisite, et utiliser le sonar en
  ligue supérieure.
- Conclusion : la combinaison « croyance simple + torpedo maximal + marche
  Warnsdorff » suffit largement ici.

## Cartographie des 4 langages

`solutions/c/bot.c` — 343 lignes
`solutions/cpp/bot.cpp` — 304 lignes
`solutions/python/bot.py` — 262 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 347 lignes

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
