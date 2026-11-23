# paper-soccer — recherche

## Sources (URL, langage, idée retenue / rejetée et pourquoi)
- https://github.com/jdermont/CodinGame-paper-soccer — referee officiel (Java). Lu : `Pitch.java` (géométrie 105 points, bords pré-dessinés, paires retirées aux poteaux, règle de rebond `free < degree - 1`, fin = but ou blocage), `MoveVerificator.java`, `Referee.java` (66 tours max, 200 ms, `myId` 0/1, `1` + `-` au 1er tour). Retenu tel quel dans le moteur.
- https://raw.githubusercontent.com/jdermont/CodinGame-paper-soccer/master/config/Boss.java — boss : choix aléatoire parmi ≤ 10 tours non bloquants (lu par OpenCode).
- https://github.com/Lecorbio/paper-soccer-strategy-engine (`README.md`, `docs/algorithms.md`) — rang 2/212 : αβ sur tours complets, éval apprise ; éval manuelle documentée (but dispo 50 000, **distance BFS sur arêtes libres 120**, progression verticale 80, mobilité 20…). Retenu : le terme distance BFS 120 + progression 80. Rejeté pour v1 : réseau de valeur, table de transposition (coût 4 langages).
- https://raw.githubusercontent.com/jdermont/YaPaperSoccer/master/ai_readme/paper_soccer.md — jacek (rang 1) : minimax αβ + ID, un coup = tour complet avec rebonds (via OpenCode, résumé vérifié cohérent avec notre moteur).
- https://raw.githubusercontent.com/jdermont/playground-kvhfh5iv/master/papersoccer.md — entrées de réseau de neurones de jacek (1156 entrées). Non retenu (v1).
- https://arxiv.org/abs/2012.10700 — *Minimax Strikes Back* (best-first minimax + UCT), cité par jacek. Non retenu (v1).
- https://raw.githubusercontent.com/bytesroboticmx/soccerBot/master/README.md — αβ + MCTS/RAVE (concours MIM UW 2014). Non retenu.
- `bash bot-programming/tools/gh_search.sh "paper soccer codingame"` → les 2 dépôts ci-dessus.
- Rapport OpenCode complet (hy4-flash-free, 37 URL) : `research-opencode-20261123-202942.md`.

## Faits du referee / contradictions avec l'énoncé
- Points x 0..8, y 0..10 ; buts : x 3..5 à y = -1 (le joueur 0 y marque) et y = 11 (joueur 1). Départ (4,5).
- Paires retirées (poteaux) : (2,0)-(3,-1), (6,0)-(5,-1), (2,10)-(3,11), (6,10)-(5,11).
- Tour terminé sur un point sans arête libre → le joueur qui vient de jouer perd ; but contre son camp → défaite.
- 66 tours maximum ; aucun vainqueur fixé dans le code à cette limite (nul présumé) [NON VÉRIFIÉ].

## Classe de problème (ALGO.md) et approche retenue
B (plateau abstrait) avec coups composés : αβ + ID sur tours complets, DFS des rebonds dédupliquée par (point d'arrivée, issue).
