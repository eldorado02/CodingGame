# amazons — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Amazons — v1 territoire 2 étages soumise
- Constat : jeu jamais joué ; ~2000 actions par tour, liste non fournie
- Hypothèse : classer les déplacements puis tester les flèches des 10 meilleurs avec une éval territoire suffit contre le boss
- Sources : énoncé, referee MultiStruct/Amazons, ALGO.md (territoire = Voronoi en distance-reine)
- Action : 4 langages ; 1re version à budget d'actions (préfixe biaisé, 61 ms) remplacée par la recherche en 2 étages ; fuzz 200/200
- Résultat : 3/3 (2–0) contre le boss `6797567` ; soumission `41796775`
- Leçon : quand le branchement explose, un pré-classement grossier + faisceau bat un budget qui ne voit qu'un préfixe de la liste
