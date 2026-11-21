# 2016 Smash The Code — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2016-smash-the-code`. Objectif : **Ligue Légende**.
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
# Smash The Code
Welcome to the First League
Multiple versions of the same game are available. Once you have proven your skills on this first version, you will access a higher league and extra rules will be unlocked.
# The Goal
Defeat your opponent by grouping colored blocks.
# Rules
Each player plays in their own grid `6` cells wide and `12` cells high. Every turn, both players are given two connected blocks, one on top of the other, they must place inside their grid. This is called a pair. The blocks work as follows:
- Every pair of block has a random color. The colors are labeled `1` to `5`.
- You are given the next pairs of blocks to place `8` turns in advance.
- Both players are given the same pairs of blocks.
The placement of blocks works as follows:
- The blocks are dropped into the grid from above, and stop once they reach the bottom or another block in the same column.
- When `4` or more blocks of the same color line up adjacent to each other, they disappear. Blocks connect horizontally or vertically, but not diagonally. The whole group need **not** be a straight line.
- When all groups of the grid have cleared, the blocks above them all fall until they reach the bottom or another block. If this causes new groups to form, those groups will also disappear. This is called a **chain**.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Smash The Code
## Algorithme (commun aux 4 langages)
```
read A, B (8 fois), my_score, 12 lignes, opp_score, 12 lignes
for x in 0..5:
    g = copie(grille)
    r = premiere ligne libre depuis le bas dans la colonne x
    if r - 1 < 0: continue              // top out -> coup illégal
    g[r-1] = B ; g[r-2] = A
    (cleared, chains) = resoudre(g)
    if cleared == 0:
        score = -hauteur_colonne(x) * 3 - voisins_incoherents
    else:
        score = cleared*10 + (chains-1)*25
                - hauteur_moyenne*0.5 - trous*1
    garder le meilleur x
print x
```
`resoudre(g)` :
```
chains = 0 ; total = 0
loop:
    visited = 6x12 faux
    a_clear = 6x12 faux ; found = false

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Test différentiel : 200 grilles aléatoires générées avec `random.seed(42)` →
  **0 divergence** entre C, C++, Rust et Python.
- Bugs réels trouvés et corrigés pendant la validation :
  - C/C++ : condition inversée dans `drop` (première case vide au lieu de la
    première case occupée) → coups illégaux partout.
  - Rust : `max_by_key` départageait vers la droite ; remplacé par `>` explicite.
  - Rust : gravité collectait bas→haut mais réécrivait haut→bas → ordre des
    blocs inversé, faux chains.
  - Python : parsing des lignes de grille (1 token = 1 ligne, pas 6 cases) ;
    crânes adjacents marqués depuis le mauvais tableau.
- Protocole CodinGame (découvert via un test game réel, pas visible dans les
  cas locaux) :
  - le nombre de paires de couleurs est **variable** (8 puis les restantes) ;
    la ligne de score est celle qui ne contient qu’un seul entier ;
  - le programme doit **boucler** : un tour = une entrée complète + une

## Cartographie des 4 langages

`solutions/c/bot.c` — 160 lignes
`solutions/cpp/bot.cpp` — 177 lignes
`solutions/python/bot.py` — 163 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 221 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **6**.
- Harnais : `bash tests/run.sh` — #!/bin/sh # Compare les 4 implémentations sur tous les cas déterministes..
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
