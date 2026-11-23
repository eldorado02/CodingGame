# twixt-pp — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (128 joueurs) : pas de promotion, seul le rang compte.
- Protocole : chaque tour `hisLastPeg` (`FIRST`, `SWAP` ou `D6`), puis mes pegs, mes liens (`A1 B3`), ses pegs, ses liens. Sortie : un trou (`F4`) ou `SWAP` (2e joueur, 1er tour). 1 s au 1er tour, 300 ms ensuite.
- Coordonnées : lettre = colonne x (A = 0), chiffre = ligne y (1 = 0) ; case = `y*12+x`. Le 1er joueur relie haut/bas et ne joue jamais en colonne A/L ; le 2e relie gauche/droite. Coins interdits. Confirmé par le bot public `TidyMaze/Twixt-PP` et par les test games (aucun coup invalide de notre part).
- Le côté est fixé au 1er tour (`FIRST` → haut/bas) ; après `SWAP` adverse on garde haut/bas.
- Template : `templates/board-2p` (moteur remplacé). Classe ALGO.md : **C** (connexion).
- Bot soumis : **v1** `41796478` (C++) — éval = distance en pegs (BFS 0-1 sur les sauts de cavalier, segments coupés par un lien adverse) adverse − nôtre + 3·liens ; classement 1 pli puis 2 plis sur les 16 meilleurs.
- Résultats v1 : 3/3 contre les rangs 65 et 49 (ils jouent des coups invalides : bots « stub ») ; **0/3** contre le rang 25 `3961435`, **0/2** contre le rang 9 `6785670`.
- Budget CG mesuré : 30 ms au pire (limite 300 ms) → marge pour TOP_K plus grand.
- Faiblesse connue : les égalités de distance sont départagées par l'index de case → le bot s'agglutine près de son bord de départ (partie `v1-r9-20261123-202517` : B1 D2 F3 C4 H2 F1 E3). Prochaine étape : départage par centralité, éval « nombre de chemins courts », puis MCTS.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/twixt-pp/tests/run.sh` (CG_NODES=3 = TOP_K, ~80 s)
  - bench : `python3 bot-programming/tools/bench.py twixt-pp hackathon-2026-12-03/challenges/twixt-pp/solutions/cpp/bot.cpp 3961435 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit twixt-pp hackathon-2026-12-03/challenges/twixt-pp/solutions/cpp/bot.cpp --language C++`
