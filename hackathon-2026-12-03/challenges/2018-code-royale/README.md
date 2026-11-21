# 2018 Code Royale — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2018-code-royale`. Objectif : **Ligue Légende**.
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
Line 1: an integer `numSites`, indicating the number of building sites on the map.
Next `numSites` lines: 4 integers `siteId` `x` `y` `radius`
- `siteId`: The numeric identifier of the site
- `x y`: The numeric coordinates of the site's center
- `radius`: The radius of the site
### Input for one game turn
Line 1 and 2: Two integers `gold` `touchedSite`
- `gold`: the current amount of gold available to be spent
- `touchedSite`: the id of the site your Queen is touching, or `-1` if none
Next `numSites` lines: Seven integers `siteId` `ignore1` `ignore2` `structureType` `owner` `param1` `param2`
- `siteId`: The numeric identifier of the site
- `ignore1`: *Reserved for future leagues.*
- `ignore2`: *Reserved for future leagues.*
- `structureType`:
  - `-1`: No structure
  - `2`: Barracks
- `owner`:
  - `-1`: No structure
  - `0`: Friendly
  - `1`: Enemy
- `param1`:
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Code Royale
## Algorithme commun
### Init
Lire `numSites`, puis les N lignes `siteId x y radius` dans un tableau indexé
par `siteId`.
### Par tour
1. Lire `gold touchedSite`.
2. Lire N lignes `siteId i1 i2 structureType owner param1 param2` ; mettre à
   jour l'état de chaque site.
3. Lire `numUnits`, puis les lignes `x y owner unitType health` ; repérer les
   deux reines (`unitType == -1`).
4. **Cible de construction** : parmi les sites `structureType == -1` ou nos
   barracks non en entraînement, choisir celui qui minimise
   `dist(site, reineEnnemie) + 0.5 * dist(site, notreReine)`, avec
   `dist(site, reineEnnemie) > 150` si possible.
5. **Action reine** :
   - si un barracks ennemi est à moins de 250 de notre reine →
     `MOVE <x du site> <y du site>` (destruction par contact) ;
   - sinon → `BUILD <siteId> BARRACKS-KNIGHT` vers la cible.
6. **TRAIN** : pour chaque site avec `structureType == 2`, `owner == 0`,
   `param1 == 0`, `param2 == 0` (KNIGHT), tant que `gold >= 80`, ajouter
   l'id au `TRAIN` et décrémenter `gold` de 80.
7. Écrire les 2 lignes.
### Cas de test

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(37)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : Rust oubliait le filtre `distance < 250` sur la destruction
    des barracks ennemis, et choisissait un `MOVE` même pour une cible lointaine.
    Corrigé.
- Physique validée contre le referee officiel : KNIGHT = 4 unités / 80 or /
  5 tours de build / vitesse 100 / 30 PV / 1 dégât par tour à la reine ;
  ARCHER n'attaque jamais la reine (donc écarté).
- Test games réels (Wood 3) :
  - adversaire par défaut : **victoire en 98 tours**, scores `0=26`, `1=0` ;
  - **Duchess Boss** (boss Wood 3) : **victoire en 94 tours**, scores `0=24`,
    `1=0`.
  - Aucune commande invalide ni timeout ; la reine ennemie meurt dans les deux cas.
- Conclusion : le rush KNIGHT suffit en Wood 3. Les ligues supérieures (TOWER,
  MINE, GIANT, or réduit) demanderont une vraie gestion économique et défensive."})

## Cartographie des 4 langages

`solutions/c/bot.c` — 105 lignes
`solutions/cpp/bot.cpp` — 99 lignes
`solutions/python/bot.py` — 101 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 139 lignes

Conventions : C11 (`gcc -O2 -std=c11 -Wall -Wextra`), C++20 (`g++ -O2 -std=c++20`),
Rust release (`cargo build --release`, pas d'`unwrap` dans la logique), Python 3.11+
(`typing`, `main()`). Même règle de départage dans les 4 langages.

## Tests

- Cas déterministes commités : **7**.
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
