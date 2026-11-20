# Critique — Wondev Woman

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Choisir une action illégale | Élevé | On ne parcourt que `legalActions`. |
| Ne pas voir la victoire imminente | Élevé | Priorité absolue à `h_dest == 3`. |
| L'adversaire gagne en premier | Moyen | Déni : élever sa case 3 à 4. |
| Hauteur mal lue (trou, hors grille) | Moyen | `-1` pour `.`/hors limites. |
| Timeout 50 ms | Faible | ≤ 64 actions. |
| Divergence entre langages | Moyen | Score entier identique, fuzz. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK (victoire, montée,
  déni, construction, adversaire invisible, trous).
- Fuzz différentiel : 200 grilles/actions aléatoires (`random.seed(23)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Test game réel : **victoire 1–0** ; niveau 3 atteint au tour 11 sans
  commande invalide ni timeout. La priorité « victoire immédiate » a joué :
  dès qu'une montée sur une case 3 est dans `legalActions`, elle est sortie.
- Conclusion : l'évaluation des actions fournies suffit en Wood 3. Les ligues
  supérieures (2 unités, PUSH, visibilité partielle) demanderont une vraie
  recherche de coups et la gestion de l'adversaire.
