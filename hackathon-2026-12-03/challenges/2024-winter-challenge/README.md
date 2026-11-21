# 2024 Winter Challenge — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2024-winter-challenge`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernières soumissions connues :
- `41776467 (Wood 3)`
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
### Initialization Input
First line: two integers `width` and `height` for the size of the grid.
### Input for One Game Turn
First line: one integer `entityCount` for the number of entities on the grid.
Next `entityCount` lines: the following `7` inputs for each entity:
- `x`: X coordinate (`0` is leftmost)
- `y`: Y coordinate (`0` is topmost)
- `type`:
- `WALL` for a wall
- `ROOT` for a ROOT type organ
- `BASIC` for a BASIC type organ
- `A` for an A protein source
- `owner`:
- `1` if you are the owner of this organ
- `0` if your opponent owns this organ
- `-1` if this is not an organ
- `organId`: unique id of this entity if it is an organ, `0` otherwise
- `organDir`: `N`, `W`, `S`, or `E`, not used in this league
- `organParentId`: if it is an organ, the `organId` of the organ that this organ grew from (0 for `ROOT` organs), else `0`.
- `organRootId`: if it is an organ, the `organId` of the `ROOT` that this organ originally grew from, else `0`.
Next line: `4` integers: `myA`,`myB`,`myC`,`myD` for the amount of each protein type you have.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Cellularena (Wood)
1. Récupérer énoncé, stub et adversaires via `cg.py` ; lire le moteur officiel
   pour fixer l'économie des protéines, le protocole et les conditions de fin.
2. Écrire le bot Python :
   - lire la grille complète chaque tour dans un tableau de cases
     (`WALL` / organe / protéine) ;
   - BFS multi-source depuis tous les organes dont `owner == 1`, voisinage
     `N, E, S, W`, en mémorisant pour chaque case libre : id du parent, premier
     pas, distance ;
   - choisir une case contenant `A` (priorité), sinon la case libre la plus
     proche ; départage `(distance, y, x)` ;
   - émettre `GROW <parent> <x> <y> BASIC` puis des `WAIT` jusqu'à
     `requiredActionsCount`.
3. Porter en C, C++, Rust en gardant le même ordre de voisinage et le même
   départage, pour garantir des sorties bit à bit identiques.
4. Générer 10 scénarios déterministes + 200 cas de fuzz contenant des organes
   `BASIC` des deux camps et le convention `owner = 1` pour le joueur local ;
   comparer les 4 implémentations via `tests/run.sh`.
5. Jouer des test games officiels contre le boss Wood `5736694` dans les 4
   langages (5 parties chacun) puis mettre à jour l'index et committer.
Complexité : `O(width × height)` par tour en temps et en mémoire, soit ~162 cases
sur la grille Wood, très en deçà des 50 ms par tour.

## Dernier bilan de validation (extrait de `critique.md`)

## Montée en ligue Wood 3 — HARVESTER (soumission `41776467`)
L'énoncé générique affirme que seul `BASIC` existe dans les ligues Wood, mais
les métadonnées des test games prouvent que **Wood 3 active le HARVESTER**
(`growthTypes_harvester_0=1`, `harvestedProteins_A_1=2`). La défaite 16–17
venait d'un détail précis : le bot absorbait lui-même la source que son propre
HARVESTER récoltait — le `GROW BASIC` vers une source plus lointaine posait le
premier organe **sur** la source harvesée (`gained 3A from absorption` au lieu
du `gained 1A from harvest` répété du boss).

Correction décisive : les cases sources d'un HARVESTER sont **bloquées dans le
BFS** (et pas seulement exclues de la liste des objectifs). Le chemin vers une
autre source ne peut plus traverser la source récoltée.

Validation : **9/9 parties gagnées** contre le boss Wood 3 `5736695` (scores
18–17 et 19–18 ; deux erreurs réseau `gaierror` exclues). Le prototype précédent
sans blocage BFS plafonnait à 3/5. **210/210 cas différentiels** identiques dans
4 langages, avec un générateur étendu aux organes HARVESTER, aux 4 types de
protéines et aux stocks C/D non nuls. Soumission officielle : `41776467`.

## Cartographie des 4 langages

`solutions/c/bot.c` — 167 lignes
`solutions/cpp/bot.cpp` — 173 lignes
`solutions/python/bot.py` — 114 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 10 lignes
`solutions/rust/bot.rs` — 248 lignes
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
- Montée en ligue Wood 3 — HARVESTER (soumission `41776467`).

## Points d'amélioration à tester

1. Affiner le choix des sources déjà harvestées (bloquées dans le BFS).

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
