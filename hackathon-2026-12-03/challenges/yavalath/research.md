# yavalath — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- http://cambolbro.com/games/yavalath/ — règles originales (Cameron Browne). Retenu : 4 gagne, 3 perd, swap.
- https://github.com/MultiStruct/Yavalath — referee Java officiel (lu en entier : `HexGrid.java`, `Referee.java`, `Action.java`, `Player.java`, `config/Boss.java` = bot aléatoire sur coups valides).
- https://raw.githubusercontent.com/MultiStruct/Yavalath/master/src/main/java/com/codingame/game/HexGrid.java — calcul des lignes (3 axes, ≤ 3 cases de chaque côté).
- Test game sonde `games/yavalath/logs/probe-*.log`.
- Recherche OpenCode : non lancée pour ce jeu (la chaîne a échoué sur Hex juste avant, tous modèles en échec) ; referee lu directement.

## Faits du referee / contradictions avec l'énoncé
- `gameTurn(turn)` commence à 0 : joueur 0 joue aux tours pairs ; `turn == 1` autorise le vol (case du coup précédent ajoutée aux coups valides).
- `myId` envoyé si `turn <= 1` (1er tour de chaque joueur).
- Plateau plein sans résultat → nul (scores 0/0). Victoire → score 1, coup invalide/timeout → -1.
- Le `HexGrid` insère des listes en trop au-delà de l'index 8 (bug inoffensif : seules les 9 premières sont lues).

## Classe de problème (ALGO.md) et approche retenue
B (plateau abstrait, branching ≤ 61). αβ + ID + extension des coups forcés.
