# domain-expansion — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- https://github.com/Mortis66666/CG-Domain-Expansion — referee officiel (Kotlin). Lu via raw.githubusercontent.com (API GitHub limitée) :
  - `src/main/java/com/codingame/game/Referee.kt` (ordre des tours, init envoyée aux tours ≤ 2, fin quand le domaine du joueur ne contient plus l'autre jeton, scores = tailles) ;
  - `src/main/java/domainExpansion/Board.kt` (`findPath` BFS profondeur d'expansion 3 + Manhattan ≤ 3, `countDomain` ignore les jetons, murs de bord) ;
  - `Cell.kt` (`canGo` bloqué par le jeton seulement pour le déplacement), `Action.kt`, `Constant.kt` (7×7, 112 tours, 100 ms) ;
  - `config/Boss.py` : boss aléatoire (destination et mur au hasard).
- `bash bot-programming/tools/gh_search.sh "domain expansion codingame"` : aucun résultat (GitHub + GitLab). `"wall go ai minimax"` : aucun résultat. API GitHub ensuite en limite de débit (403).
- Idées générales : jeux de partition (Tron/Isola) → Voronoi + αβ (ALGO.md §2.3). Retenu.

## Faits du referee / contradictions avec l'énoncé
- L'énoncé dit « jusqu'à 3 pas » ; le referee accepte un chemin de 4 pas si la cible est à Manhattan ≤ 3 (BFS qui développe les nœuds de profondeur 3).
- Coup illégal → score -5, invalide -6, timeout -7.

## Classe de problème (ALGO.md) et approche retenue
Partition / expansion. αβ sur actions + Voronoi, fin exacte.
