# paper-soccer — plan

1. Protocole : `myId` (0 = attaque le haut), puis chaque tour `longueur` + coup adverse (`-` au 1er tour du joueur 0). Sortie : chaîne de directions 0..7 flushée.
2. Invariant d'état : le terrain (arêtes dessinées + balle) est **mémorisé** et mis à jour avec le coup adverse puis le nôtre.
3. Algorithme : `generate_turns` = DFS des rebonds (directions 0..7, plafond 20 000 pas), on garde la 1re séquence par (point d'arrivée, issue ∈ {normal, gagné, perdu}). Negamax αβ + ID sur tours complets, budget en nœuds (3000 / 15000 au 1er tour) + garde-temps 80 ms. Éval (joueur au trait) : `80·(rangs adv − rangs moi) + 120·(distance BFS arêtes libres adv − moi) ± 15·décentrage près des buts`.
4. Départage : tri par insertion stable (gagné > normal par rangs restants > perdu), `>` strict, ordre DFS fixe.
5. Fuzz : flux multi-tours d'une même partie, nos réponses calculées par la référence Python (CG_NODES=300), adversaire aléatoire seedé (40 % glouton vers son but) → chaînes de rebonds et positions près des buts.
6. Hypothèse de victoire : la recherche tours complets voit les buts en 2–3 tours que les bots heuristiques ratent. Réfutation : défaite par blocage ou but adverse après une longue chaîne → affiner le plafond DFS et l'éval mobilité.
