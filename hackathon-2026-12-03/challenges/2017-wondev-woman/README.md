# 2017 Wondev Woman — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2017-wondev-woman`. Objectif : **Ligue Légende**.
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
Line 1: an integer `size` for the size of the square grid.
Line 2: an integer `unitsPerPlayer` for the number of units each player controls. *Always `1` for this league.*
### Input for one game turn
First `size` lines: `size` characters representing one `row` of the grid. A dot (`.`) for an unplayable cell or an integer `height` for the cell's current level.
Next `unitsPerPlayer` lines: `unitX`, `unitY` the coordinates of one of your units.
Next `unitsPerPlayer` lines: `otherX`, `otherY` the coordinates of one of your opponent's units.
If the unit is not visible to you, the values will be `-1 -1`.
Next line: `legalActions`, an integer for the number of valid actions you may play this turn.
Next `legalActions`lines: `atype`, `index`, `dir1`, `dir2`: the parameters of a valid command you may output this turn.
For instance, in the action `MOVE&BUILD 0 N SW`, `atype` is `MOVE&BUILD`, `index` designates the unit at index `0`, which may move north and build to the south-west.
### Output for one game turn
One line: `MOVE&BUILD` followed by `index` for the unit you wish to perform the action *(Always `0` for this league)*, `dir1` for the direction in which the unit will move, `dir2` for the direction of the cell the unit will raise the level of.
You may append text to your action, it will be displayed in the viewer.
The possible directions are: `N`, `NE`, `E`, `SE`, `S`, `SW`, `W`, `NW`.
### Constraints
`5` ≤ `size` ≤ `7`
`unitsPerPlayer` = `1`
`0` ≤ `legalActions` ≤ `64`
Response time per turn ≤ `50`ms
Response time for the first turn ≤ `1000`ms
What is in store for me in the higher leagues?
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Wondev Woman
## Algorithme commun
Init : `size`, `unitsPerPlayer` (1). Puis chaque tour :
1. Lire `size` lignes de grille (caractères `0`–`3` ou `.`).
2. Lire position unité (`ux uy`), adversaire (`ox oy`).
3. Lire `legalActions`, puis les lignes `atype index dir1 dir2`.
4. Pour chaque action, calculer :
   - `dest = (ux,uy) + delta[dir1]`, `h_dest = grille[dest]` (`-1` si hors/trou) ;
   - `build = dest + delta[dir2]`, `h_build = grille[build]`.
5. Score :
   - `h_dest == 3` → +1 000 000 (victoire) ;
   - déni (`h_build == 3` et `build` adjacent à l'adversaire visible) → +50 000 ;
   - `+1000 * h_dest` (ou -100 000 si `-1`) ;
   - `h_build == 2` → +300, `== 1` → +100, `== 3` ailleurs → -50.
6. Sortir l'action au meilleur score : `MOVE&BUILD 0 dir1 dir2`, ou `WAIT`.
### Directions
`N(0,-1) NE(1,-1) E(1,0) SE(1,1) S(0,1) SW(-1,1) W(-1,0) NW(-1,-1)`
### Cas de test
- `01_win.in` : une action mène sur une case 3 → victoire.
- `02_climb.in` : deux montées possibles → choisir la plus haute.
- `03_deny.in` : adversaire peut gagner → détruire sa case 3.
- `04_build.in` : deux cases à construire → préférer élever une case 2.
- `05_hidden.in` : adversaire invisible (`-1 -1`) → pas de déni.
- `06_hole.in` : grille avec trous `.` → hauteur -1.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK (victoire, montée,
  déni, construction, adversaire invisible, trous).
- Fuzz différentiel : 200 grilles/actions aléatoires (`random.seed(23)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Test game réel : **victoire 1–0** ; niveau 3 atteint au tour 11 sans
  commande invalide ni timeout. La priorité « victoire immédiate » a joué :
  dès qu'une montée sur une case 3 est dans `legalActions`, elle est sortie.
- Conclusion : l'évaluation des actions fournies suffit en Wood 3. Les ligues
  supérieures (2 unités, PUSH, visibilité partielle) demanderont une vraie
  recherche de coups et la gestion de l'adversaire.

## Cartographie des 4 langages

`solutions/c/bot.c` — 77 lignes
`solutions/cpp/bot.cpp` — 78 lignes
`solutions/python/bot.py` — 75 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 122 lignes
`solutions/rust/target` — 0 lignes

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
