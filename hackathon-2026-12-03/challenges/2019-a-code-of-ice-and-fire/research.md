# Recherche — A Code of Ice and Fire (2019-05-17)

## Sources

- Énoncé officiel : `statement.md`.
- Referee et modèle : https://github.com/Azkellas/a-code-of-ice-and-fire
  - `src/main/java/com/codingame/game/Referee.java`
  - `src/main/java/com/codingame/antiyoy/{Constants,GameState,Cell,Unit,Pathfinding}.java`
  - `src/test/starterkit/starter.py`
  - **Lecture seule, aucun code copié.**

## Mécanique (Wood 3 = `league level 1`)

- 2 joueurs, carte 12×12 symétrique, QG en `(0,0)` et `(11,11)`.
- `#` = void (infranchissable), `.` = neutre, `O`/`o` = nous actif/inactif,
  `X`/`x` = adversaire actif/inactif.
- 100 tours (`MAX_TURNS = 100`), une ligne de commandes par tour,
  actions séparées par `;` et traitées séquentiellement.
- Wood 3 : **uniquement des unités niveau 1**, coût `10`, upkeep `1`,
  pas de bâtiment constructible. Une unité nivel 1 tue une unité nivel 1 et
  peut capturer le QG adverse.
- Revenu = nombre de cellules actives possédées − nombre d'unités.
  Or négatif → toutes les unités meurent et l'or est remis à 0.
- `TRAIN 1 x y` : cible `isPlayable` = possédée active ou adjacente à une
  cellule possédée active. Coût 10. L'unité ne peut pas bouger le tour même.
- `MOVE id x y` : même si la cible est loin, le referee choisit le voisin
  orthogonal qui réduit la distance précalculée (hors void) vers la cible et
  qui est capturable. Une unité ne bouge donc que d'une case par tour.
- Capturer le QG adverse (cellule changée de propriétaire) gagne immédiatement.

## Décisions de conception

1. BFS depuis le QG adverse sur les cases non-`#` pour obtenir `dist[x][y]`.
2. Chaque tour : `MOVE` de toutes nos unités vers le QG adverse (le referee
   fait le choix du voisin), puis `TRAIN` sur les cases jouables libres les
   plus proches du QG adverse, autant que l'or le permet (`gold // 10`).
3. Les `MOVE` sont émis avant les `TRAIN` : une case libérée par un déplacement
   peut être réutilisée pour entraîner une nouvelle unité.
4. La cellule du QG adverse est elle-même une cible de `TRAIN` si elle est
   adjacente à notre territoire : cela gagne la partie directement.
5. Même ordre de tri `(distance, x, y)` dans les 4 langages.

## Risques

| Risque | Mitigation |
|---|---|
| Unités bloquées les unes par les autres | Entraînement sur des cases distinctes, triées par distance. |
| Or négatif | On n'entraîne que si `gold >= 10` ; capturer des cases compense l'upkeep. |
| Cellule inactive | Le referee recalcule l'activité avant chaque tour ; les unités sur cellule inactive meurent. |
| Timeout 50 ms | BFS 12×12 + tri de ~144 cases. |
