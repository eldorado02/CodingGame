# 2018 Botters Of The Galaxy — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2018-botters-of-the-galaxy`. Objectif : **Ligue Légende**.
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
Line 1: your `team`’s number, can be either 0 or 1.
Line 2:`bushAndSpawnPointCount` the number of bushes and spawn points
Next `bushAndSpawnPointCount` lines: String entityType [Bush, Spawn], Integer `x`, Integer `y`, Integer `radius`
Next Line: `itemCount` the total number of items available in game.
Next `itemCount` lines:
- String `itemName`
- Integer `itemCost`
- Integer `damage`
- Integer `health`
- Integer `maxHealth`
- Integer `mana`
- Integer `maxMana`
- Integer `moveSpeed`
- Integer `manaRegeneration`
- Integer `isPotion` [it’s a potion if it’s equal to 1]
### Input for one game turn
Line 1: your `gold` amount as Integer
Line 2: your opponent’s`gold` as Integer
Line 3: `roundType` an Integer that lets you know if you’re in the Hero pick phase or during the actual game phase.
Line 4: `entityCount` a number that represents the sum of all heroes, units, towers and neutral creatures currently on the map
Next `entityCount` inputs of:
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Botters of the Galaxy
## Algorithme commun
### Init
Lire `team`, `bushAndSpawnPointCount`, puis N lignes `type x y radius`, puis
`itemCount` et `itemCount` lignes d'objets. Tout est ignoré (Wood 6).
### Par tour
1. Lire `gold`, `oppGold`, `roundType`, `entityCount`.
2. Si `roundType < 0` → écrire `HULK`, `flush`, continuer.
3. Sinon, lire `entityCount` lignes ; garder les champs
   `unitId team unitType x y attackRange health max_health shield damage speed stun gold`
   (les champs supplémentaires des HERO sont lus/ignorés).
4. Choisir la cible dans cet ordre :
   - héros ennemi (`team != myTeam`, `unitType == HERO`) ;
   - tour ennemie (`TOWER`) ;
   - unité ennemie (`UNIT`, `GROOT`) ;
   - à défaut `WAIT`.
5. Écrire `ATTACK <id>` (ou `WAIT`), une ligne par héros possédé (`roundType`).
### Cas de test
- `01_pick.in` : `roundType=-1` → `HULK`.
- `02_hero.in` : héros adverse → ATTACK sur lui.
- `03_tower.in` : plus de héros adverse → ATTACK sur la tour.
- `04_wait.in` : aucune cible ennemie → WAIT.
- `05_dead.in` : héros adverse à 0 PV → attaquer la tour.
- `06_order.in` : plusieurs héros ennemis → le plus proche de nous.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK, dont :
  - `01_pick` : phase de pick **avec entités** ;
  - `07_multiturn` : pick puis partie (détecte toute ligne non consommée) ;
  - `08_realformat` : 22 champs sur `TOWER` (format réel du referee).
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(31)`), dont
  30 % de phases de pick avec entités et des lignes à 13 ou 22 champs →
  **0 divergence** entre C, C++, Rust et Python.
- Bugs trouvés et corrigés :
  - **Phase de pick** : le bot sortait `HULK` mais ne consommait pas les
    `entityCount` lignes d'entités → désalignement au tour suivant. Corrigé
    dans les 4 langages (lecture des entités avant le test `roundType < 0`).
  - C : oubli de la ligne `oppGold` ; Rust : `gold`/`oppGold` lus sur une seule
    ligne ; C++ : `continue` sur notre propre HERO sautait la consommation de
    ses 9 champs supplémentaires. Le C++ lit désormais chaque ligne entière
    (`getline` + `istringstream`) et ignore les champs excédentaires.
- Test game réel (adversaire `DoubleHulk`, Wood 6) : **victoire en 48 tours**,

## Cartographie des 4 langages

`solutions/c/bot.c` — 90 lignes
`solutions/cpp/bot.cpp` — 92 lignes
`solutions/python/bot.py` — 73 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 109 lignes

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
