# dots-and-boxes — guide de reprise

- Énoncé : `statement.md`. Plateau n×n boîtes (2×2 en Wood 2, plus grand ensuite : relire l'énoncé à chaque promotion). Fermer une boîte = la gagner et rejouer. Ligues Wood 2 → Wood 1. Boss Wood 2 : `6797561`.
- Protocole : `boardSize`, id (`A`/`B`) ; par tour `myScore oppScore`, N boîtes jouables `A1 LTRB` (côtés libres). Sortie `A1 R`. 1 s puis 100 ms. État complet à chaque tour (aucun suivi nécessaire).
- Recherche : loicpetit/codingame-dots-and-boxes (JS).
- Bot soumis : **v1** `41796861` — arêtes indexées (horizontales puis verticales), negamax αβ où fermer une boîte = tour supplémentaire sans changement de signe ; tri : fermer > sûr > cadeau ; éval = écart + boîtes capturables. Prof. 9 sur 2×2.
- Bench v1 : 4/4 (4–0) contre le boss Wood 2.
- Prochaine étape : grands plateaux → théorie des chaînes (contrôle par « double-cross », règle de parité des chaînes longues).
- Fuzz : `bash tests/run.sh` (tailles 2–5, 18 s).
