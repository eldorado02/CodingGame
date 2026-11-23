# isola — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Isola — v1 soumise
- Constat : jeu jamais joué ; ligues Wood 4 → Wood 1
- Hypothèse : αβ sur (déplacement, retrait) avec retraits limités aux libertés adverses + éval Voronoi bat le boss Wood 4
- Sources : énoncé, source wala-fr/CodingameIsola citée, ALGO.md classe P
- Action : 4 langages, fuzz 200/200 auto-joué ; budget abaissé 30k → 10k nœuds après TIMEGUARD sur CG
- Résultat : 6/6 contre le boss Wood 4 `4534449` ; soumission `41796400`
- Leçon : une éval à BFS par feuille coûte ~6× une éval matérielle : recalibrer le budget sur les logs serveur
