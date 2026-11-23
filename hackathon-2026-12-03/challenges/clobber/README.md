# clobber — guide de reprise

- Énoncé : `statement.md` ; referee github.com/darkhorse64/CG_Clobber. 8×8 en damier ; un coup = déplacer une pierre sur une pierre adverse orthogonalement adjacente (capture) ; plus de coup = perdu. Ligues Wood 2 → Wood 1. Boss Wood 2 : `3835619` (dépasse souvent le temps).
- Protocole : `boardSize`, couleur (`w`/`b`) ; par tour n lignes (haut → bas), `lastAction`, `actionsCount` (liste NON fournie : générateur maison). Sortie `e2e3`. 1000 ms / **150 ms**.
- Recherche : GitHub TidyMaze/clobber (Go, MCTS à playouts aléatoires ; variante minimax à éval de mobilité en commentaire) ; dépôts αβ académiques (natmali404/clobber-ai-simulation, CasRepoClone/ClobberAI).
- Bot soumis : **v1** `41796537` — negamax αβ ID, éval = mobilité (coups possibles nous − eux), comptage par décalages + popcount ; 400k nœuds tour 1, 60k ensuite (CG ≈ 40–50 ms, prof. 4–6).
- Bench : v1a 2/4, v1b 3/4 contre le boss (victoires surtout par timeouts du boss ; parties complètes perdues).
- Prochaine étape : génération de coups par décalages (×3 vitesse), TT, éval « pierres isolées » (pierre sans voisin adverse = morte), MCTS comme TidyMaze si le plafond persiste.
- Fuzz : `bash tests/run.sh` (tailles 5–8, CG_NODES=1500, 20 s).
