# hex — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (125 joueurs) : pas de promotion, seul le rang compte.
- Protocole : ligne d'init `11 red|blue` ; chaque tour `lastMove` (`empty`, `d4`, `swap-pieces`) puis 11 lignes `". . r b ..."` (cases séparées par des espaces). Sortie : `d4` ou `swap-pieces` (bleu, tour 2 seulement). 1000 ms au 1er tour, 100 ms ensuite.
- Coordonnées : lettre = colonne, chiffre = ligne ; case = `ligne*11+col`. Voisins **vérifiés par test game** (`probe3`) : `(-1,0) (-1,1) (0,-1) (0,1) (1,-1) (1,0)`. Rouge relie ligne 0 → ligne 10, bleu colonne 0 → colonne 10.
- Le bot est **sans état** : tout est relu depuis le plateau à chaque tour.
- Template : `templates/board-2p` (moteur remplacé). Classe ALGO.md : **BL / C** (connexion).
- Bot soumis : **v1** `41796234` (C++) — coup gagnant / blocage forcé, puis MCTS + RAVE (sans terme d'exploration), playouts par remplissage aléatoire, 2600 itérations/tour (30000 au 1er tour). Ouverture rouge `c2`. Bleu swappe si la pierre rouge n'est pas sur un bord.
- Résultats v1 : **3/3** contre le rang 61 `6506441` (Gemini3.5Flash).
- Budget CG mesuré : 3000 itérations = 40–44 ms sur CG (≈ 14 µs/itération) → 2600 retenu.
- Prochaine étape : playouts avec sauvegarde de ponts, réutilisation de l'arbre, puis bench contre le top 20.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/hex/tests/run.sh` (CG_NODES=400, ~30 s)
  - bench : `python3 bot-programming/tools/bench.py hex hackathon-2026-12-03/challenges/hex/solutions/cpp/bot.cpp 6506441 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit hex hackathon-2026-12-03/challenges/hex/solutions/cpp/bot.cpp --language C++`
