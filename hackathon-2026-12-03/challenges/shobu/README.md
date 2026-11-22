# Shobu — guide pour reprendre le travail

Challenge CodinGame bot-programming. Objectif : **Ligue Légende**.

## État courant

- Boss Wood 4 : `6906517` (greedy, ne regarde pas la réponse).
- Prototype retenu : **v6** = always `SWAP` quand c'est légal + évaluation
  1 ply avec réponse adverse (24 premières réponses dans l'ordre du referee)
  + terme positionnel sur les bords de board (poids 25).
- Résultat Python : **4/5** contre le boss Wood 4 (un lot ; confirmation en cours).
- Port 4 langages terminé, fuzz **200/200** identique.
- Aucune soumission pour l'instant ; validation C++ réelle avant soumission.
- Voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md`.

## Ordre de lecture

1. `statement.md` — énoncé officiel (à relire à chaque promotion).
2. `research.md` — sources et mécanique exacte du referee.
3. `plan.md` — algorithme et règle de départage.
4. `critique.md` — bilan après validation.
5. `solutions/` — 4 implémentations.
6. `tests/` — cas déterministes et fuzz différentiel.

## Contrat d'entrée / sortie

Entrée : `round myColor`, 8 lignes de 8 caractères (`.`/`b`/`w`),
action adverse (6 entiers, ou `-`/`SWAP`), `actionCount`, puis les actions
légales. Sortie : `px py ax ay dx dy` ou `SWAP` au premier tour blanc.

## Pièges déjà rencontrés

- `sys.stdin.read()` bloque : CodinGame garde le flux ouvert. Lire avec
  `input()` ligne par ligne.
- L'action adverse occupe 6 tokens ; ne pas n'en sauter qu'un seul.
- Le générateur doit exiger `sameBoard(x, y, u, k)` **avant** d'indexer les
  cases intermédiaires d'un coup agressif, sinon `IndexError`.
- Les ports doivent évaluer les **24 premières** réponses dans l'ordre du
  referee, pas les 24 meilleures après tri : sinon divergence observée.

## Tests

`sh tests/run.sh` compare les 4 implémentations sur 200 cas de fuzz.
Critère de soumission : fuzz vert + ≥3/5 test games contre le boss.
