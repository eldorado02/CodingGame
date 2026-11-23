# twixt-pp — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- Énoncé officiel (`cg.py show twixt-pp`).
- https://www.littlegolem.net/jsp/games/gamedetail.jsp?gtid=twixt&page=rules et https://en.wikipedia.org/wiki/TwixT — règles TwixT / PP.
- https://api.github.com/search/repositories?q=twixt+codingame → `TidyMaze/Twixt-PP`, `sergio-strazzacappa/Twixt-PP` (vide), `desecnd/twixt-pp` (visualisation).
- https://raw.githubusercontent.com/TidyMaze/Twixt-PP/HEAD/src/main.rs — bot aléatoire Rust : confirme lettre = x, chiffre = y, 1er joueur interdit en colonnes x = 0 / 11, après `SWAP` le 1er joueur garde haut/bas.
- Logs `games/twixt-pp/logs/v1-*.log` : rangs 49/65 jouent `F4 bla bla bla...` (stub) → invalides ; rangs 9/25 gagnent en 7–8 coups.
- Recherche OpenCode : non relancée (échec complet de la chaîne sur Hex le même jour).

## Faits du referee / contradictions avec l'énoncé
- Referee non public trouvé. Score : gagnant 1, perdant -1 (y compris sur défaite par connexion).

## Classe de problème (ALGO.md) et approche retenue
C (connexion), branching ~130. v1 : distance + 2 plis. Pistes : départage central, éval à chemins multiples, MCTS avec playouts de liens.
