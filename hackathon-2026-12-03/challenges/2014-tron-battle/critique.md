# Critique — Tron Battle

## Avant implémentation — VALIDÉ AVEC RÉSERVES
- Le tracker doit effacer la bande du joueur mort : **réserve n°1**, c'est le
  bug le plus fréquent (divergence avec l'arbitre). Test dédié à prévoir.
- Tours séquentiels : le joueur qui bouge en second ne peut PAS entrer dans la
  case prise en premier. Le Voronoi 1-ply ignore cette nuance (acceptable en
  Wood/Bronze, à corriger en Silver avec 3 joueurs).
- Égalité Voronoi = case contestée, comptée 0 : cohérent avec le bot 78-lignes.
- Pas de float : score entier `100000*voronoi + ...` → départage stable.
- Python : `deque` obligatoire, pas de `pop(0)`.

## Réserves à lever avant Silver/Gold
- 3–4 joueurs : le Voronoi 2-joueurs ne suffit pas ; passer au « paranoid mixte »
  ou max^n.
- Le greedy primé doit être validé en fin de partie (sinon le bot surestime sa
  survie dans un plateau en damier).
- Tester les deux sièges (avantage du siège 70–83 %).

## Bilan après validation — 2026-11-21

- **Fuzz 210/210** identiques dans 4 langages (10 déterministes + 200 fuzz).
- **5/5 victoires** contre le boss Wood 2 `630143` (scores 100–90).
- Soumission retenue : `41777582` (`41777481` était une version buguée,
  remplacée avant promotion).
- Ligue Wood 2, rang 1/1621, score 21,14 > boss 18,91. Promotion en attente
  du cycle.

### Bugs réels trouvés par le fuzz (corrigés)
1. **NEI dupliqué (Python)** — `NB` contenait ~4 entrées par case, donc `mob`
   valait 6–12 et `greedy_fill` comptait mal les arêtes. Corrigé en construisant
   les voisins directement depuis les 4 directions.
2. **BFS sauté (4 langages)** — `if blocked[tête]: continue` empêchait tout BFS
   (la tête est toujours une case occupée). Voronoi toujours 0. Corrigé : la
   tête est l'origine légitime du BFS.
3. **`best_score=-1` (C)** — scores tous négatifs → aucun candidat ne remplaçait
   `UP`. Corrigé avec un drapeau `have`.
4. **Joueur mort (Python)** — `continue` sans avancer d'une ligne → le prochain
   bloc de 2 entiers était lu comme 4 têtes. Corrigé.

### Réserves restantes
- Tours séquentiels : le Voronoi 1-ply ignore que le second joueur ne peut pas
  entrer dans la case du premier (acceptable Wood/Bronze, à corriger en Silver).
- 3–4 joueurs (Silver/Gold) : passer au paranoid mixte ou max^n/PUCT.
- Avantage du siège non testé (70–83 % des ouvertures) → tester les deux sièges
  après promotion.
