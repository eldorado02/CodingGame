# Recherche — Back Track King / Summer Challenge 2026 (2026-09-07)

## Sources
- Énoncé officiel : `statement.md` (`GET /api/v1/puzzles/summer-challenge-2026-back-track-king`).
- Stub officiel : `python3 bot-programming/tools/cg.py stub summer-challenge-2026-back-track-king`.
- Adversaires : `python3 bot-programming/tools/cg.py opponents summer-challenge-2026-back-track-king`
  (boss Wood 2 `6767604`, 126 opposants Wood 2).
- AI starter Python officiel : `https://gist.github.com/CGjupoulton/5a73bbd1142af98c6ca6887648b07cc2`.
- Replays de validation : `bot-programming/games/summer-challenge-2026-back-track-king/logs/`.

## Mécanique Wood confirmée

### Objectif
- La ligue 1 est un objectif spécial : former **une** connexion active entre
  deux villes, l’une ayant l’autre dans ses `desiredConnections`.
- Le boss Wood passe son tour (`WAIT`) ; il n’y a donc aucun conflit de paint.
- Limite : 100 tours. Victoire immédiate dès que l’objectif est atteint ;
  pour progresser il faut gagner au moins 3 parties sur 5.

### Carte et peinture
- Grille `21..30 × 14..20`, chaque case portant un `regionId` et un type
  `0` plaine, `1` rivière, `2` montagne.
- 4 à 12 villes, une par région, toujours sur une plaine, jamais dans deux
  régions voisines.
- Chaque tour donne 3 points de paint, non reportables.
- Coût d’un track : 1 (plaine), 2 (rivière), 3 (montagne).
- Un track ne peut être posé ni sur une ville, ni sur un track existant.
- Une connexion est un chemin orthogonal ininterrompu de tracks et de villes.
  Le plus court chemin devient actif ; en cas d’égalité la direction est
  priorisée `NORD > EST > SUD > OUEST` depuis la ville demanderesse.

### `AUTOPLACE`
- `AUTOPLACE fromX fromY toX toY` génère automatiquement la suite d’actions
  formant le chemin le **moins cher** en points de paint ; la commande est
  remplacée par ces actions.
- Si le budget du tour est insuffisant, le moteur interrompt la construction
  (« Autobuild interrupted: not enough track points to build a track at (x, y) »)
  et un nouvel `AUTOPLACE` avec les mêmes bornes reprend là où il s’est arrêté.
- Observé en parties réelles : un chemin de 9 à 10 tracks plains se construit
  en 3 tours (`3 + 3 + 3` points), puis la partie s’arrête sur la victoire.

## Protocole
- Initialisation : `myId`, puis `width`, puis `height`, puis `width × height`
  lignes `regionId type`, puis `townCount`, puis `townCount` lignes
  `townId townX townY desiredConnections` (`x` ou `id,id,...`).
- Chaque tour : `myScore`, `foeScore`, puis `width × height` lignes
  `trackOwner instability inked partOfActiveConnections`.
- Sortie : une ligne contenant au moins une action, séparée par `;` ;
  `AUTOPLACE`, `PLACE_TRACKS`, `DISRUPT` (ligues supérieures), `MESSAGE`, `WAIT`.
- Le stub lit des **jetons** : le découpage en lignes peut varier. Les bots
  Python et Rust utilisent donc un lecteur de jetons avec tampon, tandis que
  C et C++ utilisent naturellement `scanf` / `operator>>`.

## Stratégie retenue
1. Parser les villes et leurs `desiredConnections`.
2. Choisir la paire `(u, v)` réalisable la plus proche selon la clé
   `(distance de Manhattan, id demandeur, id cible)`.
3. Émettre chaque tour `AUTOPLACE xu yu xv yv` jusqu’à la fin de la partie.

Justification : le plateau est sans obstacle infranchissable ; même le chemin
le plus coûteux reste très sous les 300 points disponibles sur 100 tours, donc
la distance de Manhattan suffit à garantir l’objectif. Le départage par identifiant
rend la sortie identique dans les quatre langages.

## Risques
| Risque | Mitigation |
|---|---|
| Mauvais découpage des lignes d’entrée | Lecture par jetons dans les 4 langages. |
| Paire cible absente du plateau | Les identifiants sont indexés depuis l’entrée ; départage total. |
| `AUTOPLACE` interrompu | Réémission identique chaque tour ; le moteur reprend automatiquement. |
| Budget paint insuffisant | Bornes du plateau : moins de 100 tours même au coût maximal. |
| Connexion non désirée | On ne choisit que des paires présentes dans `desiredConnections`. |
| Adversaire en ligue supérieure | Hors périmètre Wood 1 : le boss passe son tour. |
