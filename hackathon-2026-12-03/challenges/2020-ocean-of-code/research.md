# Recherche — Ocean of Code (2020-03-20)

## Sources

- Énoncé officiel : `statement.md`.
- Referee et modèle : https://github.com/CodinGameCommunity/ocean-of-code
  - `src/main/java/com/codingame/game/{Referee,Game}.java`
  - `src/main/java/com/codingame/game/model/{PlayerModel,GridModel,BFS,Direction,PlayerAction}.java`
  - `src/main/java/com/codingame/game/model/commands/{MoveCommand,TorpedoPower,SurfaceCommand}.java`
  - **Lecture seule, aucun code copié.**

## Mécanique (Wood 3 = `league level 1`)

- 15×15, 6 PV chacun, ~300 tours par joueur (`maxTurns = 598` actions).
- Placement initial : une case water (`x` = île, `.` = eau).
- Chaque tour : une ou plusieurs actions séparées par `|`, chaque type au plus
  une fois. Action invalide/absente → `SURFACE` automatique.
- `MOVE N/E/S/W TORPEDO` : déplacement d'une case, charge le torpedo,
  interdit de revenir sur une case déjà visitée depuis le dernier `SURFACE`.
- `SURFACE` : réinitialise le chemin, coûte 1 PV, annonce le secteur (`1..9`).
- `TORPEDO x y` : 3 charges (cooldown `3 - charges`), cible eau à distance BFS
  ≤ 4 **depuis la position au début du tour** (la validation a lieu avant
  l'exécution du déplacement), dégâts 2 sur la cible, 1 sur les 8 voisins.
- `torpedoCooldown` vaut `3` (0 charge) à `0` (prêt). `MOVE ... TORPEDO`
  diminue le cooldown de 1.
- Secteur : `(x / 5) + 1 + (y / 5) * 3` (identique à `ceil((x+1)/5)`).

## Informations adversaires

- `opponentOrders` = `MOVE D`, `SURFACE s`, `TORPEDO x y`, séparés par `|`.
- Un `MOVE` révèle la direction ; un `SURFACE` révèle le secteur ; un
  `TORPEDO` révèle la cible (donc impose que l'adversaire était à ≤ 4 de là).
- Le retour de nos propres tirs est observable via la variation de `oppLife`
  au tour suivant : −2 ⇒ cible exacte, −1 ⇒ voisin 8-connexe, 0 ⇒ hors zone.

## Décisions de conception

1. Initialisons la croyance sur l'adversaire à toutes les cases eau (même la
   nôtre : l'adversaire peut choisir la même case).
2. Filtrons cette croyance à chaque tour : dégâts de notre torpedo, puis ordres
   adverses dans leur ordre (déplacement, surface, tir).
3. Si la croyance devient vide (contradiction), réinitialiser à toutes les eaux.
4. Déplacement : voisin eau non visité maximisant le nombre de voisins libres
   (Warnsdorff), départage par proximité du centroïde de la croyance, puis N/E/S/W.
5. Si aucun voisin libre : `SURFACE | MOVE ...` pour ne pas rester immobile.
6. Tir : dès que le cooldown est 0, choisir la cible eau à ≤ 4 qui maximise
   `2 * P(cible) + Σ P(voisins)` tout en évitant de nous toucher après déplacement.
7. Même ordre de calcul et mêmes départages dans les 4 langages.

## Risques

| Risque | Mitigation |
|---|---|
| Croyance erronée due au non-respect de l'historique de chemin adverse | Le filtre ignore les revisites adverses ; les indices de tir/surface compensent. |
| Se retrouver sans case libre | `SURFACE` + déplacement, coût 1 PV seulement quand nécessaire. |
| Tir invalide (hors portée / pas chargé) | Portée calculée depuis la position d'entrée, cooldown lu dans l'input. |
| Timeout 50 ms | Grille 225 cases, BFS locaux, aucun calcul exponentiel. |
