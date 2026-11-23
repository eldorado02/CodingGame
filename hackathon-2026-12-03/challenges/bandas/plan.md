# bandas — plan

1. Protocole : voir README. Une ligne de direction flushée par tour.
2. Invariant : aucun état ; grille relue à chaque tour.
3. Algorithme : port exact de `moveUp/Down/Left/Right` + `singleMove*` (récursion de poussée) + `findEmptyLines/Columns` ; `winner()` dans l'ordre du referee. Negamax αβ + ID, ordre UP RIGHT DOWN LEFT avec le meilleur coup précédent en tête à la racine ; budget 10000 nœuds, garde-temps 40 ms. Éval (joueur au trait) : Σ ±(100 + 3·min(distance de chute, 3)).
4. Départage : `>` strict, ordre fixe des directions.
5. Fuzz : 1–3 instantanés d'une partie aléatoire (placement comme le referee, directions aléatoires, trous par rognage).
6. Hypothèse : 7–8 plis de recherche exacte battent les bots heuristiques. Réfutation : défaite contre un bot plus profond (rang 44) → moteur bitboard.
