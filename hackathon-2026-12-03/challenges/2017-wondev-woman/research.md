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

## Correctif protocole (2026-11-21) — source = stub officiel + referee

Le bot initial plantait au tour 1 (`ValueError: invalid literal for int():
'4 2'`) et finissait avec un score de **−1,55**. Deux sources ont fait foi :

1. `cg.py stub wondev-woman` → `read unitsPerPlayer:int` puis **deux** lignes
   de positions par camp, puis `read legalActions:int` et autant de lignes
   `atype index dir1 dir2`.
2. Referee officiel
   [`CodinGame/WondevWoman/Referee.java`](https://github.com/CodinGame/WondevWoman/blob/master/Referee.java) :
   - `getInputs()` envoie `unitsPerPlayer` lignes par joueur (Wood 1 réel :
     **2 unités**, pas 1 comme dans l'énoncé archivé) ;
   - `handlePlayerOutput()` ne lit que **`outputs[0]`** → une seule action par
     tour, même avec 2 unités ;
   - `computeMove()` : `if (targetHeight == FINAL_HEIGHT - 1) scorePoint = true`
     → **marcher** sur une case de hauteur 3 marque un point (construire dessus
     la fait passer à 4 = case retirée) ;
   - `ACCEPT-DEFEAT` est une commande valide, `WAIT` ne l'est pas.

Conséquence : la grille de score a été réécrite (montée `400*h`, case de score
adjacente `+1500`, tremplin 2→3 `+3000`, déni près de l'adversaire `+50 000`,
victoire `+1 000 000`) et la sortie est une seule ligne.
