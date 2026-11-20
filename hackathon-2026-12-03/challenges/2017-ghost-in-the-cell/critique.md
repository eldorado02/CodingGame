# Critique — Ghost in the Cell

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Vider une usine puis la perdre | Élevé | Réserve de 5 sur la source. |
| Sous-estimer une garnison | Élevé | Forces effectives avec troupes en approche. |
| Action unique mal choisie | Moyen | Score global sur tous les couples. |
| Défense tardive | Moyen | Troupes en approche incluses dans `force`. |
| Timeout 50 ms | Faible | Floyd-Warshall en init seulement. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires (`random.seed(17)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bugs trouvés :
    - Rust : `factoryCount` et `linkCount` lus sur une seule ligne (ils sont
      sur deux lignes distinctes) → index hors bornes.
    - C/C++/Rust : la force d’une troupe était appliquée pendant la lecture
      des entités ; si une `FACTORY` était lue après une `TROOP` visant la
      même usine, elle écrasait le renfort. Corrigé : stocker les troupes puis
      les appliquer après toutes les usines.
- Test game réel : **victoire 346–5** contre l’adversaire Wood, sans timeout.
- Conclusion : la stratégie « forces effectives + une action globale » suffit
  largement en Wood 3. Les ligues supérieures demanderont plusieurs actions
  par tour, bombs et upgrade de production.
