# Recherche — Hypersonic (2016-09-24)

## Sources

- Statement officiel : `statement.md` (API CodinGame `/api/v1/puzzles/hypersonic`).
- Socle DSA : `../../dsa/README.md` (BFS, file).
- Aucun code tiers copié.

## Mécanique (Wood 3)

Grille 13 × 11. Origine haut-gauche. 30–65 boîtes placées symétriquement.
Deux joueurs dans des coins opposés. Actions simultanées, 100 ms/tour.

- `MOVE x y` : le moteur calcule le **plus court chemin** vers (x,y) et avance
  d’une case. Cible inaccessible → cellule valide la plus proche.
- `BOMB x y` : pose une bombe sur la case courante puis avance d’une case vers
  (x,y). Une seule bombe active à la fois (`param1` du joueur = 0 → plus de
  bombe disponible). On peut rester sur sa propre bombe.
- Bombes : timer 8, décrémenté en début de tour ; explosion à 0 **avant** les
  déplacements. Portée 3 en croix, arrêtée par boîte ou bombe (cases
  d’obstruction incluses). Chaîne : une bombe touchée explose aussi.
- Boîtes détruites = 1 point **pour chaque propriétaire** dont l’explosion
  touche la boîte (un même box peut compter 2 fois).
- En Wood, les joueurs ne sont pas blessés. Fin après 200 tours.

## Décisions de conception

- **BFS sur la grille** (`MOVE` intelligent) : le moteur fait le chemin, mais
  on doit viser une case **sans bombe** (on ne peut pas entrer sur une case
  occupée par une bombe). On calcule donc soi-même le plus court chemin vers
  la boîte cible en évitant les bombes, et on renvoie la case voisine suivante.
- **Choix de la bombe** : poser quand on est adjacent à une boîte avec au moins
  2 boîtes alignées dans la portée (rentabilité), ou dès qu’une bombe peut être
  posée et qu’on a une boîte à portée. On vise une case de fuite sûre.
- **Évitement des bombes alliées et ennemies** : toutes les bombes avec timer
  bas sont des obstacles. Après avoir posé, on s’éloigne d’au moins 4 cases.
- **Cibles** : boîtes les plus proches (BFS), avec pénalité si une bombe
  ennemie est déjà proche d’elles (on laisse l’adversaire les casser).
- **Idiomes** : C tableaux 13×11 + BFS file statique, C++ `array`/`queue`,
  Rust `Vec<Vec<u8>>`/`VecDeque`, Python listes/`deque`.

## Risques

| Risque | Mitigation |
|---|---|
| Poser une bombe sans boîte à portée | Ne poser que si au moins 1 boîte dans la croix de portée 3. |
| Se retrouver bloqué par une bombe | BFS évite les cases à bombe ; fuite après pose. |
| Timer mal anticipé | On ne pose que si on peut s’éloigner de 4 cases en 8 tours (toujours vrai). |
| Dégâts adverses | Wood : aucun dégât, on ignore. |
