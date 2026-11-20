# Critique — Fantastic Bits

## Avant implémentation

| Risque | Évaluation | Mitigation |
|---|---|---|
| Inertie, dépassement du Snaffle | Élevé | Visée anticipée `pos+3v`, thrust réduit à proximité. |
| Lancer dans le mauvais but | Élevé | But adverse = miroir de `myTeamId`. |
| Oubli du THROW après grab | Élevé | Priorité absolue dès `state==1`. |
| Deux sorciers sur le même Snaffle | Moyen | Exclusion de la cible déjà choisie. |
| Divergence entre langages | Moyen | Même logique entière, fuzz différentiel. |

## Après implémentation

- `tests/run.sh` : 7 cas déterministes × 4 langages → OK.
- Fuzz différentiel : 200 parties aléatoires (`random.seed(13)`) →
  **0 divergence** entre C, C++, Rust et Python.
  - Bug trouvé : Rust lisait les deux lignes de score comme une seule,
    `entities` tombait sur la 2e ligne de score → index hors bornes.
    Corrigé (2 lignes de score + 1 ligne `entities`, comme l’énoncé).
- Test game réel : **victoire 4–0** contre l’adversaire Wood, sans timeout
  ni commande invalide. Le THROW au but adverse et le rôle défenseur
  fonctionnent en conditions réelles.
- Conclusion : stratégie Wood 2 validée. Les leagues supérieures ajoutent
  Bludgers et sorts ; il faudra gérer les extrusions de Snaffle et les
  collisions de sorciers.
