# ALGO — patterns relevés par l'agent 2 (à fusionner dans ALGO.md §6)

| Pattern | Preuve | Règle |
|---|---|---|
| Géométrie non précisée par l'énoncé (diagonale hex, orientation lettre/chiffre) | Hex (sonde anti-diagonale), TwixT-PP (bot public) | vérifier sur une vraie partie (chemin gagnant final) avant d'écrire le moteur |
| Éval « distance de connexion » plate | TwixT-PP v1 : égalités → coups au bord de départ, 0/5 vs top 25 | toujours un départage secondaire (centralité, nombre de chemins courts) |
| Milieu de tableau des jeux communautaires = bots stub invalides | TwixT-PP rangs 49 et 65 | bencher aussi contre le top 25 pour mesurer la force réelle |
| MCTS sur CG : expansion à chaque visite = création de nœuds dominante | Hex v1 (14 µs → 6 µs/itération avec seuil 3 + tableaux statiques) | seuil d'expansion ≥ 3, pas de `std::vector` dans l'arbre |
| Coups composés (rebonds, rafles) : coût d'un nœud très variable | Paper Soccer v1 : 3000 nœuds = 4 ms au début, > 150 ms en fin de partie | garde-temps vérifié à chaque nœud, pas tous les 64 |
