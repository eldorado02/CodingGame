# dice-duel — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-26, relire après chaque promotion).
- Template de départ : `templates/board-2p (moteur réécrit, génération par DFS)` (voir `bot-programming/ALGO.md`).
- Ligue actuelle : Wood 2 (cible Wood 1 : passer SUM_RULE à true après promotion).
- Bot soumis : v1 alpha-bêta matériel, submission 41820534 (2026-11-26).
- Adversaires : boss Wood 2 (`boss`).
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/dice-duel/tests/run.sh`
  - test : `python3 bot-programming/tools/cg.py test dice-duel hackathon-2026-12-03/challenges/dice-duel/solutions/cpp/bot.cpp --language C++ --opponents <id>`
  - submit : `python3 bot-programming/tools/cg.py submit dice-duel hackathon-2026-12-03/challenges/dice-duel/solutions/cpp/bot.cpp --language C++`
