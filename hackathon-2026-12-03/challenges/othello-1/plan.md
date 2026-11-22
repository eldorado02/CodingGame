# Othello — plan

## Objectif

Atteindre la Ligue Légende. Étape courante : consolider Wood 2 puis gravir les
ligues supérieures.

## Algorithme retenu (bitboard, déterministe)

1. Représentation : deux `uint64_t` P (nous) et O (adversaire), bit `r*8+c`.
2. `moves(P,O)` par dilatation directionnelle (8 directions, 6 étages) ;
   `flips(P,O,mv)` par marche directionnelle.
3. Évaluation : table positionnelle (coins +120, X −40, C −20), mobilité
   (`±20`), frontier (`±8`), matériel `+8` seulement si ≤12 cases vides.
4. Ordre des coups : bits par ordre croissant, tri insertion stable par
   `POS[bit]` décroissant.
5. Alpha-bêta itératif profondeurs 1→6, élagage à la racine, coupure
   `alpha >= beta`, arrêt anticipé sur gain forcé.
6. Protocole tolérant (lignes de plateau détectées par leur contenu), sortie
   avec flush.

## Ports et tests

- 4 langages : Python3 (référence fuzz), C, C++, Rust.
- Fuzz différentiel : `bash tests/run.sh` → 200 cas.
- Validation bitboards : 10 000 positions aléatoires vs référence tableau.
- Test games C++ : **5/5** contre `6797560`.
- Soumissions : `41787330` (temps réel, en cours) ; bitboard à soumettre.

## Pièges rencontrés

- `sh(O)&P` puis dilatation est un no-op : la dilatation doit partir des pions
  propres (`sh(P)&O`).
- `id`/`boardSize` absents après le 3ᵉ tour : ignorer les lignes non-plateau.
- `sync_with_stdio(false)` + `'\n'` ne vide pas le flux : utiliser `flush`.
