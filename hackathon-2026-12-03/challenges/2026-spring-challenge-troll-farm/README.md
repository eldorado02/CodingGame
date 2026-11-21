# 2026 Spring Challenge Troll Farm — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2026-spring-challenge-troll-farm`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernières soumissions connues :
- `41773233 (Wood 1)`
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
# Spring Challenge 2026 — Troll Farm (2026-05-12)
This is a **league-based** challenge.
Wood leagues should be considered as a tutorial which lets players discover the different rules of the game.
In Bronze league all rules will be unlocked and the real challenge will begin.
To see the full rules, go [here](https://www.codingame.com/demo/1423805f9c28c9654cfd4d57b0208df71315c49) and switch leagues at the top-right corner.
# The Goal
You control a pack of trolls. Make them collect the most resources.
# Rules
Each player starts with a shack and a troll. Control your troll to collect fruits and then bring the resources back to your shack. The resources can be used to train more trolls or plant trees. The game is played on a grid, cells have different terrains and trees on them.
**Troll attributes**
Each troll has `4` attributes: `movementSpeed`, `carryCapacity`, `harvestPower` and `chopPower`. They can't be changed.
**Trees**
A tree has a `type` (`PLUM`, `LEMON`, `APPLE`, `BANANA`), a `size`, a `health`, an amount of `fruits` and a `cooldown`. When cooldown reaches `0`, the tree either grows in size or, if at maximum size (`4`), produces fruits. Trees can hold up to `3` fruits.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Troll Farm (Wood)
1. Récupérer l’énoncé, le stub, la liste des adversaires et le moteur officiel
   (`cg.py stub` / `cg.py opponents` / dépôt `eulerscheZahl/Troll-Farm`) pour
   fixer le protocole exact, la durée (100 tours), les attributs Wood et le
   calcul du score.
2. Observer une vraie partie contre le boss Wood `6479367` avec un bot sonde
   qui journalise les entrées sur `stderr`, afin de confirmer le format des
   arbres, des trolls et les commandes jouées par le boss.
3. Écrire le bot Python :
   - lire la grille une seule fois, mémoriser la cabane `0` et les cases
     `GRASS` (`.`) comme seules cases praticables ;
   - lire exactement les lignes de chaque tour dans une boucle `while True` ;
   - BFS depuis la position réelle du troll vers toutes les cases herbeuses,
     en conservant la distance et le premier pas ;
   - si le troll porte un fruit : `DROP` dès que la distance de Manhattan à
     la cabane vaut 1, sinon `MOVE` d’un pas vers la case herbeuse adjacente
     à la cabane la plus proche (départage `distance, y, x`) ;
   - sinon : `HARVEST` si un arbre fruitier est sur la case courante, sinon
     `MOVE` d’un pas vers l’arbre fruitier atteignable le plus proche
     (départage `distance, -fruits, y, x`) ;
   - `WAIT` si aucun fruit n’est disponible.
4. Porter en C, C++, Rust avec le même ordre de voisinage
   `(N, E, S, O)` et le même départage, pour garantir des sorties bit à bit
   identiques. Chaque langage garde ses idiomes : tableaux statiques et BFS à

## Dernier bilan de validation (extrait de `critique.md`)

## Montée en ligue Wood 1 (soumission officielle)
- Après promotion automatique, le boss Wood 1 (`6479368`) dispose de 3 trolls
  et plante des arbres : le bot Wood 0 perdait 0/3 (scores 77–110, 87–116,
  85–128).
- Nouvelle stratégie : `TRAIN` d’un troll polyvalent au premier tour (choix du
  triplet `movementSpeed / carryCapacity / harvestPower` maximum abordable),
  contrôle de **tous** les trolls, récolte prioritaire, retour vers une case
  adjacente à la cabane, déplacement BFS multi-source avec évitement des cases
  occupées.
- Validation : **5/5 victoires** contre `6479368` (155–128, 136–133, 137–123,
  123–108, 149–92). 210/210 cas différentiels identiques dans les 4 langages.
- Nouvelle soumission officielle : `41773233`.

## Cartographie des 4 langages

`solutions/c/bot.c` — 195 lignes
`solutions/cpp/bot.cpp` — 181 lignes
`solutions/python/bot.py` — 118 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 10 lignes
`solutions/rust/bot.rs` — 261 lignes
`solutions/rust/target` — 0 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **10**.
- Harnais : `bash tests/run.sh` — #!/usr/bin/env bash # Différentiel 4 langages : 10 cas déterministes + 200 cas de fuzz..
- Le générateur `tests/gen_cases.py` couvre les cas listés dans son docstring ;
  vérifier avant d'ajouter un cas qu'il est rejoué dans les 4 langages.
- Critère de soumission : **≥3/5 victoires** contre le boss de la ligue courante,
  en plus du fuzz intégralement vert.

## Pièges déjà rencontrés

Voir les entrées correspondantes dans
[`../../bot-programming/IMPROVEMENTS.md`](../../bot-programming/IMPROVEMENTS.md).
Sections présentes dans `critique.md` :
- Avant implémentation
- Après implémentation
- Montée en ligue Wood 1 (soumission officielle).

## Points d'amélioration à tester

1. Affiner le choix des sources déjà harvestées (bloquées dans le BFS).
2. Tester un horizon de recherche différent en mesurant le pire cas (attention au timeout).

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
