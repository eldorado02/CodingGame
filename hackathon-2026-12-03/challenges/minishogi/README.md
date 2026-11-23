# minishogi — guide de reprise

- Énoncé : `statement.md` ; referee github.com/jdermont/MiniShogi. Ligue unique `Wood` (92 joueurs). 5×5, parachutages, promotion, répétition ×3 perdue par le joueur 0.
- Protocole (sonde) : id, taille 5 ; par tour 5 lignes plateau (jetons `+X` promus), main J0, main J1, dernière action, N actions légales (`4534`, `1425+`, `G*33`). Case (col, rang) = `board[rang-1][5-col]` (colonne 1 = file de droite, rang 1 = ligne du haut).
- Bot soumis : **v1** `41796267` — glouton sur la liste légale : 10 × valeur capturée + 4 promotion − 10 × valeur de la pièce si la case d'arrivée est attaquée (carte d'attaque adverse), −1 pour un coup de roi.
- Bench v1 : 3/3 contre le rang 61 `5820877`.
- Prochaine étape : moteur complet (coups, parachutages, échec) + αβ ; bench rangs 31/16. Code partagé avec `dice-shogi` (macro/constante DICE).
- Fuzz : `bash tests/run.sh`.
