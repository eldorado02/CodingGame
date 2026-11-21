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

## Montée en ligue Wood 1 — 4 courses simultanées (soumission `41773894`)
Le bot BFS « jamais de collision » restait bloqué à `0=6063 1=6436` après
promotion : avec **4 machines jouant en parallèle**, une seule action commune
par tour, et un score final qui est un **produit** (bronze dans une seule course
⇒ score 0), optimiser chaque course indépendamment est sous-optimal. Le BFS
choisissait tantôt `LEFT`, tantôt `UP`, sans jamais coordonner les 4 pistes.

Nouvelle stratégie retenue :
- énumérer les `4^5 = 1024` séquences d'actions sur un horizon de 5 tours ;
- simuler les 4 courses avec le **modèle exact** (`UP` saute la 1re haie,
  collision ⇒ 3 tours d'étourdissement, arrivée à `len(GPU)-1`) ;
- minimiser la somme des tours restants estimés `(5 + last - pos)` ;
- jouer le **premier pas** de la meilleure séquence.

Pourquoi l'horizon 5 : 6 dégénère en timeout (`scores: 0=-1`), 4 est trop myope
(1/5), 5 donne **10/10 victoires** contre `5507122` + `5507574` (scores
19440→64800, jamais sous 13500). Le gain vient de ce que la séquence tient
compte des haies sur les 4 pistes et de l'étourdissement, alors que le boss
joue presque toujours `UP` et se cogne.

Validation : 10 test games consécutifs gagnés, **210/210 cas différentiels**
identiques dans 4 langages (générateur étendu à `nbGames` 1..4 avec resets
partiels). Soumission officielle : `41773894`.
