# Recherche — Code Royale (2018-04-20)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/code-royale`).
- Refereur officiel : https://github.com/csj/team-2/blob/master/src/main/kotlin/com/codingame/game/Referee.kt
- Constantes : https://github.com/csj/team-2/blob/master/src/main/kotlin/com/codingame/game/Constants.kt
- Socle DSA : `../../dsa/README.md` (géométrie).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Carte 1920 × 1000, 2 reines (100 PV, rayon 30, vitesse 60), sites de
construction circulaires. Reine commence à (200,200) ou (1720,800).

| Unité | Coût | Nb | Build | Vitesse | PV | Portée | Dégâts |
|---|---|---|---|---|---|---|---|
| KNIGHT | 80 | 4 | 5 tours | 100 | 30 | 0 (mêlée) | 1 / tour à la reine |
| ARCHER | 100 | 2 | 8 tours | 75 | 45 | 200 | 2 / tour aux creeps uniquement |

Or : 100 au départ, +10/tour (Wood 3). Les creeps perdent 1 PV/tour
(vieillissement). `TRAIN` est une commande obligatoire sur la 2ᵉ ligne.

**Les ARCHER n'attaquent PAS la reine ennemie** → inutiles pour gagner en
Wood 3. Stratégie : **BARRACKS-KNIGHT + TRAIN** en boucle.

## Détails referee importants

- `BUILD <site> BARRACKS-KNIGHT` : si la reine touche le site, construit ;
  sinon **la reine se déplace vers le site**. Une seule commande suffit.
- Une reine détruit les barracks ennemis en les touchant (distance <
  `radius + 30 + 5`).
- `param1` d'un barracks = tours restants avant entraînement (0 = prêt).
- `maxTurns = 200` dans le referee (l'énoncé dit 250).
- Toutes les unités sont visibles en Wood 3 (pas de brouillard).

## Décisions de conception

1. Choisir comme cible le site **le plus proche de la reine ennemie** mais à
   distance > 150 (sécurité), avec un léger bonus pour la proximité de notre
   reine (setup rapide).
2. `BUILD <cible> BARRACKS-KNIGHT` tant qu'on n'a pas assez de barracks ;
   si un barracks ennemi est proche, `MOVE` dessus pour le détruire.
3. Chaque tour : `TRAIN` sur tous nos barracks KNIGHT prêts (`param1 == 0`)
   et dont on peut payer le coût (80 chacun).

## Risques

| Risque | Mitigation |
|---|---|
| Reine ennemie détruit nos barracks | Multiplier les barracks, cible à > 150. |
| Pas assez d'or | Un seul TRAIN par barracks prêt, tri par coût. |
| Cible invalide | Ne cibler que les sites par `siteId` connu. |
| Timeout 50 ms | ≤ 12 sites + ≤ 40 unités, parcours linéaire. |
| Divergence entre langages | Même score de cible, fuzz. |
