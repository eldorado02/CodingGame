# fireworks — plan

1. Protocole (sondes 2026-11-26) : par tour `erreursRestantes infosRestantes`,
   n lignes : événements depuis notre dernier tour (`p:NEWGAME`, `p:PLAY|ERROR|
   DISCARD:L:COULEUR-N`, `p:SAYLEVEL|SAYCOLOR:q:V` suivi des lignes de
   connaissance `q:CARD:L:?-N` / `q:CARD:L:COULEUR-?`), puis les mains complètes
   des 2 coéquipiers. Carte jouée remplacée dans le même emplacement.
   Sortie `PLAY:L`, `DISCARD:L`, `SAY:q:VALEUR`.
2. Invariant : feux, connaissance par (joueur, emplacement), pioche (35), nos
   emplacements (retirés quand la pioche est vide).
3. Algorithme : jouer une carte sûre > indice sûr (n'éclaire que des cartes
   jouables, attribut pas encore connu) > défausse (inutile, sans indice, A)
   > indice inoffensif.
4. Départage : coéquipier suivant d'abord, emplacements triés.
5. Fuzz : 200 flux (NEWGAME, indices, connaissances complètes, déchets).
6. Résultat : 2/3, 2/3 (1er sur 4) contre 3 boss.
