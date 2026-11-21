# 2016 Hypersonic — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2016-hypersonic`. Objectif : **Ligue Légende**.
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
Line 1: 3 integers, `width`, `height` and `myId`. `width` and `height` represent the size of the grid and will always be `13` and `11` respectively, and are given for purely for convenience. `myId` is your player's id.
### Input for one game turn
First `height` lines: a string `row` representing each row of the grid. Each character can be:
- `.` an empty cell.
- `0` a box.
Next line: an integer `entities` for the amount of entities on the grid.
Next `entities` lines : `6` space separated integers `entityType` , `owner` , `x` , `y` , `param1` & `param2` . Represents a player or a bomb.
### Output for one game turn
One of the following:
- `MOVE` followed by two integers `x` and `y`.
- `BOMB` followed by two integers `x` and `y`.
You may append text to your instructions, it will be displayed in the viewer. e.g. `MOVE 1 1 hypersonic`.
### Constraints
Response time per turn ≤ `100` ms
Response time for the first turn ≤ `1000` ms
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Hypersonic (Wood 3)
## Algorithme commun
Init : `width height myId`. Puis chaque tour : 11 lignes de grille (`.` ou
`0`), puis `entities`, puis `entities` lignes `type owner x y param1 param2`.
### Par tour
1. Lire grille et entités. Repérer ma position (`type==0 && owner==myId`) et
   mon nombre de bombes disponibles (`param1`).
2. Marquer les cases occupées par une bombe (toutes, alliées et ennemies) dans
   `blocked`.
3. **Si je peux poser une bombe** (`bombs > 0`) et qu’une boîte est dans ma
   croix de portée 3 (cases non bloquées avant la boîte) :
   - choisir la direction de fuite la plus éloignée des bombes (BFS vers une
     case à distance ≥ 4 de la bombe) ;
   - output `BOMB fuite_x fuite_y`.
4. **Sinon** : BFS depuis ma position (cases traversables = `.` sans bombe)
   vers la boîte accessible la plus proche. Output `MOVE casesuivante_x
   casesuivante_y`. Si aucune boîte accessible, `MOVE` vers le centre.
5. `flush`.
### Portée de bombe
Depuis (x,y), pour chaque direction, avancer case par case jusqu’à 3 :
- si case = bombe → inclure la case puis s’arrêter ;
- si case = boîte → inclure la case puis s’arrêter (et c’est un score) ;
- sinon continuer.
### BFS

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(11)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : repli Python sur `(x,y)` au lieu du centre quand aucune
    boîte n’est accessible → harmonisé avec les 3 autres langages.
- Test game réel : **victoire** contre l’adversaire Wood (rang 0, score
  1000014–1000013). Le score affiché inclut un offset interne mais le rang
  est bien gagnant. Aucun timeout, aucune commande invalide de notre part.
- Conclusion : stratégie Wood suffisante. Les leagues supérieures ajoutent
  dégâts, range et range-up ; il faudra alors simuler les explosions et fuir
  activement.

## Cartographie des 4 langages

`solutions/c/bot.c` — 114 lignes
`solutions/cpp/bot.cpp` — 103 lignes
`solutions/python/bot.py` — 117 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 137 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **8**.
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
