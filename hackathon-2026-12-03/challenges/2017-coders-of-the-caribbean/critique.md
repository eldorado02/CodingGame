# Critique — Coders of the Caribbean

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Rhum épuisé avant le baril | Moyen | Cible la plus proche, vitesse maximale. |
| Mauvais ciblage hex | Faible | `MOVE` gère le chemin ; on ne donne que la destination. |
| Timeout 50 ms | Faible | 26 entités → trivial. |
| Divergence entre langages | Faible | Distance entière identique. |

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 grilles aléatoires (`random.seed(19)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Test game réel : partie complète jouée sans timeout ni commande invalide,
  **rang 1/2** (score 0–0, victoire au départage). Les deux navires ont
  survécu jusqu’au tour 200.
- Limite : la stratégie « baril le plus proche » est minimale en Wood 3 ;
  avec 1 navire et `MOVE` automatique, il n’y a pas grand-chose d’autre à
  optimiser. Les ligues supérieures (3 navires, canons, mines, virage
  manuel) demanderont une refonte.
