# 2016 Fantastic Bits — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2016-fantastic-bits`. Objectif : **Ligue Légende**.
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
# Fantastic Bits (2016-11-26)
This is a **league based** puzzle.
For this puzzle, multiple versions of the same game are available. Once you have proven your skills on this first version, you will access a higher league and extra rules will be unlocked.
**Edit: Since the challenge, new inputs/changes are available (magic, new states, spell duration/cost). If you want to reuse your challenge code, you will have to take them into account to avoid timeout errors.
See the updated statement for details.**
# The Goal
Score more points than your opponent.
Two players face off in a rectangular map.
Each player controls 2 wizards. To score a point, a Snaffle (a mix between a Snitch and a Quaffle) must go through the opponent's goal.
# Rules
**The map**
The playing field is a rectangle of `16001`x `7501` units. The coordinates `X=0, Y=0` represent the pixel at **the top-left**.
`2` goals, one per player, are located on either side of the field.
Each **goal** is composed of two poles whose centers are separated by `4000` units.
Each **pole** has a radius of `300` units.
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — Fantastic Bits (Wood 2)
## Algorithme commun
Init : `myTeamId`. Puis chaque tour : `myScore myMagic`, `oppScore oppMagic`,
`entities`, puis `entities` lignes `id type x y vx vy state` (type en string).
### Par tour
1. Lire les entités. Séparer `mine` (WIZARD), `snaffles` (SNAFFLE).
2. But adverse = (16000,3750) si team 0, (0,3750) si team 1.
   But propre = l’inverse.
3. Pour chaque sorcier `w` (id croissant) :
   - si `w.state == 1` → `THROW goal_x goal_y 500` ;
   - sinon, choisir un Snaffle cible :
     - sorcier 0 (attaquant) : Snaffle libre le plus proche de `w` ;
     - sorcier 1 (défenseur) : Snaffle le plus proche de **notre but**, s’il
       est dans notre moitié, sinon le plus proche de `w` ;
     - exclure les Snaffles déjà visés par l’autre sorcier.
   - viser `pos + v*3`, avec thrust 150 si distance > 1200, sinon 60.
4. Écrire une ligne par sorcier, `flush`.
### Géométrie
Tout en entier. `anticipated = (x + 3*vx, y + 3*vy)`, borné au terrain.
Distance² pour le choix des cibles.
### Cas de test
- `01_grab.in` : un sorcier `state=1` → THROW vers but adverse.
- `02_chase.in` : Snaffle loin → MOVE thrust 150.
- `03_close.in` : Snaffle proche → MOVE thrust 60.

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires (`random.seed(13)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : Rust lisait les deux lignes de score comme une seule,
    `entities` tombait sur la 2e ligne de score → index hors bornes.
    Corrigé (2 lignes de score + 1 ligne `entities`, comme l’énoncé).
- Test game réel : **victoire 4–0** contre l’adversaire Wood, sans timeout
  ni commande invalide. Le THROW au but adverse et le rôle défenseur
  fonctionnent en conditions réelles.
- Conclusion : stratégie Wood 2 validée. Les leagues supérieures ajoutent
  Bludgers et sorts ; il faudra gérer les extrusions de Snaffle et les
  collisions de sorciers.

## Cartographie des 4 langages

`solutions/c/bot.c` — 91 lignes
`solutions/cpp/bot.cpp` — 89 lignes
`solutions/python/bot.py` — 85 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 132 lignes

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
