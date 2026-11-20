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
