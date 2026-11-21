# 2022 Cultist Wars — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2022-cultist-wars`. Objectif : **Ligue Légende**.
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
# Cultist Wars (2022-09-19)
# The Goal
The aim of this game is to gain as many followers as possible. You win if your opponent's number of units drops to 0 or if you have more units than your enemy by the last round.
# Rules
The game is played on a grid **13** units wide and **7** units high. The coordinate **X=0**, **Y=0** is the top left tile. The positions of obstacles and neutral units is randomly generated, but is symmetric.
The source code of the game is available here: https://bitbucket.org/Nixerrr/cultist-wars/src
**Units**, there are three types:
- **Neutral** unit: Technically cultists but move randomly and cannot shoot until converted.
  ![](https://i.imgur.com/945L5N7.jpg)
- **Cult Leader:** This is your initial unit and you have only one of it. It can `WAIT`, `MOVE` or `CONVERT` neutral and enemy units. It has 10 hp.
  ![](https://i.imgur.com/ULyMdiD.jpg)
- **Cultist:** Neutral units become cultists when you convert them. It can `WAIT`, `MOVE` or `SHOOT` enemy units. It has 10 hp.
  ![](https://i.imgur.com/lWTU23n.jpg)
**Turns** play out in the following way:
1. A player performs  **1 action / turn**. The action can be perfomed by any of the player's units.
2. A random neutral unit moves in a random direction.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Cultist Wars
1. Extraire l'énoncé et le stub officiel via l'API CodinGame.
2. Lire le source du moteur pour confirmer Bresenham, portée et pathfinding.
3. Écrire le bot Python : menaces, conversions, tirs, déplacement par `SHOOT`.
4. Porter en C, C++ et Rust avec les mêmes règles de départage.
5. Générer 10 scénarios déterministes et comparer les 4 sorties.
6. Fuzz différentiel 200 cas.
7. Jouer des test games contre les 4 meilleurs bots Bronze et documenter les résultats.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation
- `tests/run.sh` : 10 scénarios × 4 langages → sorties identiques.
- Fuzz différentiel : 200 scénarios aléatoires → **0 divergence**.
- Une divergence initiale venait du départage des tirs : Python utilisait la
  distance à la cible visée, les ports la distance à l'unité réellement touchée.
  Python a été aligné sur les ports.
- Test games CodinGame (adversaires Bronze `2411205`, `4779806`, `4524271`,
  `4459794`) : la configuration finale a gagné 2 parties sur 4 dans le dernier
  lot ; les itérations précédentes ont produit 23 victoires sur 80 parties.
- Limite de l'approche : la stratégie est réactive et ne coordonne pas les
  cultistes pour encercler le leader adverse. Amélioration possible : réserver
  deux culistes à la chasse du leader ennemi pendant que le leader convertit.

## Cartographie des 4 langages

`solutions/c/bot.c` — 260 lignes
`solutions/cpp/bot.cpp` — 222 lignes
`solutions/python/bot.py` — 187 lignes
`solutions/rust/Cargo.toml` — 10 lignes
`solutions/rust/bot.rs` — 303 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **10**.
- Harnais : `bash tests/run.sh` — #!/usr/bin/env bash set -euo pipefail.
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
