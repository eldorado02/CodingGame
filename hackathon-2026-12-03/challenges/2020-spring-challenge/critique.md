# Critique — Spring Challenge 2020

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Oscillation entre pellets proches | Élevé | Mémorisation de la cible jusqu'à ce qu'elle soit mangée ou inaccessible. |
| Timeout (50 ms) | Élevé | Un seul BFS par tour (distances + prédécesseurs), pas de BFS par candidat. |
| Collision avec l'adversaire | Moyen | Prédiction de son meilleur pas, pénalité sur notre premier pas identique. |
| Divergence 4 langages | Élevé | Même clé de tri `(penalty, coût/point, -valeur, distance, x, y)` + fuzz. |

## Après implémentation

- `tests/run.sh` : 10 cas × 10 tours × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires de 8 tours → **0 divergence**.
- Deux bugs détectés puis corrigés pendant la validation :
  1. le calcul du premier pas cherchait un voisin à distance `d-1` depuis le
     **départ** au lieu du BFS inverse — cela figeait le pac sur place ;
  2. le port Rust n'excluait pas les pellets situés sur notre propre case.
- Validation officielle (agent 0) : **9 victoires / 9**.
  - Python : 5/5 vs BLINKY (143–87), Zoryx (103–8), EnderWiggins (103–15),
    gue (151–2), PulsarBF (146–5).
  - C : 2/2 vs BLINKY (120–41, 115–49).
  - C++ : 1/1 vs BLINKY (111–44).
  - Rust : 1/1 vs BLINKY (109–35).
- La stratégie « cible unique mémorisée + BFS + évitement de collision » collecte
  100 à 150 pellets par partie, largement de quoi battre les bots Wood 2.
- Amélioration possible : tenir compte du rayon de collecte et optimiser un
  chemin global (plusieurs pellets par tour) via un TSP approximé.
