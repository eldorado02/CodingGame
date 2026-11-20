# Critique — A Code of Ice and Fire

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Rush trop lent face à l'adversaire | Élevé | Déplacer toutes les unités vers le QG adverse dès le tour suivant leur création. |
| Or négatif → wipeout | Moyen | Limiter les entraînements à `gold // 10` ; chaque capture augmente le revenu. |
| Unités bloquées | Moyen | Entraîner sur les cases jouables distinctes les plus proches du QG adverse. |
| Divergence 4 langages | Moyen | Même BFS, même tri `(distance, x, y)`, fuzz différentiel. |
| Actions invalides (case occupée) | Faible | Les `MOVE` sont émis avant les `TRAIN`, ce qui libère les cases. |

## Après implémentation

- `tests/run.sh` : 10 cas × 3 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 4 tours → **0 divergence**.
- Validation officielle (agent 0) :
  - Python vs boss Snow et 6 joueurs Wood 3 classés 1 à 6 : **7 victoires / 7**,
    scores adversaires toujours `-1` (QG capturé).
  - Ports C (`568`), C++ (`807`) et Rust (`624`) : **3 victoires / 3** vs boss Snow.
- Le bot gagne en capturant le QG adverse avant la limite de 100 tours, sans
  jamais dépendre du score militaire final.
- Améliorations possibles : éviter les `MOVE` qui « restent sur place » quand
  un voisin est occupé (cas observé dans les logs, non bloquant), et coordonner
  les unités pour réduire les files d'attente sur le chemin du QG.
- Conclusion : en Wood 3, une stratégie de rush massif d'unités niveau 1 suffit
  largement ; la complexité des ligues supérieures (niveaux 2/3, mines, tours)
  n'est pas nécessaire ici.
