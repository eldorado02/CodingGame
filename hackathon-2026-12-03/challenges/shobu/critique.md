# Shobu — critique

## Avant implémentation

- La promotion en Wood 3 change d'adversaire : le greedy Wood 4 est remplacé
  par un MCTS à playouts courts qui reconnaît les positions résolues.
- L'ancien bot Wood 4 (1 ply + 24 premières réponses) perd **0/5** contre le
  boss Wood 3 ; sans swap il obtient 2 nulles sur 5 mais jamais de victoire.
- La victoire est une course aux éliminations : les logs montrent des plateaux
  vidés dès le tour 3 et des manches de 18 à 43 tours.

## Après implémentation

### Tentatives

- `v6` Wood 4 (1 ply + 24 réponses + bords, swap) : **0/5** en Wood 3.
- `v6` sans swap : **0V+2N/5**.
- `r200` (24 réponses remplacées par les 200, toujours 1 ply) : **0V+3N/5**.
- `deep` (minimax itératif, chrono 82 ms, beam 24/16) : **5/5** (4–0 à chaque
  manche) mais non reproductible dans les 4 langages.
- `fixed3` / `fixed4` (profondeur fixe) : 1V/5 et timeout systématique.
- `nodes200` (budget de nœuds) : timeout systématique ; instrumentation :
  ~50 000 évaluations et ~180 ms par tour.
- `work18000` (budget d'évaluations) : **3V+2N/5**.
- `work20000` : **4V+1N/5** puis **4V+1N/5**, retenu.
- `work22000` : **3V+1N+1 timeout** sur la version du dépôt, rejeté.
- `work26000` : 3V+1N+1 timeout, rejeté.

## Leçons

- Un budget temps gagne mais casse l'exigence de fuzz identique ; un budget de
  nœuds est déterministe mais ne borne pas le vrai coût (génération + tri +
  évaluation de chaque coup).
- Le budget d'évaluations est la bonne unité : il est déterministe et corrélé
  au temps réel.
- Le swap reste joué systématiquement : même contre un boss qui ne swap pas,
  il n'a pas empiré les résultats dans les séries testées.
