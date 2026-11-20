# Critique — Fall Challenge 2020

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Mauvais index du prix dans la ligne d'ordre | Élevé | Vérification contre le stub officiel. |
| Deuxième brew impossible | Moyen | Départage par consommation minimale d'ingrédients. |
| Attendre trop longtemps | Élevé | Brassage immédiat dès qu'une recette est réalisable. |

## Après implémentation

- `tests/run.sh` : 10 cas × 6 tours × 4 langages → OK.
- Fuzz différentiel : 200 cas de 6 tours → **0 divergence**.
- Un bug d'indexage du prix (index 5 au lieu de 6) a été détecté par la première
  test game (défaite 17–25 contre Bossrova) puis corrigé.
- Validation officielle (agent 0) : **8 victoires / 8**.
  - Python : 35–27 et 28–0.
  - C : 35–25 et 39–0.
  - C++ : 37–24 et 35–0.
  - Rust : 29–22 et 32–0.
- La stratégie est volontairement minimale : aucun calcul de planification sur
  plusieurs tours n'était nécessaire en Wood 4.
