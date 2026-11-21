# 2017 Mean Max — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2017-mean-max`. Objectif : **Ligue Légende**.
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
# Mean Max (2017-11-17)
This is a **league based** puzzle.
For this puzzle, multiple versions of the same game are available. Once you have proven your skills on this first version, you will access a higher league and extra rules will be unlocked.
## The Goal
Gather more water than your enemies
## Rules
The game is played on a circular map with 3 players, each spawning with a Reaper.
Players move simultaneously and compete to gather water from the Wrecks of Tankers.
**The map**
- The game is played in a circular area of radius `6000`.
**The Reapers**
- The Reaper harvests `1` water per turn from each Wreck it finishes the turn in.
- Your Reaper can `X Y THROTTLE` to accelerate towards `(x,y)`.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Mean Max
## Algorithme commun
Pas d'init. Chaque tour :
1. Lire 3 scores, 3 lignes « rage » (placeholders), `unitCount`, puis les
   unités : `id type playerId mass radius x y vx vy extra extra2`.
2. Repérer le Faucheur (`type==0 && playerId==0`) et les épaves (`type==4`,
   `extra` = eau restante).
3. Générer les candidats `(tx, ty, power)` :
   - `(w.x, w.y, p)` pour chaque épave et `p ∈ {0, 100, 200, 300}` ;
   - `(x-vx, y-vy, 150)` et `(x-vx, y-vy, 300)` (freinage) ;
   - `WAIT` (pas de poussée).
4. Pour chaque candidat, simuler un tour :
   - `accél = 2*p * (cible - pos) / distance` (0 si `p==0` ou distance nulle) ;
   - `nv = v + accél` ;
   - `np = round_half_away(pos + nv)` ; `nv = round_half_away(nv * 0.8)`.
5. Score = `1e6 * épaves(np dans rayon) + 10 * eau cumulée - distance_min(np) - |nv|`.
6. Sortir le meilleur candidat (`X Y p` ou `WAIT`), puis 2 lignes `WAIT`.
### Cas de test
- `01_reach.in` : une épave proche → accélérer vers elle.
- `02_stop.in` : déjà dans l'épave mais vitesse forte → freiner.
- `03_stay.in` : dans l'épave, vitesse nulle → WAIT.
- `04_choose.in` : deux épaves → privilégier la plus proche/riche.
- `05_none.in` : aucune épave → WAIT.
- `06_overlap.in` : deux épaves superposées → rester dans les deux.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(29)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bugs trouvés : `inf` vs `1e18` pour « aucune épave », et `best` initialisé
    à `-inf` vs `-1e18`. Harmonisé sur `1e18` / `-1e18` dans les 4 langages.
  - Le générateur de cas écrivait aussi 3 entiers par ligne de score au lieu
    d'un seul → corrigé.
- Physique validée contre le referee officiel (accél = `2*P` vers le point
  cible, arrondi demi-loin-de-zéro, friction 0.2).
- Test game réel (2 adversaires Wood 3 : MiniMax + snarmph) : **victoire
  50–20–21**, seuil de 50 atteint sans commande invalide ni timeout.
- Conclusion : la simulation 1 pas suffit en Wood 3. Les ligues supérieures
  (Looters multiples, Tankers, sorts) demanderont une gestion des collisions
  et des rôles.

## Cartographie des 4 langages

`solutions/c/bot.c` — 110 lignes
`solutions/cpp/bot.cpp` — 105 lignes
`solutions/python/bot.py` — 110 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 148 lignes

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

1. Affiner le choix des sources déjà harvestées (bloquées dans le BFS).
2. Remplacer la heuristique par un minimax à profondeur fixe; valider par test game.

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
