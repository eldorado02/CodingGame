# twixt-pp — plan

1. Protocole : voir README. Une ligne de sortie flushée (`F4` ou `SWAP`).
2. Invariant d'état : seul le côté (haut/bas ou gauche/droite) est mémorisé (1er tour). Plateau et liens relus chaque tour depuis l'entrée.
3. Algorithme : géométrie précalculée (440 segments de cavalier, liste des segments croisés par chacun, ≤ 9). `State.place` crée les liens comme le referee (cavalier, pas de croisement avec un lien adverse ; PP : nos liens peuvent se croiser). `cut[p][seg]` compte les liens adverses qui croisent `seg`. Distance = BFS 0-1 (peg à nous 0, trou jouable 1). Éval = `100·(dist adv − dist moi) + 3·(liens moi − liens adv)`. Recherche : tous les coups à 1 pli, tri stable, 2 plis (pire réponse adverse, coupure dès `≤` meilleur) sur les `TOP_K = 16` premiers.
4. Départage : `>` strict, tri par insertion stable, ordre des cases croissant ; la distance BFS ne dépend pas de l'ordre de parcours.
5. Fuzz : flux de 1–3 tours d'une même partie aléatoire (FIRST, SWAP, ouverture adverse pour la règle de swap, milieux de partie), listes de pegs/liens mélangées.
6. Hypothèse : la distance en pegs suffit contre les bots faibles. Réfutation observée : 0/5 contre le top 25 (agglutinement au bord de départ).
