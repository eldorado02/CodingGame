# 2026 Summer Challenge Back Track King — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2026-summer-challenge-back-track-king`. Objectif : **Ligue Légende**.
Ce README est écrit pour qu'une autre IA puisse comprendre et améliorer le bot
sans deviner. Mettre à jour **avant** toute modification du code.

## État courant

- Dernières soumissions connues :
- `41773207 (Wood 1)`
- `41773849 (Bronze)`
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
`myId`: your player id. `0` or `1` .
`width`: number of cells in a row of the map.
`height`: number of cells in a column of the map.
Next `height`*`width` lines: two integers to describe each cell of the map, from left to right, top to bottom:
- `regionId`: the id of the region this cell is a part of.
- `type`: the terrain type of this cell ( `0-2` )
`townCount`: number of towns on the map.
Next `townCount` lines:
- `townId`: unique identifier of this town.
- `townX`: X position of this town ( `0` is left-most).
- `townY`: Y position of this town ( `0` is top-most).
- `desiredConnections`:
- A string of comma-separated `townIds`. e.g. " `1,2,4` "
- " `x` " if this town has no desired connections.
### Input for one game turn
`myScore`: your points.
`foeScore`: your opponent's points.
Next `height`*`width` lines: state of each cell, in the same order they were given before:
- `trackOwner`:
- `-1` if this cell has no track.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Back Track King (Wood)
1. Récupérer énoncé, stub, adversaires et AI starter officiel ; identifier le
   boss Wood `6767604` et l’objectif de la ligue 1.
2. Sonde sur une vraie partie : journaliser les dimensions, le terrain, les
   villes et les états de cases, puis vérifier le comportement de `AUTOPLACE`
   avec des coordonnées fixes.
3. Écrire le bot Python :
   - lire les entrées comme un flux de **jetons** (tampon de lignes), et non
     comme un nombre fixe de lignes ;
   - mémorier `townId -> (x, y)` et toutes les paires demandées ;
   - choisir la paire minimisant `(Manhattan, id demandeur, id cible)` ;
   - émettre chaque tour la même commande
     `AUTOPLACE x1 y1 x2 y2`.
4. Porter en C, C++, Rust avec le même départage. C/C++ lisent déjà des jetons
   via `scanf` / `>>` ; Rust reçoit un lecteur de jetons avec `VecDeque`.
5. Générer 10 scénarios déterministes + 200 cas de fuzz (dimensions, terrain,
   villes et `desiredConnections` aléatoires, deux tours par scénario) et
   comparer les 4 implémentations via `tests/run.sh`.
6. Jouer 5 test games officiels par langage contre le boss Wood `6767604` ;
   absorber les 429 (20 parties / 300 s) en attendant le délai indiqué.
7. Rédiger `research.md` / `critique.md`, mettre à jour l’index des challenges
   et `steps.json`, puis committer et pousser.
Complexité : `O(townCount²)` pour le choix de la paire (au plus 12 villes),
puis `O(width × height)` jetons lus par tour. Aucun calcul de chemin : le moteur

## Dernier bilan de validation (extrait de `critique.md`)

## Montée en ligue Bronze (soumission officielle `41773849`)
- Nouvelle stratégie : lecture des connexions **actives** depuis l'entrée (4e
  champ de chaque case), construction de la paire désirée non active la moins
  chère (Dijkstra ornée : rail existant = 0, case libre = coût terrain + 1), et
  DISRUPT ciblé sur la région non-ville où l'adversaire possède le plus de rails,
  avec priorité aux régions traversées par une connexion active.
- Point décisif : quand toutes les connexions sont actives (~75 % de la partie,
  comme le confirment les post-mortems publics), le bot ne gaspille plus sa
  peinture : il pose jusqu'à 3 rails adjacents aux chemins qui paient, ce qui
  détourne le tie-break N/E/S/O et vole des points au boss.
- Validation : **8/10 victoires** contre le boss Bronze `6767605` (scores
  14442–13825, 5647–5384, 24787–22023, 2552–1976, 9457–7015, 9560–7421,
  32020–24725, 3161–2971). 210/210 cas différentiels identiques dans 4 langages.
- Soumission officielle : `41773849`.

## Cartographie des 4 langages

`solutions/c/bot.c` — 237 lignes
`solutions/cpp/bot.cpp` — 202 lignes
`solutions/python/bot.py` — 183 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 10 lignes
`solutions/rust/bot.rs` — 254 lignes
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
- Montée en ligue Wood 1 (soumission officielle)
- Montée en ligue Bronze (soumission officielle `41773849`).

## Points d'amélioration à tester

1. Tester un horizon de recherche différent en mesurant le pire cas (attention au timeout).

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
