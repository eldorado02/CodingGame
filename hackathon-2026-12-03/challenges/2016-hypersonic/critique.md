# Critique — Hypersonic

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Bombe posée à côté d’aucune boîte | Élevé | Ne poser que si la croix de portée 3 contient ≥ 1 boîte. |
| Se bloquer soi-même | Moyen | BFS évite toutes les bombes ; fuite après pose. |
| Timer / chaîne mal anticipés | Moyen | Wood : pas de dégât ; on optimise seulement le placement. |
| `MOVE` vers case bombe | Élevé | On renvoie la case suivante calculée par notre propre BFS. |
| Divergence entre langages | Moyen | Même BFS, cas déterministes + fuzz différentiel. |

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(11)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : repli Python sur `(x,y)` au lieu du centre quand aucune
    boîte n’est accessible → harmonisé avec les 3 autres langages.
- Test game réel : **victoire** contre l’adversaire Wood (rang 0, score
  1000014–1000013). Le score affiché inclut un offset interne mais le rang
  est bien gagnant. Aucun timeout, aucune commande invalide de notre part.
- Conclusion : stratégie Wood suffisante. Les leagues supérieures ajoutent
  dégâts, range et range-up ; il faudra alors simuler les explosions et fuir
  activement.
