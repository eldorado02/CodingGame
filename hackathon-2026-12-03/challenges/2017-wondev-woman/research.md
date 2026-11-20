# Recherche — Wondev Woman (2017-06-23)

## Sources

- Statement officiel : `statement.md` (API `/api/v1/puzzles/wondev-woman`).
- Socle DSA : `../../dsa/README.md` (grille, BFS local).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Grille carrée 5–7, un seul pion par joueur. À chaque tour, la plateforme
**fournit la liste des actions légales** `MOVE&BUILD index dir1 dir2`.
Le pion se déplace d'une case (8 directions) en montant d'au plus 1 niveau,
puis doit élever d'1 une case adjacente à sa nouvelle position. Une case à
hauteur 4 est retirée du jeu. Victoire immédiate en se déplaçant sur une
case de hauteur 3.

## Décisions de conception

- On évalue chaque action légale à partir de la grille courante :
  - `h_dest` = hauteur de la case d'arrivée (unité + `dir1`) ;
  - `h_build` = hauteur de la case à élever (arrivée + `dir2`).
- Score (décroissant) :
  1. `h_dest == 3` → **victoire immédiate** (priorité absolue) ;
  2. déni : si `h_build == 3` et la case est adjacente à l'adversaire,
     l'élever à 4 la retire et bloque sa victoire imminente ;
  3. montée : `+1000 * h_dest` (aller vers le niveau 3) ;
  4. construction : `+300` si `h_build == 2` (crée un tremplin de niveau 3),
     `+100` si `h_build == 1`, `-50` si `h_build == 3` ailleurs.
- On ignore l'adversaire s'il est invisible (`-1 -1`).

## Risques

| Risque | Mitigation |
|---|---|
| Action illégale | On ne sort que des actions présentes dans `legalActions`. |
| Case d'arrivée hors grille / trou | Hauteur `-1` → score très négatif (jamais choisie). |
| Adversaire gagne en premier | Déni : retirer sa case de niveau 3 si possible. |
| Timeout 50 ms | ≤ 64 actions, grille ≤ 7×7 → trivial. |
| Divergence entre langages | Même grille de score entière, fuzz différentiel. |
