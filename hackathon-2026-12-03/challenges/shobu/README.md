# Shobu — guide pour reprendre le travail

Challenge CodinGame bot-programming. Objectif : **Ligue Légende**.

## État courant

- Boss Wood 3 : `6906535` (`BOSS 2`, MCTS avec playouts courts).
- Version retenue : minimax déterministe à **budget de 20 000 évaluations**,
  alpha-beta, beam 24 à la racine / 16 aux nœuds, profondeurs 1→4, évaluation
  de course aux pushes (plateau adverse le plus faible, matériel, bords/coins).
- Résultats Wood 3 : **4 victoires + 1 nulle / 5** contre `6906535`, sans
  timeout. Le budget temps de 82 ms donnait 5/5 mais n'était pas reproductible
  dans les 4 langages ; le budget de nœuds (200) timeoutait à ~180 ms.
- Port 4 langages terminé, fuzz **200/200** identique.
- Soumission C++ `41786861` (promotion immédiate en Wood 2). Contre le boss
  Wood 2 `6906553` : 1V+1N+3D ; variantes testées sans gain.
- Voir `critique.md` et `../../bot-programming/IMPROVEMENTS.md`.

## Ordre de lecture

1. `statement.md` — énoncé officiel (relu après promotion en Wood 3).
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

- `sys.stdin.read()` bloque : CodinGame garde le flux ouvert. Lire ligne à ligne.
- L'action adverse occupe 6 tokens sur une seule ligne.
- Le générateur doit exiger `sameBoard(x, y, u, k)` avant d'indexer les cases
  intermédiaires d'un coup agressif.
- Les ports doivent utiliser le même budget d'évaluations et le même ordre de
  tri (score décroissant, puis move lexicographique) pour rester identiques.

## Tests

`sh tests/run.sh` compare les 4 implémentations sur 200 cas de fuzz.
Critère de soumission : fuzz vert + ≥3/5 test games contre le boss.
