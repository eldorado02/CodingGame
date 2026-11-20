# Recherche — Mean Max (2017-11-17)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/mean-max`).
- Refereur officiel : `https://github.com/CodinGame/MeanMax/blob/master/Referee.java`
  (consulté pour la physique exacte, aucun code copié).
- Socle DSA : `../../dsa/README.md` (géométrie vectorielle).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Carte circulaire de rayon 6000, 3 joueurs, un Faucheur (`type 0`) chacun.
Le Faucheur récolte **1 eau par tour et par épave** (`type 4`) dont le centre
est à l'intérieur du rayon de l'épave (`distance <= wreck.radius`). Victoire
à 50 eau ou après 200 tours (meilleur score).

### Physique (validée dans le referee)

- `X Y THROTTLE` : point cible absolu `(X,Y)`, accélération de grandeur
  `THROTTLE / masse` vers ce point. Faucheur : `masse = 0.5` → accél = `2*P`.
- `0 ≤ THROTTLE ≤ 300` → accél max 600.
- Ordre : `v += accél` ; `pos += v` ; `pos` arrondie ; récolte ;
  `v = round(v * (1 - friction))` avec `friction = 0.2`.
- Arrondi du referee = **demi loin de zéro** (passage par `Math.round(|x|)`).

## Décisions de conception

- **Simulation 1 pas** des actions candidates, puis choix du meilleur score :
  - candidats : chaque épave × puissances `{0, 100, 200, 300}`, plus un
    freinage (cible `pos - v`) à 150/300, plus `WAIT` ;
  - score = `1e6 * nb_épaves_habitées + 10 * eau cumulée - distance_min - vitesse`.
- On privilégie donc rester dans une épave, y cumuler plusieurs épaves, et
  se stabiliser (faible vitesse) pour continuer à récolter.
- Sortie : 3 lignes (action du Faucheur + 2 `WAIT`).

## Risques

| Risque | Mitigation |
|---|---|
| Dépasser l'épave | Candidats de freinage + pénalité de vitesse. |
| Arrondi divergent entre langages | `round_half_away` explicite, sqrt IEEE. |
| Épave épuisée | On pondère par l'eau restante. |
| Collision avec un adversaire | Ignorée en Wood 3 (les bots adverses sont simples). |
| Timeout 50 ms | ~30 candidats × ~10 épaves → trivial. |
| Divergence entre langages | Simulation entière identique, fuzz différentiel. |
