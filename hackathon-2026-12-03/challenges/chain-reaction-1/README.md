# chain-reaction-1 — guide de reprise

- Énoncé : `statement.md`. Ligue unique `Wood` (73 joueurs). 6×6, masse critique = nb de voisins, explosions en chaîne, perte si plus aucune case (après le 1er coup de chaque joueur).
- Protocole : 1re ligne couleur (`r` commence, `b`) ; par tour `lastMove` (`null`) + 6 lignes de 12 caractères (rangée 6 en haut). Sortie `c4` ou `random`. 1000 ms puis 100 ms.
- Fait clé : les orbes sont conservés → total d'orbes = nombre de coups joués ; test de fin exact « un camp sans case et total ≥ 2 ».
- Bot soumis : **v1** `41796092` — template board-2p, éval classique (orbes, cases vulnérables à une case critique adverse voisine, bonus coin/bord/critique), budget 50k nœuds (250k au tour 1).
- Bench v1 (150k nœuds, TIMEGUARD fréquent) : 1/3 contre le rang 37 `6917044`, 0/2 contre le rang 19 `6792696`. Ancien plafond (session OpenCode, code perdu) : 0/5 contre jacek `6785873`.
- Prochaine étape : vérifier le budget (TIMEGUARD), éval « chaînes » (cases critiques connexes), quiescence sur cases critiques.
- Fuzz : `bash tests/run.sh` (CG_NODES=1500, 25 s).
