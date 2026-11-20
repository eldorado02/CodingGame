# Critique — Spring Challenge 2021

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Nutrient qui baisse trop vite | Élevé | Compléter dès le tour 1. |
| Mauvais choix d'arbre | Moyen | Priorité à `richness` (bonus +4). |
| Divergence 4 langages | Faible | Clé de tri unique + fuzz. |

## Après implémentation

- `tests/run.sh` : 10 cas × 8 tours × 4 langages → OK.
- Fuzz différentiel : 200 cas de 6 tours → **0 divergence**.
- Validation officielle (agent 0) : **8 victoires / 8**.
  - Python : 72–70 et 85–24.
  - C : 78–74 et 78–64.
  - C++ : 78–74 et 76–76 (départage au nombre d'arbres).
  - Rust : 76–72 et 82–6.
- La victoire contre le boss est serrée (72–70) : la règle de départage au
  nombre d'arbres peut sauver une partie à score égal, ce qui s'est produit en C++.
- Amélioration possible : modéliser le nombre optimal de complétions en
  comparant le gain d'un point de nutrient vs le bonus de fin de soleil.
