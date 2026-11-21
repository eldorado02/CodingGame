# Recherche — Troll Farm / Spring Challenge 2026 (2026-05-12)

## Sources
- Énoncé officiel : `statement.md` (`GET /api/v1/puzzles/spring-challenge-2026-troll-farm`).
- Stub officiel : `python3 bot-programming/tools/cg.py stub spring-challenge-2026-troll-farm`.
- Moteur officiel : `https://github.com/eulerscheZahl/Troll-Farm`
  (`src/main/java/engine/` : `Board.java`, `Cell.java`, `Plant.java`, `Unit.java`,
  `Constants.java`, `task/MoveTask.java`, `task/HarvestTask.java`, `task/DropTask.java`,
  `task/TaskManager.java`, `com/codingame/game/Referee.java`).

## Mécanique confirmée par le source

### Ligue Wood (1/2)
- Grille 16×8, entièrement `GRASS` sauf les deux cabanes en symétrie centrale
  (`Board.createMap` : `height = 8`, `width = 2 × height`, pas de rivière/fer/rocher).
- Arbres générés par paires miroirs, 1 à 3 par type de fruit. Chaque arbre démarre
  à une taille et un cooldown aléatoires (`Plant.tick` appliqué plusieurs fois).
- Un seul troll par joueur, attributs `movementSpeed = carryCapacity =
  harvestPower = 1`, `chopPower = 0` (`Player.init`).
- 100 tours (`GAME_TURNS_LOW_LEAGUE` ; 300 en ligue supérieure), 50 ms par tour.
- Score = nombre de fruits dans la cabane (`Player.recomputeScore`).

### Déplacements
- `Cell.isWalkable()` = **uniquement** `GRASS` : la cabane est un point de spawn,
  mais on ne peut pas marcher dessus après l'avoir quittée. C'est la cause d'un
  bug de notre première version, qui routait un chemin *à travers* la cabane.
- `Board.getNextCell()` calcule un BFS depuis la cible puis depuis le troll, et
  avance d'au plus `movementSpeed` cases vers la cible ; en cas d'égalité le
  choix est aléatoire. Avec une vitesse de 1, un `MOVE` = un pas de plus court
  chemin, à condition de viser une case `GRASS`.
- `getDistances` n'explore que des cases `GRASS` : viser la case cabane fait
  retomber sur le départ (`sourceDist == -1` puis tirage aléatoire).

### Récolte et dépôt
- `HarvestTask.apply` boucle `i = 1..3` et appelle `unit.harvest(i)` ; `Unit.harvest`
  refuse si `i > harvestPower` ou si le sac est plein. Avec `harvestPower = 1` et
  `carryCapacity = 1`, un `HARVEST` rapporte exactement **1** fruit.
- `DropTask` exige `isNearShack()` (cabane ou case adjacente) et transfère tout
  le contenu du troll dans l'inventaire du joueur.
- Priorités des tâches : `MOVE` 1, `HARVEST` 2, `DROP` 7. Une seule commande par
  troll par tour, plusieurs commandes séparées par `;`.
- Les arbres à `cooldown = 0` grandissent (`size++`) jusqu'à 4 puis produisent un
  fruit (max 3) ; le cooldown est alors 6..9 selon l'espèce (réduit près de l'eau).

## Protocole
- Entrée initiale : `width height`, puis `height` lignes de caractères
  (`.` = herbe, `0` = notre cabane, `1` = cabane adverse ; `#`, `+`, `~` en ligue
  supérieure).
- Chaque tour : 2 lignes d'inventaire (6 entiers), `treeCount` puis `treeCount`
  lignes `type x y size health fruits cooldown`, `trollsCount` puis
  `trollsCount` lignes de 14 entiers (`owner = 0` = notre troll).
- Le boss Wood joue `MSG Eat your vegetables!;MOVE <id> <arbre fruitier>` et
  dépose dès qu'il est adjacent : c'est exactement la stratégie greedy ci-dessous.

## Stratégie retenue
BFS depuis le troll (cases `GRASS` seulement) :
1. si le troll porte un fruit : `DROP` si la distance de Manhattan à la cabane
   vaut 1, sinon `MOVE` d'un pas vers la case herbeuse adjacente à la cabane la
   plus proche ;
2. sinon, si un arbre avec des fruits est sous le troll : `HARVEST` ;
3. sinon `MOVE` d'un pas vers l'arbre fruitier atteignable le plus proche
   (départage `distance, -fruits, y, x`) ;
4. `WAIT` si aucun arbre ne porte de fruit.

## Risques
| Risque | Mitigation |
|---|---|
| Marcher sur la cabane | BFS limité aux cases `.`. |
| Cibler une case non herbeuse | Cibles = arbres (sur herbe) ou voisins herbeux de la cabane. |
| Inviter un tirage aléatoire du moteur | Recalcul du BFS à chaque tour depuis la position réelle. |
| Fruit insoluble (`carryCapacity = 1`) | Dépôt dès que la distance à la cabane vaut 1. |
| Dépassement de 50 ms | Grille 128 cases, BFS en `O(w × h)`. |
