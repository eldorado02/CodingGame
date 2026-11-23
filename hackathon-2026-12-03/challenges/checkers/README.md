# checkers — guide de reprise

- Énoncé : `statement.md`. Ligue unique `Wood` (352 joueurs). Dames anglaises 8×8, prise obligatoire, rafles complètes, dames (rois) d'un pas.
- Protocole (vérifié en test game) : 1re ligne couleur (`b`/`r`) ; par tour 8 lignes plateau (1re = rangée 8), N, N coups (`A3B4`, rafle `A3C5A7`). Noir en bas, monte, joue en premier (nous = agent 0 = noir en test). 100 ms/tour. Partie nulle vers ~300 frames.
- Bot soumis : **v1** `41796035` — template board-2p, racine = liste du referee, générateur maison en profondeur, budget 25k nœuds (prof. 7–8, ~20 ms CG).
- Bench v1 : 3/3 contre le rang 177 `5820835`, 3/3 contre le rang 89 `4735875`.
- Prochaine étape : bench rang 45 `4016191` / rang 1 `6811606`, TT + éval (mobilité, dames, structure).
- Fuzz : `bash tests/run.sh` (CG_NODES=3000, ~1 min).
