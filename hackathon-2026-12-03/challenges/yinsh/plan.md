# yinsh — plan

1. Protocole (sonde 2026-11-26) : init `myId`, on répond `yes` (coups légaux).
   Par tour : `11`, 11 lignes = colonnes a..k (numéros croissants, débuts
   2,1,1,1,1,2,2,3,4,5,7 ; tailles 4,7,8,9,10,9,10,9,8,7,4 ; `.RSrs`), puis
   `actionCount` actions complètes (`c6`, `f7-f2`, `xA-BxC;m-n`, `STEAL`).
2. Invariant : aucun, plateau relu chaque tour.
3. Algorithme : 1 pli glouton ; simulation de chaque action (marqueur laissé,
   retournements, retraits) ; score = 1000 x anneaux retirés + fenêtres de 5
   d'une seule couleur (+m² / -t²) + marqueurs + mobilité des anneaux.
4. Départage : première action de la liste ; STEAL jamais joué.
5. Fuzz : 200 cas, actions valides, mal alignées, retraits, majuscules, déchets.
6. Hypothèse : 1 pli suffit contre le milieu. Résultat : 4/4 contre rang 71/142.
