# Brandubh — critique

## Avant implémentation

- Le premier prototype ne lisait qu'un tour puis quittait : timeout au frame 3.
- L'évaluation initiale utilisait une distance Manhattan au coin, ce qui
  sous-estime la capacité du roi à fuir en ligne droite sur une longueur libre.
- Le BFS d'encerclement appelé à chaque feuille coûtait ~46 ms ; il a été retiré
  de l'évaluation pour respecter la limite de 50 ms, puis réintroduit avec une
  garde terminale peu coûteuse.

## Après implémentation

### Tentatives rejetées

- `v1` (alpha-beta 2 plis, BFS d'encerclement à chaque feuille) : timeout.
- `v2` (sans BFS d'encerclement, distance Manhattan) : 0/5, le roi s'échappe.
- `v3` (poids distance 300, contrôle 100) : 0/5, le roi s'échappe en ~30 tours.
- `v4` (BFS d'évasion) : le BFS laissait traverser les défenseurs ; 0/5.
- `v5` (proximité des attaquants avec mauvais signe) : 0/5, repousse les attaquants.
- `v6` (BFS avec murs + proximité corrigée) : 0/5.
- `v7` (garde du anneau initial, bonus 250) : 0/5.
- `v8` (C++ alpha-beta 4 plis, coupe 42 ms) : 0/5, roi échappé en 10 tours.
- Bugs corrigés en chemin : boucle de lecture unique, orientation des lignes,
  BFS d'encerclement sans test « roi atteint », départage alpha-beta sur bornes.
- Leçon : il manque un modèle de contrôle des couloirs de fuite vers les coins.

## Leçons

- Corriger d'abord la boucle de lecture avant d'attribuer les défaites à l'évaluation.
- Une évaluation doit être testée sur une position où le roi a une sortie évidente,
  pas seulement sur la position initiale.
- Le quota `30 test games / 900 s` est vite épuisé quand on balaie des variantes ;
  regrouper les modifications indépendantes.

### v9 — réécriture template board-2p (2026-11-23)
- Constat : v8 perd 0/3 contre le rang 15 (partie identique à chaque fois) ; aucune soumission n'existait.
- Hypothèse : règles mal modélisées + absence d'anti-répétition ; le moteur doit suivre le referee exactement.
- Sources : `Referee.java` (captures actives seulement, roi armé, trône vide hostile sauf au roi, trône occupé hostile aux attaquants seulement, roi sur trône = 4 côtés, encerclement par BFS depuis le bord, répétition ×3 = défaite du joueur qui joue).
- Action : moteur 4 langages, règle exacte « roi a une ligne libre vers un coin → gain du défenseur », éval matériel + attaquants au contact du roi + distance-roi-coin (BFS en coups de tour) ; fuzz 200/200.
- Résultat : 0/3 contre le rang 15 en attaquant. Variante bonus cases adjacentes aux coins (25 puis 80) : 0/3 — le roi capture une pièce adjacente au coin contre le coin (le coin est hostile) puis s'échappe. Bonus déplacé sur les diagonales b2/b6/f2/f6 : toujours 0/3 (échappée a4a7). Soumission `41795979` (première soumission autorisée sous 3/5, règle du 2026-11-23).
- Leçon : en tafl, ne jamais garder un coin depuis une case adjacente (capturable contre le coin) ; profondeur 3 insuffisante pour l'attaquant : il faut un moteur plus rapide.
