# 2016 Codebusters — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2016-codebusters`. Objectif : **Ligue Légende**.
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
Line 1: an integer `bustersPerPlayer` for the amount of busters each team controls.
Line 2: an integer `ghostCount` for the amount of ghosts available on the map.
Line 3: an integer `myTeamId` for the team you are playing as.
### Input for one game turn
Line 1: an integer `entities` for the amount of entities on the map that are visible to you.
Next `entities` lines : `6` space separated integers `entityId` , `x` , `y` , `entityType` , `state` & `value` . Represents a buster or a ghost.
### Output for one game turn
One line for each buster: one of the following:
- `MOVE` followed by two integers `x` and `y`
- `BUST` followed by one integer `ghostId`
- `RELEASE`
You may append text to your instructions, it will be displayed in the viewer.
### Constraints
2 ≤ `bustersPerPlayer` ≤ 5
8 ≤ `ghostCount` ≤ 28
Response time per turn ≤ `100` ms
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — CodeBusters
## Algorithme commun
Entrée : `bustersPerPlayer`, `ghostCount`, `myTeamId`. Puis, chaque tour :
`entities`, puis `entities` lignes `id x y type state value`.
Base : `(0,0)` si team 0, `(16000,9000)` sinon.
### Par tour
1. Lire toutes les entités. Séparer `mine` (type == teamId), `ghosts`
   (type == -1). Ignorer les busters adverses (non nécessaires en Wood).
2. **Réservations** : ensemble des ids de fantômes déjà choisis par un buster
   ce tour.
3. Pour chaque buster `b` (id croissant):
   - s’il porte un fantôme (`state == 1`) :
     - si `dist(b, base) <= 800` → `RELEASE` ;
     - sinon `MOVE` vers un point à 800 de `b` sur la droite vers la base.
   - sinon, choisir le fantôme visible non réservé minimisant
     `dist(b, ghost)` ; s’il n’y en a pas, `MOVE` vers le centre (800).
   - avec le fantôme choisi `g` et `d = dist(b, g)` :
     - `d < 900` → `MOVE` vers un point à 800 de `b` **en sens opposé** à `g`
       (recul pour revenir dans la bande) ;
     - `900 <= d <= 1760` → `BUST g.id` (réserver `g`) ;
     - `d > 1760` → `MOVE` vers un point à 800 de `b` vers `g`.
   - Arrondi des coordonnées à l’entier le plus proche ; bornes [0,16000]
     et [0,9000].
4. Écrire une ligne par buster, `flush`.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(7)`) → **0 divergence**.
- Test game réel : le bot tourne en boucle sans timeout (score 3–5 contre
  Bosster en Wood). Les 4 fantômes manquants sont dus à une stratégie encore
  simple (poursuite du plus proche), pas à un bug de protocole.
- Améliorations possibles (Wood+) : départage par distance à notre base,
  anticipation de fuite à 400, gestion des contestations 2v1.

## Cartographie des 4 langages

`solutions/c/bot.c` — 109 lignes
`solutions/cpp/bot.cpp` — 88 lignes
`solutions/python/bot.py` — 87 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 138 lignes

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

1. Remplacer une heuristique par une recherche exacte sur un sous-problème et mesurer le gain en test games.
2. Ajouter au fuzz un cas limite non couvert (égalité, reset, entrée vide) puis vérifier les 4 langages.
3. Comparer les métadonnées de test game (scores, summary) pour trouver une cause mécanique de défaite.

Pour chaque tentative : **une seule modification**, fuzz 200+ cas, puis test games
contre le boss. Écrire le résultat dans `critique.md` et dans le journal global,
que le gain soit positif ou négatif.
