# 2018 A Star Craft — guide pour reprendre le travail

Challenge CodinGame bot-programming, dossier `2018-a-star-craft`. Objectif : **Ligue Légende**.
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
# A*Craft (2018-10-19)
# The Goal
Score as many points as possible by guiding your robots on the platforms.
# Rules
You must guide multiple `Automaton2000` robots by placing arrows on your first turn. Every turn, you score as many points as the number of Automaton2000 robots still functioning.
**The map**
- The game is played on a grid `19` units wide and `10` units high.
- The grid is **torus-shaped**, meaning exiting the grid from one side will wrap you round to the other side.
- The coordinate `X=0, Y=0` is the top left cell.
- Each cell can be of 1 of 3 different types: platform, arrow and void.
**Automaton2000**
- Automaton2000 moves in a straight line by 1 cell every turn.
- Automaton2000 only changes direction when stepping on an arrow.
- Automaton2000 will stop functioning if it revisits a state (position + direction) it's been in.
- Automaton2000 will stop functioning if it steps off the platform.
**Arrows**
```

> ⚠️ Le contrat ci-dessus est celui de l'énoncé archivé (ligue d'origine). À chaque promotion, relire l'énoncé réel : `python3 ../../bot-programming/tools/cg.py show <pretty_id>`.

## Architecture décisionnelle (extrait de `plan.md`)

# Plan — A*Craft
## Algorithme commun
### Init
Lire 10 lignes de 19 caractères. `arrow[y][x] = 0..3` pour `URDL`
préexistantes, `-1` sinon (case libre) ; `void[y][x]` pour `#`.
Lire `robotCount`, puis les `(x, y, dir)` des robots.
### Construction (une passe, ordre `order`)
`visits[x][d]` = nombre de fois où l'état `(case, direction d)` a été
enregistré. Pour chaque robot de `order` :
1. `d = arrow[y][x]` si flèche préexistante, sinon direction fournie.
   Enregistrer l'état initial `(x, y, d)`.
2. Tant que le nombre de pas < 760 :
   - `nx = (x + dx[d]) mod 19`, `ny = (y + dy[d]) mod 10` ;
   - si `(nx, ny)` est `#` → arrêt ;
   - choisir `nd` parmi `0..3` tels que `visits[nx][nd] == 0` :
     - si `(nx, ny)` a une flèche fixe → `nd` imposé ;
     - sinon évaluer chaque `d2` : `look = sum(visits[(nx,ny)+d2])`,
       clé `(look, d2)` ; le minimum gagne ; si `d2 != d`, poser la flèche
       `(nx, ny, d2)` ;
   - si aucun `nd` valide → arrêt ;
   - enregistrer `(nx, ny, nd)`, `x,y,d = nx,ny,nd`.
3. Accumuler les pas dans le score de cette passe.
On exécute la construction avec `order = [0..n-1]` puis avec l'ordre
inversé ; on garde la passe au meilleur score (égalité → ordre original).

## Dernier bilan de validation (extrait de `critique.md`)

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 cartes aléatoires (murs + flèches fixes +
  robots, `random.seed(53)`) → **0 divergence**.
- Deux bugs corrigés en chemin :
  - Python lisait tout stdin d'un coup → timeout sur CodinGame
    (stdin reste ouvert entre les tours) ;
  - indexage des robots en pas de 3 au lieu de 1 après passage aux lignes.
- Découverte importante : une flèche placée sous un robot au tour 0 change
  sa direction initiale. Le cas `Buggy Robot` passe de **1 à 22 points**.
- Validation officielle : **30/30 testcases joués sans erreur**, total
  **2487 points**. Exemples : Simple 23, Dual 46, Buggy 22, Roundabout 78,
  Portal 24, Multiple 3x3 62, One Long Road 104, Hard Choice 165,
  To the Right 231, StarCraft 144, Xel 178.
- Le port C++ a été rejoué sur le testcase 13 : **165 points**, identique
  au Python.
- Conclusion : la stratégie gloutonne « flèche seulement si le droit

## Cartographie des 4 langages

`solutions/c/bot.c` — 150 lignes
`solutions/cpp/bot.cpp` — 140 lignes
`solutions/python/bot.py` — 99 lignes
`solutions/rust/Cargo.lock` — 7 lignes
`solutions/rust/Cargo.toml` — 11 lignes
`solutions/rust/bot.rs` — 162 lignes

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
