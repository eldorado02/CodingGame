# abalone — plan

1. Protocole (sonde 2026-11-26) : init `myId` (1 blanc, 2 noir ; noir commence) ;
   par tour `myScore oppScore`, 9 lignes de 5,6,7,8,9,8,7,6,5 chiffres (`0`
   vide), dernier coup adverse, liste des coups légaux `fx fy lx ly dir`.
2. Coordonnées : x = index dans la ligne, y = ligne ; axial r = y - 4,
   q = x + max(-4, -r-4). Directions 0 E, 1 NE, 2 NW, 3 W, 4 SW, 5 SE.
3. v1 : simulation de chaque coup légal (en ligne + Sumito, latéral) ; score =
   10000 x billes sorties - 10 x nos distances au centre + 5 x les leurs
   + 2 x nos paires voisines.
4. Départage : premier coup légal.
5. Fuzz : 200 cas, coups alignés aléatoires (poussées, sorties, latéraux).
6. Résultat : 4/4 contre boss Wood 2 (6-0).
