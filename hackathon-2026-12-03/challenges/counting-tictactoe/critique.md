# counting-tictactoe — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Counting TicTacToe — v1→v2 αβ sélectif soumise
- Constat : v1 (éval complète des 288 fenêtres, prof. 2) : 0/3 contre le boss Wood 3 (≈200–230)
- Hypothèse : éval incrémentale + tri par gain immédiat + faisceau de 10 coups aux nœuds internes donne prof. 4
- Sources : referee RezaSi/counting-tic-tac-toe (règle de comptage), aucun bot public trouvé (gh_search.sh)
- Action : éval incrémentale (×10), tri stable + faisceau 10, budget 60k (TIMEGUARD) puis 2500 nœuds ; 4 langages, fuzz 200/200
- Résultat : 2/3 puis 3/4 contre le boss `3856980` ; soumission `41796704`
- Leçon : un nœud qui trie ses fils coûte ~100 évaluations : budgéter en nœuds triés, pas en feuilles
