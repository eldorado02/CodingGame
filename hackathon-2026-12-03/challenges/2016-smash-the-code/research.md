# Research — Smash The Code (2016-04-30)

## Source

- Statement officiel : `statement.md` (API CodinGame `/puzzles/smash-the-code`).
- Stub : `/puzzles/smash-the-code/stub-generator`.
- Aucun code tiers copié.

## Mécanique extraite de l’énoncé

Grille 6 colonnes × 12 lignes, lignes lues **du haut vers le bas** (ligne 0 = haut).
Chaque tour, une paire verticale `(A, B)` est donnée 8 tours à l’avance ; en
Wood `A == B` (contrainte confirmée). Les deux joueurs reçoivent les mêmes
paires. On choisit une colonne `x` ; les deux blocs tombent et s’empilent.

Résolution après dépôt, jusqu’à stabilisation :

1. Composantes connexes de même couleur (voisinage 4, pas de diagonale).
2. Toute composante de taille ≥ 4 disparaît.
3. Un bloc crâne (`0`) ne disparaît jamais seul, mais est retiré s’il est
   adjacent (4-voisin) à un bloc coloré effacé.
4. Gravité : chaque colonne se compacte vers le bas en conservant l’ordre.
5. Si de nouveaux groupes apparaissent, on recommence → **chaîne**.

Victoire : survivre ou meilleur score après 200 tours. Les groupes et chaînes
donnent des points de score et des « nuisance points » qui font tomber des
lignes de 6 crânes chez l’adversaire. En Wood, la formule exacte n’est pas
donnée : on optimise donc un proxy (blocs effacés + chaînes + hauteur).

## Décisions de conception

- **Pas de recherche arbre** en Wood : 6 coups seulement, et l’adversaire est
  sur une grille séparée (pas d’interaction directe). Une simulation exacte du
  dépôt + des clears + de la gravité suffit.
- **Évaluation** : `cleared * 10 + (chaines - 1) * 25 - pénalité de hauteur -
  pénalité de trous`. La hauteur pénalise les colonnes qui montent (risque de
  « top out » = défaite immédiate).
- **Crânes** : ils sont un malus (ils encombrent et ne se nettoient que par
  voisinage) ; on les compte comme obstacles dans la hauteur.
- **Complexité** : simulation O(72) par colonne, 6 colonnes → trivial sous 100 ms.
- **Idiomes par langue** : C = tableaux fixes + BFS file statique ; C++ =
  `std::array`, `std::queue`, RAII ; Rust = `Vec<Vec<u8>>`, `Result`, pas
  d’`unwrap` métier ; Python = `list[list[str]]`, `dataclass`, `typing`.

## Pourquoi pas DSA ici

Le socle `dsa/` couvre BFS/DFS et la file, réutilisés pour détecter les
composantes connexes. Le reste (Dijkstra, SCC, etc.) est inutile : on garde
chaque solution minimale et lisible, sans importer le header(par souci de
portabilité dans le binder CodinGame, on recopie les 15 lignes de BFS).
