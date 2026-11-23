# domain-expansion — plan

1. Protocole : voir README. Une ligne `x y D` flushée par tour.
2. Invariant d'état : `walls[cell]` (masque U R D L, bords inclus) et les deux jetons, mis à jour par l'action adverse lue puis par la nôtre.
3. Algorithme : actions = destinations (BFS du referee : expansion jusqu'à la profondeur 3, Manhattan ≤ 3, jeton adverse bloquant, ordre croissant des cases) × murs libres (U R D L). Après chaque action : si les jetons sont séparés → score exact `±(WIN + diff)`. Negamax αβ + ID (racine triée par valeur à 1 pli, insertion stable), budget en nœuds (10000 / 60000), garde-temps 40 ms. Éval = cases strictement plus proches de nous − de lui (BFS à travers les murs).
4. Départage : `>` strict, ordre fixe des actions.
5. Fuzz : flux multi-tours, nos réponses par la référence Python (CG_NODES=300), adversaire aléatoire seedé qui évite de finir la partie.
6. Hypothèse : Voronoi + recherche 2–4 plis bat le milieu de tableau. Réfutation : fermeture d'une grande zone adverse non vue → éval par zones fermables.
