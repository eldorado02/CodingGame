# hex — plan

1. Protocole exact : init `boardSize playerColor` ; tour : `lastMove` puis `boardSize` lignes de cases séparées par des espaces ; une ligne de sortie (`d4` ou `swap-pieces`) flushée.
2. Invariant d'état : aucun état mémorisé, le plateau complet est relu chaque tour. Swap possible si je suis bleu, plateau = 1 pierre rouge et `lastMove != empty`.
3. Algorithme : (a) plateau vide → ouverture `c2` ; (b) swap si la pierre rouge est hors bord ; (c) coup gagnant immédiat ; (d) blocage d'un gain adverse immédiat ; (e) MCTS + RAVE : sélection `v = (1-β)·Q + β·AMAF`, `β = an/(an+n+0.04·an·n)`, nœud non vu = 0,5 ; expansion après 3 visites (tous les coups vides, ordre croissant) ; playout = remplissage aléatoire (Fisher-Yates xorshift64, index `((x>>32)·(i+1))>>32`) puis un seul BFS (Hex n'a pas de nul, le gagnant d'un plateau plein est unique) ; coup final = enfant le plus visité. Budget : 2600 itérations (≈ 37 ms CG), 30000 au 1er tour.
4. Départage : `>` strict, premier enfant (index de case croissant) gagne ; même RNG et même graine chaque tour dans les 4 langages ; seuls `/`, `*`, `+` en double (pas de `log`).
5. Matrice de fuzz : plateau vide, 1 pierre rouge (swap ou non, bords inclus), positions 1–30 et 30–80 pierres, gains/blocages immédiats ; 1 à 3 tours par cas.
6. Hypothèse de victoire : un MCTS-RAVE de quelques milliers de playouts bat les bots heuristiques/LLM du milieu de tableau. Réfutation : défaite sur une échelle (ladder) ou un pont coupé → ajouter la sauvegarde de ponts dans les playouts.
