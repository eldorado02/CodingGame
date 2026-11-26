# 6-nimmt-6-qui-prend-take-5 — plan

1. Protocole réel (sonde du 2026-11-26, l'énoncé se trompe d'ordre) : init
   `4 myId` ; par tour : dernières cartes (4 entiers, -1), 4 x (taille, cartes),
   scores (négatifs = vaches, -999 disqualifié), taille de main, main, puis la
   phase `CHOOSE_CARD_TO_PLAY` | `CHOOSE_LINE_TO_PICK`. Sortie `PLAY c` | `PICK i`
   (`RANDOM` accepté).
2. Invariant : cartes vues dans la manche (main, rangées, dernières cartes) ;
   remise à zéro quand la main a 10 cartes.
3. Algorithme : coût espéré par carte = P(Binomial(autres, trou/inconnues) >=
   places restantes) x vaches de la rangée ; carte sous toutes les rangées =
   rangée la moins chère. O(main x 104), négligeable.
4. Départage : coût, trou, carte ; rangée : vaches, index.
5. Fuzz : 200 parties simulées 1 à 5 manches, joueurs aléatoires, choix de rangée
   forcé, disqualifications.
6. Hypothèse : éviter les 6e cartes suffit à battre le milieu de tableau.
   Réfutation : place 3-4 contre les rangs ~100.
