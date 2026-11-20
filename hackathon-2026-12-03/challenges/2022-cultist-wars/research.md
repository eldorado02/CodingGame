# Recherche — Cultist Wars (2022-09-19)

## Sources
- Énoncé officiel : `statement.md`.
- Stub officiel : `cg.py stub cultist-wars`.
- Code source du moteur : `https://api.bitbucket.org/2.0/repositories/Nixerrr/cultist-wars/src/...`.

## Mécanique confirmée par le source
- Grille 13×7, 14 unités (2 leaders + 12 neutres), 150 tours, 1 action par tour.
- `CONVERT` sur une cible non adjacente est transformé par le moteur en
  `MOVE` vers la case de la cible ; `SHOOT` hors portée fait de même.
  Un `SHOOT` est donc aussi un déplacement utile.
- Dégâts = `7 - distance de Manhattan` ; le tir suit Bresenham tracé depuis la
  coordonnée Y la plus basse et peut toucher une unité/obstacle intermédiaire.
- Conversion : un swing de 2 au score (–1 à l'adversaire, +1 à soi), sauf sur
  un neutre (+1 seulement).
- Score final = nombre d'unités vivantes ; départage par l'ordre des joueurs en
  cas d'égalité.

## Stratégie retenue
1. Neutraliser en priorité un cultiste ennemi qui peut toucher l'une de nos
   unités (simulation de Bresenham dans les deux sens).
2. Sinon, le leader convertit le neutre le plus proche de sa moitié (x ≤ 6 pour
   le joueur 0, x ≥ 6 pour le joueur 1), avec une distance BFS ignorant les
   unités puisque le moteur pathfind tout seul.
3. Si un cultiste ennemi est à ≤ 2 cases, le leader le convertit (swing de 2).
4. Sinon, les cultistes tirent sur la cible la plus rentable (kill, dégâts,
   distance).
5. Sinon, un `SHOOT` hors portée fait avancer le cultiste vers l'ennemi.

## Risques
| Risque | Mitigation |
|---|---|
| Bresenham mal reproduit | Implémentation identique dans les 4 langages + fuzz. |
| Leader qui erre en poursuivant un neutre mobile | Cible la plus proche recalculée chaque tour, limitée à notre moitié. |
| Cultiste qui reste inactif | `SHOOT` hors portée utilisé comme déplacement. |
| Timeout 50 ms | BFS 13×7 et au plus 16 unités : constante. |
