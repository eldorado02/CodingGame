# Critique — Green Circle

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Valeur exacte des points bâclés mal modélisée | Élevé | S’appuyer sur `possibleMoves` pour les releases et n’utiliser le score que pour classer. |
| Cartes de bureau invisibles | Moyen | Évaluer chaque destination comme si elle fournissait sa compétence. |
| Règle de la 5e application | Moyen | Déléguée au moteur via la liste des releases possibles. |

## Après implémentation

- `tests/run.sh` : 10 cas × 12 phases × 4 langages → OK.
- Fuzz différentiel : 200 cas simples + 1000 cas avec mains multiples → **0 divergence**.
- Une test game de diagnostic a révélé que les bureaux ne figurent pas dans
  `cardLocations` ; le bot s’appuie donc sur `possibleMoves` plutôt que sur une
  lecture directe des cartes de bureau.
- Validation officielle (agent 0) : **17 victoires / 18**.
  - Python : 8/8 (boss et trois joueurs Wood 2, plus 4 revanches contre le boss).
  - C : 2/2, C++ : 2/2.
  - Rust : 5/6 (défaite isolée 4–5 contre le boss, puis 4 victoires 5–4).
- Toutes les victoires se terminent sur le score `5–4` : la stratégie gagne une
  application d’avance plutôt que par départage de Technical Debt.
- Amélioration possible : estimer l’épuisement des bureaux en suivant les
  déplacements des deux joueurs, et pénaliser les destinations déjà visitées.
