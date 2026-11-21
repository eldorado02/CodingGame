# 2017 Ghost In The Cell — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2017-ghost-in-the-cell`. Objectif : **Ligue Légende**.
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
Line 1:`factoryCount`, the number of factories.
Line 2:`linkCount`, the number of links between factories.
Next `linkCount` lines: 3 space-separated integers `factory1`, `factory2` and `distance`, where `distance` is the number of turns needed for a troop to travel between `factory1` and `factory2`.
### Input for one game turn
Line 1: an integer `entityCount`, the number of entities.
Next `entityCount` lines: an integer `entityId`, a string `entityType` and 5 integers `arg1`, `arg2`, `arg3`, `arg4` and `arg5`.
If `entityType` equals `FACTORY` then the arguments are:
- `arg1`: player that owns the factory: `1` for you, `-1` for your opponent and `0` if neutral
- `arg2`: number of cyborgs in the factory
- `arg3`: factory production (between 0 and 3)
- `arg4`: unused
- `arg5`: unused
If `entityType` equals `TROOP` then the arguments are:
- `arg1`: player that owns the troop: `1` for you or `-1` for your opponent
- `arg2`: identifier of the factory from where the troop leaves
- `arg3`: identifier of the factory targeted by the troop
- `arg4`: number of cyborgs in the troop (positive integer)
- `arg5`: remaining number of turns before the troop arrives (positive integer)
### Output for one game turn
The available actions are:
- `MOVE` `source` `destination` `cyborgCount`: creates a troop of `cyborgCount` cyborgs at the factory `source` and sends that troop towards `destination`. Example: `MOVE 2 4 12` will send 12 cyborgs from factory 2 to factory 4.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Ghost in the Cell
## Algorithme commun
Init : `factoryCount`, `linkCount`, puis `linkCount` lignes `f1 f2 dist`.
Floyd-Warshall → `dist[i][j]` (INF si pas de lien direct).
### Par tour
1. Lire `entityCount`, puis les entités :
   - `FACTORY id owner cyborgs prod _ _`
   - `TROOP id owner src dst count eta`
2. `force[i]` = cyborgs de l’usine i (owner connaissable) ; pour chaque troupe :
   `force[dst] += count` si elle est à nous, `force[dst] -= count` sinon.
   Usines sans entité = 0.
3. Défense : pour chaque usine `i` à nous avec `force[i] < 0`, chercher une
   usine `j` à nous avec `force[j] > -force[i] + 5` ; envoyer
   `min(force[j]-5, -force[i]+2)` vers i. Garder le meilleur (force la plus
   négative).
4. Expansion : pour chaque usine `src` à nous (force > 5) et chaque cible
   `dst` neutre ou ennemie :
   - `need = (owner[dst] == 0 ? force[dst] : force[dst]) + 1`
   - si `force[src] - 5 >= need`, envoyer `need`.
   - score : `100` si neutre + `prod[dst]*20` - `dist[src][dst]*2` - `need`.
5. Output le meilleur `MOVE src dst n`, sinon `WAIT`, `flush`.
### Cas de test
- `01_two.in` : 2 usines, neutre proche → MOVE capture.
- `02_defense.in` : troupe ennemie arrive → renfort.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires (`random.seed(17)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bugs trouvés :
    - Rust : `factoryCount` et `linkCount` lus sur une seule ligne (ils sont
      sur deux lignes distinctes) → index hors bornes.
    - C/C++/Rust : la force d’une troupe était appliquée pendant la lecture
      des entités ; si une `FACTORY` était lue après une `TROOP` visant la
      même usine, elle écrasait le renfort. Corrigé : stocker les troupes puis
      les appliquer après toutes les usines.
- Test game réel : **victoire 346–5** contre l’adversaire Wood, sans timeout.
- Conclusion : la stratégie « forces effectives + une action globale » suffit
  largement en Wood 3. Les ligues supérieures demanderont plusieurs actions
  par tour, bombs et upgrade de production.

## Cartographie des 4 langages

`solutions/c/bot.c` — 80 lignes
`solutions/cpp/bot.cpp` — 81 lignes
`solutions/python/bot.py` — 103 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 131 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **6**.
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
