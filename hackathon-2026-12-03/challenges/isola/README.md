# isola — guide de reprise

- Énoncé : `statement.md` ; source github.com/wala-fr/CodingameIsola. 9×9, déplacer son pion d'un pas (8 directions) puis retirer une case libre ; bloqué à son tour = perdu. Ligues Wood 4 → Wood 1 (ligue max). Boss Wood 4 : `4534449`.
- Protocole : init `x`, `y` (notre pion) ; par tour `oppX`, `oppY`, `removedX`, `removedY` (`-1` au 1er tour) ; sortie `mx my rx ry` ou `RANDOM`. 1000 ms / 100 ms.
- Bot soumis : **v1** `41796400` — negamax αβ ID sur (déplacement, retrait) avec retraits limités aux libertés adverses ; éval Voronoi (BFS pas de roi) + 3 × mobilité. Budgets : 60k nœuds tour 1, 10k ensuite (BFS coûteux : 30k nœuds déclenchaient le garde-temps sur CG).
- Bench v1 : 3/3 + 3/3 contre le boss Wood 4.
- Prochaine étape : suivre la promotion ; contre le boss suivant, ajouter une TT et la fin de partie séparée (remplissage maximal).
- Fuzz : `bash tests/run.sh` (flux auto-joués, CG_NODES=800, ~2 min).
