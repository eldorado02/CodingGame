# breakthrough — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### v1 — tableau 64 cases, αβ template
- Constat : 0/3 contre le rang 97, 0/2 contre le rang 33 ; `TIMEGUARD` à chaque tour, y compris au 1er tour (600k nœuds).
- Hypothèse : CodinGame compile le C++ sans `-O2`.
- Action : `#pragma GCC optimize("O3,unroll-loops")`, diagnostics `d= n= ms=` sur stderr.
- Résultat : 600k nœuds = 494 ms sur CG (1,2 M/s) contre 114 ms en local -O2 ; 1/3 puis 1/4 après recalibrage (60k nœuds).
- Leçon : pragma O3 obligatoire, STL lente même avec pragma (tri par insertion maison) ; calibrer sur ~1 M nœuds/s CG.

### v2 → v3 — bitboards + règle des coureurs
- Constat (v2) : défaite type : un pion noir en b3 → a2 → b1 ; prof. 4 ne voit pas la percée (5 plis).
- Hypothèse : une règle exacte « coureur à 1 pas » à chaque nœud ajoute 2 plis de vision gratuitement.
- Sources : log `v2-r97-*.log` ; règles officielles (énoncé).
- Action : bitboards (3× plus rapide : 600k nœuds en 71 ms local), `runner_rule`.
- Résultat : **4/4** contre le rang 97 `5322777`, **0/4** contre le rang 33 `6829501`. Fuzz 200/200. Soumission `41795905`.
- Leçon : dans les jeux de course, détecter exactement les menaces de dernière ligne vaut plus qu'un ply de recherche.

### À tester ensuite
- TT Zobrist + killer ; coureur « sûr » à 2 pas (non attaquable, chemin libre) ; quiescence sur captures.
