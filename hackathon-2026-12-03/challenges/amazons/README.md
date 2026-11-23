# amazons — guide de reprise

- Énoncé : `statement.md` ; referee github.com/MultiStruct/Amazons ; boss par Astrobytes. 8×8, position de départ aléatoire, 2 manches couleurs inversées (la couleur est relue à chaque tour). Action = déplacement de reine + flèche (mur) en reine depuis l'arrivée. Ligues Wood 2 → Wood 1. Boss Wood 2 : `6797567`.
- Protocole : `boardSize` ; par tour couleur, n lignes (haut → bas, `-` = mur), `lastAction`, `actionsCount` (liste NON fournie). Sortie `d8d1d7`. 1000 / 100 ms.
- Bot soumis : **v1** `41796775` — recherche à 1 coup en 2 étages : tous les déplacements classés sans flèche, les 10 meilleurs reçoivent toutes leurs flèches ; éval = territoire en distance-reine (BFS multi-sources, égalités neutres) + mobilité. ~300 évaluations/tour (CG ≤ 2 ms).
- Bench v1 : 3/3 (2–0) contre le boss Wood 2.
- Prochaine étape : αβ 2 plis sélectif sur le même faisceau, éval « king distance » + territoire (méthode classique Amazons), fin de partie = remplissage.
- Fuzz : `bash tests/run.sh` (CG_NODES=3 = taille du faisceau pour Python, 17 s).
