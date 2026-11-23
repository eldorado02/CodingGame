# yavalath — plan

1. Protocole : voir README (myId au 1er tour seulement, `9` + 9 lignes + `x y`). Une ligne de sortie `x y` flushée.
2. Invariant d'état : aucun état sauf `myId` et « 1er tour » (vol). Plateau relu à chaque tour.
3. Algorithme : `analyse(p)` classe chaque case vide : gain (ligne ≥ 4), suicide (ligne = 3), neutre ; et pour l'adversaire. Gain → on joue. Cases gagnantes adverses → coups forcés (recherche sans décrément de profondeur). Sinon coups neutres, αβ + ID, ordre par fenêtres de 4 (stable, insertion). Feuille : `10·(fenêtres 2 pierres) + 40·(3 pierres) + 3·(coups sûrs moi − lui)`. Budget en nœuds (`CG_NODES`).
4. Départage : `>` strict, ordre des cases = ordre d'entrée, tri par insertion stable.
5. Fuzz : 1ers tours (vide, 1 pierre adverse pour le vol), parties aléatoires sans ligne de 3 coupées à 1–50 plis (blocages forcés, positions perdues, gains immédiats).
6. Hypothèse de victoire : les bots du milieu de tableau ne gèrent pas les doubles menaces ni le zugzwang (plus de coup sûr). Réfutation : défaite par manque de coups sûrs en fin de partie → augmenter `W_SAFE` / profondeur.
