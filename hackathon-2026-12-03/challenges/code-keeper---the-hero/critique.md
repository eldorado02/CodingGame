# code-keeper---the-hero — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Code Keeper - The Hero — v1->v3 règles + BFS, portages C/C++/Rust, soumission
- Constat : C/C++/Rust encore le template TTT, gen_fuzz template ; v1 0/2 (attend 2 tours sur 3, pas de sortie) ; v2 0/3 (oscille entre 2 cases)
- Hypothèse : les monstres s'arrêtent en diagonale (pas d'attente utile) ; un monstre hors vue ne bouge pas donc on peut le mémoriser
- Sources : referee acatai/CodeKeeper-TheHero MazeManager.turnMonsters/monsterPathfinding, MazeGenerator
- Action : portages 4 langages, gen_fuzz instantanés, suppression de l'attente, mémoire des monstres
- Résultat : fuzz 200/200 ; v3 0/3 (1 sortie 10185, 2 morts en 9 tours encerclé) ; soumis (premier envoi)
- Leçon : lire le pathfinding des monstres avant d'écrire une règle d'attente ; prochain : fuir vers un couloir quand >=2 monstres attaquent
