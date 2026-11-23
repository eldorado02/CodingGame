# penguins — guide de reprise

- Énoncé : `statement.md` (« Hey, That's my fish! »). Grille hexagonale 8 rangées alternant 7/8 cases ; 2–4 joueurs ; déplacement en ligne droite (6 directions) sans traverser trou/pingouin ; quitter un bloc le retire et mange ses poissons (100 pts/poisson + 1/bloc). Ligues Wood 3 → Wood 1. Boss Wood 3 : `6797562` (pas aléatoire : gagne nettement).
- Protocole (sonde) : nbPlayers, playerId, pingouins/joueur ; par tour nbRows + lignes (rangée 1 en premier, chiffres = poissons), scores, pingouins `id owner pos` (**id unique par joueur seulement**), actions `MOVE id POS`. 1 s / 50 ms.
- Géométrie (vérifiée sur les listes de coups) : rangées impaires (index pair) étroites ; diagonales : depuis une rangée étroite droite = x+1 / gauche = x, depuis une large droite = x / gauche = x−1.
- Bot soumis : **v2** `41796912` — glouton : 10 × poissons(source) + poissons(destination) + mobilité de la destination.
- Bench : v1 (destination) 0/3 serrés ; v1b (+ source) 1/4 ; v2 (+ mobilité) 0/4.
- Prochaine étape (v3) : recherche paranoïde αβ + éval Voronoi des poissons accessibles par pingouin (territoire), comme Isola/Amazons.
- Fuzz : `bash tests/run.sh`.
