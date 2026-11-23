# nine-mens-morris — guide de reprise

- Énoncé : `statement.md` ; referee github.com/bindstone/nine-mens-morris. 24 cases, pose (9 pierres) / déplacement / vol (3 pierres) ; moulin = prise (hors moulins d'abord) ; perd sous 3 pierres ou sans coup ; 200 tours = nul. Ligues Wood 2 → Wood 1. Boss Wood 2 : `3863794`.
- Protocole : id, 24 lignes d'adjacence `A1:A4;D1` ; par tour coup adverse, plateau `A1:2;...` (2 = vide), N commandes légales (`PLACE;A1`, `PLACE&TAKE;A1;D1`, `MOVE;A1;D1`, `MOVE&TAKE;A1;D1;D2`). 50 ms. Pierres en main non fournies : 9 − nombre de PLACE vus.
- Bot soumis : **v1** `41796948` — moulins déduits des triplets alignés de l'adjacence, negamax αβ ID, captures d'abord ; éval 100/pierre + 10/moulin + mobilité ; 15k nœuds.
- Bench v1 : 3/3 (5–0) contre le boss Wood 2.
- Fuzz : `bash tests/run.sh` (flux auto-joués, CG_NODES=800).
