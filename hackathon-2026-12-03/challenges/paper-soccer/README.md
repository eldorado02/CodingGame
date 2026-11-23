# paper-soccer — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (241 joueurs) : pas de promotion, seul le rang compte.
- Referee : https://github.com/jdermont/CodinGame-paper-soccer (lu, voir `research.md`).
- Protocole : `myId` (0 attaque le but du haut y = -1, 1 le bas y = 11) ; chaque tour : longueur puis coup adverse (`-` au 1er tour). Sortie : chaîne de directions `0`=N … `7`=NW. 1000 ms au 1er tour, 200 ms ensuite, 66 tours max.
- Le terrain est **mémorisé** entre les tours (le referee n'envoie que le dernier coup adverse).
- Template : `templates/board-2p` (moteur remplacé). Classe ALGO.md : **B** (coups composés).
- Bot soumis : **v1b** `41796586` (C++) — αβ + ID sur tours complets (DFS des rebonds, 1re séquence par (arrivée, issue)), éval rangs + distance BFS sur arêtes libres (idée Lecorbio) ; 3000 nœuds/tour (15000 au 1er), garde-temps 80 ms vérifié à chaque nœud.
- Résultats : v1 **3/3** rang 97 `4440891`, **3/3** rang 49 `2767844` ; v1b **3/3** rang 33 `5296809`. Pire tour CG 81 ms (garde déclenchée 5 fois en fin de partie).
- Prochaine étape : table de transposition, mobilité dans l'éval, bench contre le top 10 (`6876815` jacek = rang 1).
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/paper-soccer/tests/run.sh` (CG_NODES=300, ~2 min)
  - bench : `python3 bot-programming/tools/bench.py paper-soccer hackathon-2026-12-03/challenges/paper-soccer/solutions/cpp/bot.cpp 5296809 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit paper-soccer hackathon-2026-12-03/challenges/paper-soccer/solutions/cpp/bot.cpp --language C++`
