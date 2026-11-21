# 2022 Spring Challenge — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2022-spring-challenge`. Objectif : **Ligue Légende**.
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
Line 1: two integers `baseX` and `baseY` for the coordinates of your base. The enemy base will be at the opposite side of the map.
Line 2: the integer `heroesPerPlayer` which is always `3`.
### Input for One Game Turn
First 2 lines: two integers `baseHealth` and `mana` for the remaining health and mana for both players. Your data is always given first. *You may ignore the mana for this league.*
Next line: `entityCount` for the amount of game entities currently in play.
Next `entityCount` lines: 11 integers to describe each entity:
- `id`: entity's unique id.
- `type`:
  - `0`: a monster
  - `1`: one of your heroes
  - `2`: one of your opponent's heroes
- `x` & `y`: the entity's position.
- `shieldLife`: *ignore for this league*.
- `isControlled`: *ignore for this league*.
The next 5 integers only apply to monsters (will be `-1` for heroes).
- `health`: monster's remaining health points.
- `vx` & `vy`: monster's current speed vector, they will add this to their position for their next movement.
- `nearBase`: `1`: if monster is **targeting** a base, `0` otherwise.
- `threatFor`:
  - With the monster's current trajectory — if `nearBase` is `0`:
    - `0`: it will never reach a base.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Spring Challenge 2022
1. Lire la base, les 3 héros et les entités du tour.
2. Ne conserver que les monstres avec `threatFor == 1`.
3. Estimer leur prochaine position (400 vers notre base si `nearBase == 1`,
   sinon `x + vx`, `y + vy`).
4. Trier par `(distance estimée à la base, pv, x, y)`, puis assigner les héros
   au plus proche disponible.
5. `MOVE` vers la cible, ou rester sur place si déjà à portée de coup.
6. Livrables : `solutions/{c,cpp,rust,python}`, `tests/`, `critique.md`.
7. Validation : 10 cas déterministes + fuzz 200 cas + test games CodinGame.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 10 cas × 8 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 6 tours → **0 divergence**.
- Deux erreurs corrigées pendant la mise au point :
  1. `px, py = bx, by if threat == 1 else ...` en Python affectait `py` à un
     tuple au lieu de la paire de coordonnées ;
  2. `std::tie` en C++ ne peut pas lier des temporaires, remplacé par
     `std::make_tuple`.
- Validation officielle (agent 0) : **8 victoires / 8**, toujours `0=1 1=0`
  (notre base garde 1 PV, celle de l'adversaire est détruite) :
  - Python, C, C++, Rust : 2 victoires chacun contre BOSS 1 Wood 2 (4258166)
    et absoluce (4260028).
- La défense est volontairement pure : les sorts et la brume ne font pas partie
  de la première ligue et ne sont pas implémentés.
- Amélioration possible : regrouper les héros pour infliger 4 ou 6 dégâts à un
  monstre résistant, et intercepter les monstres non-ciblants avant qu'ils
  n'atteignent le rayon de 5000.

## Cartographie des 4 langages

`solutions/c/bot.c` — 114 lignes
`solutions/cpp/bot.cpp` — 100 lignes
`solutions/python/bot.py` — 83 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 134 lignes

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
