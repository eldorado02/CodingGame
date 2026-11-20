# Critique — Olymbits

## Avant implémentation
| Risque | Mitigation |
|---|---|
| Arrivée mal identifiée | Lecture du source : `maxX = length - 1` = 29. |
| Saut mal modélisé | `UP` ignore uniquement la première case. |
| Étourdissement sous-estimé | 2 tours perdus : toute transition vers `#` est écartée. |
| Score mal compris | Or = 3 points, argent = 1, bronze = 0. |

## Après implémentation
- `tests/run.sh` : 10 scénarios déterministes + 200 cas de fuzz, **0 divergence**
  entre Python, C, C++ et Rust.
- Deux divergences initiales venaient d'un décalage de lecture des registres
  dans les ports C/C++ (la première ligne de registres n'était pas lue) et
  d'une cible d'arrivée erronée (27 au lieu de 29). Corrigées.
- Test games officiels (boss Wood `5507121` et adversaires `5556390`,
  `5507397`, `5556004`) : **7/7 victoires**, score `21 – 7 – 7` à chaque fois,
  soit 7 médailles d'or sur 7 courses. Validation Python, C, C++ et Rust.
- Limite de l'approche : le bot ignore délibérément les positions adverses.
  C'est optimal ici car la course s'arrête dès le premier arrivé et qu'un
  chemin sans collision bat toujours les adversaires. En ligue supérieure,
  avec 4 mini-jeux simultanés, il faudrait traiter chaque registre et arbitrer
  les actions selon la course la plus menacée.
