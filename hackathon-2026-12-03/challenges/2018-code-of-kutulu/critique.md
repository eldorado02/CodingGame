# Critique — Code of Kutulu

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Pas de réassurance | Élevé | Poids 100000 dans le score. |
| Wanderer sur notre case | Élevé | Distance 0 fortement pénalisée. |
| Fuite qui casse la réassurance | Moyen | Réassurance prioritaire sur la fuite. |
| Minion en cours de spawn | Faible | `param1 == 1` seulement. |
| Mur / hors carte | Faible | Voisins filtrés. |
| Timeout 50 ms | Faible | 5 coups × distances Manhattan. |
| Divergence entre langages | Moyen | Score entier, fuzz. |

## Après implémentation

- `tests/run.sh` : 6 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 labyrinthes aléatoires (`random.seed(41)`) →
  **0 divergence** entre C, C++, Rust et Python.
- Choix de conception validé : le poids 100000 de la réassurance prime sur
  la fuite ; `- de` pousse à rester collé au groupe (un explorateur peut
  partager la case d'un allié, aucune règle ne l'interdit).
- Deux bugs Rust corrigés en chemin : lecture des dimensions sur une seule
  ligne (au lieu de deux) et inférence du closure de lecture.
- Test game réel (Wood 3, 3 adversaires obligatoires) : opposants
  **Davy Jones (boss), just_code2 (rang 1), KPricorn (rang 2)** →
  **victoire**, scores `0=74 1=71 2=55 3=50`, rang `0 1 2 3`.
  Aucune commande invalide de notre agent ; l'adversaire $1 a émis une
  commande invalide au frame 249.
- Conclusion : la stratégie « réassurance + fuite des wanderers » suffit en
  Wood 3. Les ligues supérieures ajoutent les Sanity Loss, recherches et
  objets : il faudra un vrai arbre de recherche (paranoid) et une gestion
  des items."})
