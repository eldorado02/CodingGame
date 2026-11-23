# tower-dereference — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- https://github.com/eulerscheZahl/TowerDefense — referee (Java). Lus via raw.githubusercontent.com :
  - `src/main/java/TowerDefense/Constants.java` : coûts (canon 100, feu 100, colle 70, soin 100), tables damage/range/reload, améliorations 50/100/150, vagues (`WAVE_START`, `WAVE_COUNT`, `WAVE_HP` 5..18, `WAVE_SPEED`, `WAVE_BOUNTY`), 200 tours, 350 or, 20 vies.
  - `Tower.java` (`inRange` sur les sous-cases, `cooldown = reload - 1`, impression `0.#`), `GunTower.java` (cible la plus proche de notre base).
  - `Board.java` (création des vagues, vague suivante dès que les attaquants d'un joueur ont parcouru 2/3 du chemin, ordre de tir).
  - `com/codingame/game/Referee.java` (actions `;`, améliorations immédiates, constructions différées, conflits), `Player.java` (score `100·vies + argent`).
  - `config/Boss.cs` : construit des canons sur les cases maximisant Σ 1/d² vers le canyon.
- `bash bot-programming/tools/gh_search.sh "tower dereference codingame"` : aucun dépôt de bot public.
- Logs : `games/tower-dereference/logs/probe-py-*`, `diag-*`, `v1b-*`, `v1c-*`.

## Faits du referee / contradictions avec l'énoncé
- Les vagues s'enchaînent plus vite quand un joueur tue vite (`canCreateAttackers`) ; après la 9e vague, vagues inventées (+1 unité, +1 PV).
- Le chemin de chaque attaquant est raccourci de 0 à 9 sous-cases à la fin.

## Classe de problème (ALGO.md) et approche retenue
Gestion / placement : glouton gain/coût sur un modèle de couverture du chemin.
