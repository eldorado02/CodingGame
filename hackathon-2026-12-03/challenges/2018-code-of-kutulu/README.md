# 2018 Code Of Kutulu — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2018-code-of-kutulu`. Objectif : **Ligue Légende**.
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
Line 1  : `width` an integer for the width of the map
Line 2  : `height` an integer for the height of the map
Next `height` lines  : A string representing each row of the map starting from the top. Each character encodes a cell as follows
- `#` : wall
- `w` : spawn for wanderers
- `.` : empty cell
One line:  4 integers to read. No signification yet.
### Input for one game turn
First line:  An integer `entityCount` for the number of entities on the field (including spawning  minions).
Next `entityCount` lines:  Each entity represented as follows:
- String `entityType` the type of entity amongst those: `EXPLORER` | `WANDERER`
- Integer `id` the unique identifier of a unit, -1 for an effect
- Integers `x` & `y` the position of the entity.
- Integer `param0`
  - Explorer: sanity
  - Spawning minion: time before spawn
  - Wanderer: time before being recalled
- Integer `param1` :
  - Explorer: ignore for this league
  - Minion: Current state amongst those -> `SPAWNING = 0` , `WANDERING = 1`
- Integer `param2` :
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Code of Kutulu
## Algorithme commun
### Init
Lire `width`, `height`, les `height` lignes de la carte (`#` mur, `w` spawn,
`.` vide), puis la ligne des 4 entiers ignorés.
### Par tour
1. Lire `entityCount`, puis les lignes
   `entityType id x y param0 param1 param2`.
   - Première ligne = nous (`mx, my`, sanity).
   - `EXPLORER` autres → liste des autres explorateurs (avec sanity).
   - `WANDERER` avec `param1 == 1` (WANDERING) → liste des menaces.
2. Candidats : `(mx, my)` + 4 voisins dans la carte et non-murs.
3. Pour chaque candidat `p` :
   - `reassured` = ∃ autre explorateur avec `|dx|+|dy| ≤ 2` ;
   - `dw` = distance Manhattan minimale à un wanderer actif (100 si aucun) ;
   - `de` = distance Manhattan minimale à un autre explorateur (100 si aucun) ;
   - `score = (100000 si reassured sinon 0) + 100*dw - de`.
4. Choisir le meilleur score (égalité → premier dans l'ordre :
   rester, haut, bas, gauche, droite), écrire `WAIT` ou `MOVE x y`.
### Cas de test
- `01_reassure.in` : déjà à 2 d'un explorateur → rester (pas de wanderer).
- `02_alone.in` : seul → se rapprocher du plus proche explorateur.
- `03_flee.in` : réassuré mais wanderer à distance 1 → fuir tout en restant réassuré.
- `04_wall.in` : voisin mur → ne pas y aller.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 labyrinthes aléatoires (`random.seed(41)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Choix de conception validé : le poids 100000 de la réassurance prime sur
  la fuite ; `- de` pousse à rester collé au groupe (un explorateur peut
  partager la case d'un allié, aucune règle ne l'interdit).
- Deux bugs Rust corrigés en chemin : lecture des dimensions sur une seule
  ligne (au lieu de deux) et inférence du closure de lecture.
- Test game réel (Wood 3, 3 adversaires obligatoires) : opposants
  **Davy Jones (boss), just_code2 (rang 1), KPricorn (rang 2)** →
  **victoire**, scores `0=74 1=71 2=55 3=50`, rang `0 1 2 3`.
  Aucune commande invalide de notre agent ; l'adversaire $1 a émis une
  commande invalide au frame 249.
- Conclusion : la stratégie « réassurance + fuite des wanderers » suffit en
  Wood 3. Les ligues supérieures ajoutent les Sanity Loss, recherches et
  objets : il faudra un vrai arbre de recherche (paranoid) et une gestion

## Cartographie des 4 langages

`solutions/c/bot.c` — 67 lignes
`solutions/cpp/bot.cpp` — 65 lignes
`solutions/python/bot.py` — 61 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 92 lignes

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
