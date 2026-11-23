# breakthrough — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (389 joueurs) : pas de promotion, seul le rang compte.
- Protocole : chaque tour `opponentMove` (`None` au 1er tour si on joue Blanc), `legalMoves`, puis les coups (`c2c3`). Sortie : un coup + flush. 1000 ms au 1er tour, 100 ms ensuite.
- Template : `templates/board-2p` (αβ + ID, budget en nœuds). Classe ALGO.md : **B** (plateau abstrait).
- Bot soumis : **v3** `41795905` (C++), bitboards + règle des coureurs exacte à chaque nœud.
- Résultats v3 : 4/4 contre le rang 97 `5322777`, 0/4 contre le rang 33 `6829501`.
- Budget CG mesuré : ~60k nœuds = 10–36 ms (CG ~2× plus lent que local avec pragma).
- Prochaine étape : TT Zobrist + éval « coureur sûr » (pièce progrès 5 non attaquable), viser le rang 33.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/breakthrough/tests/run.sh` (CG_NODES=3000, ~3 min)
  - bench : `python3 bot-programming/tools/bench.py breakthrough hackathon-2026-12-03/challenges/breakthrough/solutions/cpp/bot.cpp 6829501 --n 4`
  - submit : `python3 bot-programming/tools/cg.py submit breakthrough hackathon-2026-12-03/challenges/breakthrough/solutions/cpp/bot.cpp --language C++`
