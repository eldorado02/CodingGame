# Critique — Mean Max

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Physique mal reproduite | Élevé | Refereur officiel consulté, simulation 1 pas. |
| Arrondi divergent | Élevé | `round_half_away` explicite dans les 4 langages. |
| Dépasser l'épave | Moyen | Candidats de freinage + pénalité de vitesse. |
| Mauvais ciblage | Moyen | Scoresimulation sur tous les candidats. |
| Timeout 50 ms | Faible | ~30 candidats × ~10 épaves. |
| Divergence entre langages | Moyen | Mêmes calculs flottants IEEE, fuzz. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 configurations aléatoires (`random.seed(29)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bugs trouvés : `inf` vs `1e18` pour « aucune épave », et `best` initialisé
    à `-inf` vs `-1e18`. Harmonisé sur `1e18` / `-1e18` dans les 4 langages.
  - Le générateur de cas écrivait aussi 3 entiers par ligne de score au lieu
    d'un seul → corrigé.
- Physique validée contre le referee officiel (accél = `2*P` vers le point
  cible, arrondi demi-loin-de-zéro, friction 0.2).
- Test game réel (2 adversaires Wood 3 : MiniMax + snarmph) : **victoire
  50–20–21**, seuil de 50 atteint sans commande invalide ni timeout.
- Conclusion : la simulation 1 pas suffit en Wood 3. Les ligues supérieures
  (Looters multiples, Tankers, sorts) demanderont une gestion des collisions
  et des rôles.
