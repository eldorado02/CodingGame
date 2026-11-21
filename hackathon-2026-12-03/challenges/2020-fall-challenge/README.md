# 2020 Fall Challenge — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2020-fall-challenge`. Objectif : **Ligue Légende**.
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
### Input for One Game Turn
Line 1: one integer `actionCount` for the number of all available client orders.
Next `actionCount` lines: `11` space-separated values to describe a game action.
- `actionId`the id of this action
- `actionType`: a string
  - `BREW` for a potion recipe
- `delta0`, `delta1`, `delta2`, `delta3`: the four numbers describing the consumption/producion of ingredients for each tier.
- `price` the amount of rupees this will win you if this is a potion recipe, `0` otherwise.
- `tomeIndex`: ignore for this league.
- `taxCount`: ignore for this league.
- `castable`: ignore for this league.
- `repeatable`: ignore for this league.
Next `2` lines: `5` integers to describe each player, **your** data is always first:
- `inv0` for the amount of tier-0 ingredients in their inventory
- `inv1` for the amount of tier-1 ingredients in their inventory
- `inv2` for the amount of tier-2 ingredients in their inventory
- `inv3` for the amount of tier-3 ingredients in their inventory
- `score` for the amount of rupees earned so far
### Output
A single line with your command:
- `BREW id`: your witch attempts to brew the potion with the given id.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Fall Challenge 2020
1. Lire les ordres et l'inventaire ; ignorers les champs non utilisés en Wood 4.
2. Filtrer les recettes réalisables, trier par `(-price, somme besoins, id)`.
   Le prix est le 7e champ de la ligne d'ordre.
3. Jouer `BREW id` immédiatement, sinon `WAIT`.
4. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
5. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 6 tours × 4 langages → OK.
- Fuzz différentiel : 200 cas de 6 tours → **0 divergence**.
- Un bug d'indexage du prix (index 5 au lieu de 6) a été détecté par la première
  test game (défaite 17–25 contre Bossrova) puis corrigé.
- Validation officielle (agent 0) : **8 victoires / 8**.
  - Python : 35–27 et 28–0.
  - C : 35–25 et 39–0.
  - C++ : 37–24 et 35–0.
  - Rust : 29–22 et 32–0.
- La stratégie est volontairement minimale : aucun calcul de planification sur
  plusieurs tours n'était nécessaire en Wood 4.

## Cartographie des 4 langages

`solutions/c/bot.c` — 67 lignes
`solutions/cpp/bot.cpp` — 70 lignes
`solutions/python/bot.py` — 46 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 60 lignes

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
