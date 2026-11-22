# Elemental Wars — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : valider Wood 3 puis viser la
promotion avec la soumission C++ `41786190`.

## Algorithme retenu

1. Lire l'init (tribu, taille, grille) puis, chaque tour, les scores et les 3
   élémentaux.
2. Calculer deux BFS depuis la proie et le prédateur (murs infranchissables).
3. Évaluer cinq candidats (rester + 4 voisins) :
   `score = -100 * d(proie) + 20 * min(d(prédateur), 6)` ;
   bonus `+10000` pour une capture immédiate ; malus `-10000` pour entrer sur
   le prédateur ; malus `-2000` si le prédateur est à ≤1 et la proie à >1.
4. Départage déterministe : score décroissant, puis `x` croissant, puis `y`
   croissant.
5. Émettre `id MOVE x y` (ou `id WAIT` si prisonnier).

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → **200/200** identiques.
- Test games C++ : **5/5** contre `6910638,6697588`.
- Soumission C++ : `41786190`.

## Pièges rencontrés

- `sys.stdin.read()` bloque : lire avec `input()` et répondre immédiatement.
- La tribu `P` a pour prédateur `F` (et non `P`) : bug de port corrigé dans les
  3 langages compilés.
- Un prédateur prisonnier doit être ignoré (coordonnées sentinelles `100,100`).
