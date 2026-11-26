# code-keeper---the-hero — recherche

## Sources
- Referee : https://github.com/acatai/CodeKeeper-TheHero (MazeManager.java,
  MazeGenerator.java, Monster.java, Constants.java), lu le 2026-11-26.

## Faits du referee
- Monstre : attaque si distance Chebyshev <= portée, sinon avance s'il voit le
  héros (Chebyshev <= fov) ; il ne vient donc jamais d'office sur un côté
  cardinal : il s'arrête en diagonale et frappe. Attendre qu'il vienne à l'épée
  est une perte de tours (v1 : 2 tours sur 3 passés à attendre).
- Pathfinding monstre : BFS limité à sa boîte de vision, minimise la distance
  de Manhattan, les autres monstres sont des obstacles.
- Générateur : sortie à distance BFS >= 16 du départ ; 5 à 9 trésors (souvent en
  cul-de-sac) ; 5 à 15 boîtes ; 4 à 7 potions ; 0 à 4 coffres par arme ;
  30 à 50 monstres, aucun à distance <= 2 du départ.
- Ordre d'action des monstres : aléatoire à chaque tour.

## Classe de problème (ALGO.md) et approche retenue
Exploration en brouillard + combat à règles (pas de recherche adversariale).
