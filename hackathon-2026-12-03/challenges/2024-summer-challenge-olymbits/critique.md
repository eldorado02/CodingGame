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

## Montée en Bronze — correction du crash sur registres négatifs (soumission `41776913`)
En Bronze, les 4 mini-games tournent en parallèle et le bot Wood 1 interprète
tous les registres comme des positions de course d'obstacles. L'archerie stocke
cependant une **coordonnée x signée** dans `reg[player]` : le bot indexait la
piste avec une valeur négative et plantait (`IndexError: string index out of
range`) dans ~30 % des parties, ramenant le score à `-1`.

Correction : ramener toute position négative à `0` avant la simulation, dans
les 4 langages (Rust parsem déjà les valeurs invalides à 0 via `unwrap_or`). Le
générateur de fuzz couvre désormais explicitement les registres négatifs.

Validation : **210/210 cas différentiels** identiques dans 4 langages (générateur
étendu), puis **13/13 parties gagnées** contre le boss Bronze `5507123`
(adversaire `5507574`), scores 0 contre 0 pour le boss. Soumission officielle :
`41776913`.

## Montée en Silver — décision jointe sur les 4 mini-jeux (soumission `41778730`)
- Constat : le bot Bronze traite les 7 registres de **chaque** machine comme
  une piste de course d'obstacles. En Silver, les 4 machines tournent en
  parallèle et utilisent des formats différents ; score de ligue effondré à
  `5,8` (rang 1227) contre `18,93` pour le boss.
- Nouvelle stratégie : classifier chaque machine d'après `GPU` et `reg6`,
  calculer un score entier par action possible (LEFT/DOWN/RIGHT/UP) dans
  chaque mini-jeu, les pondérer (haies ×3, roller ×2, plongée ×2, archerie ×1)
  et jouer l'action totale maximale.
  - haies : temps restant estimé par simulation greedy sur 3 tours après le
    premier pas (`UP` saute, collision ⇒ 3 tours d'étourdissement) ;
  - archerie : optimisation de la distance euclidienne sous forme de carrés
    entiers, avec correction greedy sur les 5 prochains vents ;
  - roller : progression selon l'ordre `GPU` (`ULDR`), pénalité de risque,
    de collision et de reset ;
  - plongée : gain du combo si l'action correspond au goal du tour.
- Validation : **3 victoires / 5** contre le boss Silver `5507124` et le rank 1
  `6914836` (scores 4320, 5940, 10010) contre **0/5** pour l'ancien bot sur la
  même fenêtre. Fuzz étendu aux 4 types de machines : **210/210** cas
  identiques dans 4 langages. Soumission C++ `41778730`.
- Bug de portage détecté par le fuzz : la liste des adversaires de roller
  contenait un 0 fantôme dans les 3 langages compilés, ce qui créait une
  collision illusoire quand `pos=0`. Corrigé en passant la longueur réelle.
