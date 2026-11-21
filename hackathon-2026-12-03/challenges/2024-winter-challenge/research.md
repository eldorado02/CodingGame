# Recherche — Cellularena / Winter Challenge 2024 (2024-12-19)

## Sources
- Énoncé officiel : `statement.md` (récupéré via `GET /api/v1/puzzles/winter-challenge-2024`).
- Stub officiel : `python3 bot-programming/tools/cg.py stub winter-challenge-2024`.
- Moteur officiel : `https://github.com/CodinGame/WinterChallenge2024-Cellularena`
  (`src/main/java/com/codingame/game/` : `Referee.java`, `Game.java`, `GridMaker.java`,
  `Serializer.java`, `CommandManager.java`, `Player.java`, `OrganType.java`,
  `grid/Tile.java`, `grid/Protein.java`).

## Mécanique confirmée par le source

### Ligue Wood (`leagueLevel <= 4`)
- `Game.initTutorial()` : grille fixe 18×9, bords `WALL`, mur horizontal en `y = 4`
  avec une seule brèche en `x = 16`, sources de protéines `A` réparties des deux
  côtés du mur. Racines en `(1, 2)` (joueur 0) et `(1, 6)` (joueur 1).
- Chaque joueur démarre avec `10` protéines `A` (`MIN_STARTING_PROTEINS = 3`,
  `MAX_STARTING_PROTEINS = 10` en ligue supérieure ; la ligue Wood fixe 10).
- Seules les actions `GROW … BASIC` existent : coût `1 A`, aucune autre famille
  d'organes (pas de `HARVESTER`, `TENTACLE`, `SPORER`).
- Marcher sur une case `A` vide la case et rapporte `PROTEIN_PER_ABSORB = 3`
  protéines `A` (`Game.placeOrgan()` → `Tile.placeOrgan()` → `player.absorb()`).
  Bilan : +3 − 1 = **+2 A nets** par source capturée.

### Protocole
- `Referee.gameTurn()` : `setMaxTurns(101)`, `setTurnMaxTime(50)` ms,
  `setFirstTurnMaxTime(1000)` ms. Le processus reste vivant d'un tour à l'autre :
  il faut lire exactement `entityCount` lignes puis les 3 lignes de stocks, boucler,
  et ne jamais faire de `read()` global.
- `Serializer.serializeFrameInfoFor()` parcourt **toute** la grille chaque tour :
  `WALL` pour un obstacle, lettre pour une source, type d'organe sinon.
  `owner` vaut `1` pour ses propres organes, `0` pour ceux de l'adversaire, `-1`
  pour les cases neutres. `organDir` vaut `X` hors organe.
- `entityCount` **augmente** au fil des tours (85 → 97 observé) : les organes
  nouvellement créés apparaissent bien dans l'entrée du tour suivant.

### Croissance et fin de partie
- `CommandManager` accepte `GROW id x y type`. Si la cible n'est pas adjacente à
  `id`, `Game.checkCanGrowWithPathfinding()` cherche le dernier organe de la même
  racine sur un plus court chemin et y substitute le parent, l'organe étant créé
  sur la case libre suivante. Envoyer une cible **adjacente** évite toute dépendance
  au pathfinding (et ses messages d'erreur).
- Deux growths visant la même case dans le même tour créent un `WALL` (collision).
- `isGameOver()` : plus de protéines pour progresser, grille pleine, joueur mort,
  ou 100 tours. Score = nombre d'organes produits (`growthTypes_basic_*`).
- Le test game exige **exactement un adversaire** (`INVALID_PLAYER_COUNT` sinon)
  et se joue en meilleur de cinq pour passer de ligue.

## Pièges identifiés
| Piège | Preuve / mitigation |
|---|---|
| `BASIC` commence par `B`, comme la protéine `B` | Comparer le **type entier** (`kind == "A"` / `kind[1] == '\0'`), pas le premier caractère. |
| `owner = 1` signifie « à moi » | Confirmé par `Serializer` ; le générateur de tests initial l'avait inversé. |
| Croire que la case source reste visible après absorption | `Tile.placeOrgan()` met `protein = null`. |
| Lire les organes une seule fois | Relire toute la grille à chaque tour ; `entityCount` croît. |
| Confondre victoire et score de protéines | Le score affiché (15–14) est le nombre d'organes. |

## Adversaires Wood disponibles via l'API
`5736694` (boss principal, parties serrées 15–14), `5738801`, `6501284`,
`5737299`, `5736741` (boss très faibles, ~1–53). Validation principale faite
contre `5736694`.

## Stratégie retenue
BFS multi-source depuis **tous** nos organes vers toutes les cases libres, relancé
chaque tour ; on retient la source `A` atteignable la plus proche (départage
`distance, y, x`), sinon la case libre la plus proche. Un seul `GROW` adjacent
est émis, complété par des `WAIT` jusqu'à `requiredActionsCount`.

## Recherche externe Wood 3 HARVESTER (21/11/2026)
Dépôt public consulté : `aautin/cellularena` (C++).
Points clés extraits de `src/growth.cpp`, `src/path.cpp` et `src/Map.cpp` :
- un `HARVESTER` se place sur une case **adjacente** à une protéine, orienté vers
  elle ; la direction est déterminée par le décalage entre la case de croissance
  et la source ;
- une source déjà desservie est mémorisée (`add_generator`) et n'est plus
  visée : deux harvesters face à la même source ne rapportent qu'1 protéine/
  tour/ joueur, donc le second gaspille `1 C + 1 D` ;
- le chemin BFS part de **tous** nos organes (layer 0) et vise la protéine
  atteignable dans la limite `stock[A] + (peut poser un harvester)` ;
- `GROW` sur une case protéine rapporte 3 protéines du type correspondant ;
- priorité : HARVESTER si `C && D`, puis BASIC vers une source A, puis BASIC
  sur n'importe quelle case libre adjacente.

## Résultat du blocage des sources harvesées (21/11/2026)
| Variante | Résultat vs `5736695` |
|---|---|
| BFS greedy initial (14–18 organes) | ~0/5 |
| HARVESTER sans suivi des sources | 0/5 (16–17, absorption de sa propre source) |
| HARVESTER + sources exclues des objectifs | 3/5 |
| **HARVESTER + sources bloquées dans le BFS** | **9/9** |

Le suivi des sources doit être **structurel** (case infranchissable), pas
seulement **déclaratif** (case non ciblée) : le GROW crée l'organe sur le
premier pas du chemin, donc une source traversée est absorbée. C'est l'erreur
que les dépôts publics évitent en retirant la source du graphe dès qu'un
`add_generator` est posé.
