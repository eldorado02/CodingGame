# mad-knights — guide de reprise

- Énoncé : `statement.md`. Ligue unique `Wood` (136 joueurs). Isolation à 3 cavaliers (r, g, b) sur 8×8 ; chaque case visitée est bloquée ; sans coup → éliminé ; dernier survivant gagne. Referee : github.com/darkhorse64/CG_MadKnights.
- Protocole : couleur ; par tour 3 lignes `joueur statut dernierCoup`, 8 lignes plateau (rangée 8 en haut, `#` bloqué), N coups légaux. Sortie `e3` ou `random`. 1000 ms / 100 ms. Test games : 2 adversaires obligatoires.
- Bot soumis : **v1** `41796137` — recherche paranoïde (nous max, les 2 adversaires min) αβ ID ; éval = Σ(10 + mobilité suivante) sur nos coups − 6 × mobilité adverse ; tri Warnsdorff.
- Bench v1 : 3/3 contre les rangs 69 `6080650` + 86 `6155530`. CG : 60k nœuds ≈ 8 ms (prof. 9) → marge ×5 disponible.
- Prochaine étape : budget 300k, bench rangs 18 `6893434` / 35 `5826123`, éval Voronoi (cases atteignables avant les autres).
- Fuzz : `bash tests/run.sh` (CG_NODES=3000, 20 s).
