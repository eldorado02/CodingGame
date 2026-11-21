# Recherche — Xmas Rush (2018-12-07)

## Sources

- Énoncé officiel : `statement.md` (API `/api/v1/puzzles/xmas-rush`).
- Referee officiel : https://github.com/CodinGameCommunity/XmasRush
  - `src/main/java/com/codingame/game/Referee.java`
  - `src/main/java/com/codingame/game/Model/GameBoard.java`
  - `src/main/java/com/codingame/game/Model/TileModel.java`
  - `src/main/java/com/codingame/game/Utils/Constants.java`
  - **Lecture seule, aucun code copié.**

## Mécanique (Wood 3 = league 1)

- Plateau torique ? **Non** : 7×7 sans wrap pour les tuiles ; wrap seulement
  pour la position des joueurs poussés hors ligne (`Vector2.wrap`).
- Pattern de tuile = 4 bits `URDL` (haut, droite, bas, gauche).
  Déplacement valide si `current[d]=='1'` **et** `next[opp(d)]=='1'`.
- Directions : `U=0, R=1, D=2, L=3` ; opposé `(d+2)%4`.
- Poussée :
  - `RIGHT` sur ligne y : insertion en `(0,y)`, popping en `(6,y)` ;
  - `LEFT` : insertion `(6,y)`, popping `(0,y)` ;
  - `UP` sur colonne x : insertion `(x,6)`, popping `(x,0)` ;
  - `DOWN` : insertion `(x,0)`, popping `(x,6)`.
  - La tuile poussée est celle du joueur ; la tuile éjectée devient la sienne.
  - Un joueur sur la ligne poussée est décalé d'une case avec wrap.
- Si les deux joueurs poussent la même ligne, rien ne se passe.
- Quest : 1 par joueur, toujours révélée en Wood 3 (`numVisibleCards=1`).
  Un item sur la tuile joueur a `x=y=-1`, sur celle de l'adversaire `-2`.
- Collecte : en début de tour **et** après les actions, si le joueur est sur
  la tuile portant son item de quête.
- 150 tours de jeu, 20 pas max par tour `MOVE`, 50 ms par tour.

## Décisions de conception

1. Tour `MOVE` : BFS sur les 49 tuiles vers l'item de quête, sortir le
   chemin (≤ 20 directions) ou `PASS`.
2. Tour `PUSH` : simuler les 14 poussées possibles (7 lignes × 2 sens,
   7 colonnes × 2 sens), recalculer position + item, puis BFS ; choisir la
   poussée qui minimise la distance (ou la distance de Manhattan si aucun
   chemin).
3. Si l'item est sur notre tuile, la poussée l'insère sur le plateau à
   l'emplacement d'insertion : modélisé explicitement.
4. Arithmétique entière, ordre de poussée fixe → fuzz déterministe.

## Risques

| Risque | Mitigation |
|---|---|
| Item sur tuile joueur (`-1`) | Insertion simulée lors du push. |
| Item sur tuile adverse (`-2`) | Ignoré (jamais notre quête). |
| Poussée simultanée même ligne | Non modélisée (adversaire inconnu). |
| Chemin > 20 pas | On sort les 20 premières directions ; on continue le tour suivant. |
| Timeout 50 ms | 49 cases × 14 poussées → BFS O(14·49·4). |
| Divergence 4 langages | Ordre de poussée et départage identiques. |

## 2026-11-21 — recherche approfondie Bronze

Sources complémentaires consultées :
- [Referee officiel](https://github.com/CodinGameCommunity/XmasRush/blob/master/src/main/java/com/codingame/game/Referee.java) :
  `checkForFinishedItems()` ne teste que la tuile **sous le joueur** après les actions ;
  un item sur la tuile tenue en main n'est collecté que si une poussée insère cette
  tuile sur le plateau **et** que le joueur est sur la ligne poussée (il atterrit
  alors sur la case d'insertion).
- [`GameBoard.pushLine`](https://github.com/CodinGameCommunity/XmasRush/blob/master/src/main/java/com/codingame/game/Model/GameBoard.java) :
  la tuile éjectée devient la tuile du joueur ; un item qui y est présent change
  donc de camp.
- [bot `toomyem/xmas-rush`](https://github.com/toomyem/xmas-rush/blob/master/game.py) :
  restreint les poussées aux 3 lignes voisines du joueur, évalue chaque poussée
  par la distance de Manhattan minimale atteignable en ≤20 pas, et détecte les
  poussées répétées (stale) pour éviter les blocages.
- [bot `vkandoba/xmasrush`](https://github.com/vkandoba/xmasrush/blob/master/main.py) :
  composante connexe de la quête et recherche de poussée reliant cette composante
  à la position du joueur.

Constats sur la version validée Wood 2 :
- Elle suppose 1 quête révélée et ignore le cas `itemX=-1` (quête sur notre tuile),
  noté `dist=1000` → boucle `PUSH`/`PASS` de ~20 tours observée en Bronze.
- En Bronze (12 quêtes, ≤3 révélées), le score plafonne à 8–11 contre 12 pour le boss.

Pistes testées :
- `xmas_v3` : BFS multi-quêtes en 20 pas + rapprochement si aucun chemin → 0/5 victoires (8–10 contre 12).
- `xmas_v4` : gestion correcte des quêtes en main, évaluation des poussées par
  BFS + connectivité → 2/5 victoires, deux défaites 11–12.
- `xmas_v4b` : priorité anti-éjection + rapprochement généralisé → 1/5 victoire, quatre 10–12.
- `xmas_v5` : poussées locales, distance de Manhattan atteignable, anti-stale → en cours de validation.
