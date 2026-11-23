# domain-expansion — guide de reprise

- Énoncé : `statement.md` (archivé le 2026-11-23). Ligue unique `Wood` (92 joueurs) : pas de promotion, seul le rang compte.
- Referee : https://github.com/Mortis66666/CG-Domain-Expansion (Kotlin, lu : `Referee.kt`, `Board.kt`, `Cell.kt`, `Action.kt`, `Constant.kt`, `config/Boss.py`).
- Protocole : init `7 7`, ma position, sa position (joueur 0 en (0,0), joueur 1 en (6,6)) ; chaque tour `x y D` = action adverse (`-1 -1 _` au tout 1er tour). Sortie `x y D` (D ∈ U R D L). 1000 ms au 1er tour, 100 ms ensuite, 112 tours max.
- Règle subtile (referee `findPath`) : cible à Manhattan ≤ 3, mais le BFS développe les nœuds jusqu'à la profondeur 3 → **chemins de longueur 4 autorisés** (contournement). Le jeton adverse bloque le passage, pas le décompte de domaine.
- Fin : dès que le joueur qui vient d'agir ne peut plus atteindre l'autre jeton (murs seuls) ; score = taille des domaines.
- État mémorisé : murs + positions, mis à jour par l'action adverse puis la nôtre.
- Template : `templates/board-2p` (moteur remplacé). Classe ALGO.md : **E/P** (partition).
- Bot soumis : **v1** `41796695` (C++) — negamax αβ + ID sur les actions (déplacement × mur), racine triée par valeur à 1 pli, éval Voronoi (BFS à travers les murs), fin de partie exacte ; 10000 nœuds/tour (60000 au 1er).
- Résultats v1 : **2/3** contre le rang 43 `6247522`, **3/3** contre le rang 25 `6763488`. CG : 28 ms au pire (97 ms au 1er tour).
- Défaite analysée (`v1-r43-20261123-205124`) : l'adversaire ferme une grande zone pendant que notre Voronoi à profondeur 2–3 ne voit pas la fermeture. Piste : éval « zone fermable » (articulations), budget plus grand (marge 28 → 40 ms), tri des coups internes.
- Commandes :
  - fuzz : `bash hackathon-2026-12-03/challenges/domain-expansion/tests/run.sh` (CG_NODES=300, ~1 min)
  - bench : `python3 bot-programming/tools/bench.py domain-expansion hackathon-2026-12-03/challenges/domain-expansion/solutions/cpp/bot.cpp 6247522 --n 3`
  - submit : `python3 bot-programming/tools/cg.py submit domain-expansion hackathon-2026-12-03/challenges/domain-expansion/solutions/cpp/bot.cpp --language C++`
