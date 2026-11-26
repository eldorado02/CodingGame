# tulips-and-daisies — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Tulips and Daisies — v1->v3 glouton fenêtres + trous
- Constat : v1 timeout (60 ms, copies de grille) + or épuisé ; v2 rapide (2 ms) mais 17 vs 374
- Hypothèse : le boss comble des trous entre tronçons de 3 (33 or) ; nous récoltons des 4 (7 or)
- Sources : sonde stdin ; log des coups du boss ; referee WildSmilodon
- Action : v2 sans copie (case surchargée), réserve d'or ; v3 potentiel trou
- Résultat : fuzz 200/200 ; v3 1/3 (victoire = timeout du boss), 21-43 vs 281-374 ; soumis (premier envoi)
- Leçon : Fibonacci : la valeur est dans les grandes jonctions, pas les 4 ; prévoir une éval de plateau v4
