# clobber — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Clobber — v1 αβ mobilité soumise
- Constat : jeu jamais joué ; pas de liste de coups fournie
- Hypothèse : αβ + éval de mobilité (classique Clobber) bat le boss Wood 2
- Sources : énoncé, referee darkhorse64/CG_Clobber, GitHub TidyMaze/clobber (MCTS + minimax mobilité), dépôts αβ académiques (gh_search.sh)
- Action : 4 langages ; comptage de mobilité par décalages/popcount (×2,6) ; budget 60k nœuds
- Résultat : 3/4 contre le boss `3835619` mais grâce à ses timeouts ; soumission `41796537`
- Leçon : un boss qui dépasse le temps gonfle les résultats : compter les parties complètes séparément
