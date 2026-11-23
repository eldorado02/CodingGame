# bandas — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (649 joueurs) : pas de promotion, seul le rang compte.
- Referee : https://github.com/Oli8/CG-bandas (`Referee.java`, lu en entier).
- Protocole : init `myId`, `height`, `width` (8, 8) ; chaque tour 8 lignes de 8 jetons séparés par des espaces (`0`, `1`, `-` vide, `x` trou). Sortie `UP|RIGHT|DOWN|LEFT`. 100 ms par tour, 200 tours max (puis plus de pions gagne).
- Le bot est **sans état** (grille complète à chaque tour).
- Règles portées à l'identique (ordre de balayage du referee, poussée récursive, pion qui sort ou tombe dans un trou = perdu, lignes puis colonnes vides des bords transformées en trous ; 0 pion des deux côtés = victoire du joueur 1).
- Template : `templates/board-2p` (moteur remplacé). Classe ALGO.md : **B** (branching 4).
- Bot soumis : **v1** `41796751` (C++) — negamax αβ + ID (profondeur 7–8), éval `100·pions + 3·distance de chute (plafond 3)`, 10000 nœuds/tour.
- Résultats v1 : **3/3** rang 302 `6178566`, **3/3** rang 130 `4763290`, **0/2** rang 44 `3789187` (eulerscheZahl). CG : 29 ms au pire.
- Prochaine étape : moteur plus rapide (bitboards 64 bits, compteurs incrémentaux) pour gagner 2–3 plis ; éval de mobilité/centre.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/bandas/tests/run.sh` (CG_NODES=1500, ~50 s)
  - bench : `python3 bot-programming/tools/bench.py bandas hackathon-2026-12-03/challenges/bandas/solutions/cpp/bot.cpp 4763290 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit bandas hackathon-2026-12-03/challenges/bandas/solutions/cpp/bot.cpp --language C++`
