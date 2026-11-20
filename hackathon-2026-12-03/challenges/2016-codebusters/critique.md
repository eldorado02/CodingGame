# Critique — CodeBusters

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| `MOVE` dépasse la cible | Élevé | On vise un point à exactement 800 ; si d < 900 on recule. |
| Fantôme en fuite | Moyen | Bande 900–1760 Large ; anticipation 400 si buster à ≤ 2200. |
| Deux busters sur le même fantôme | Moyen | Réservation globale par tour. |
| Contestations adverses | Faible en Wood | Pas de counter fiable sans historique ; on privilégie la proximité. |
| Timeout 100 ms | Faible | 5 × 28 comparaisons → trivial. |
| Divergence entre langages | Moyen | Même plan, cas déterministes + fuzz différentiel. |

## Après implémentation

- `tests/run.sh` : 8 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(7)`) → **0 divergence**.
- Test game réel : le bot tourne en boucle sans timeout (score 3–5 contre
  Bosster en Wood). Les 4 fantômes manquants sont dus à une stratégie encore
  simple (poursuite du plus proche), pas à un bug de protocole.
- Améliorations possibles (Wood+) : départage par distance à notre base,
  anticipation de fuite à 400, gestion des contestations 2v1.
