# domain-expansion — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Déplacement : reproduction exacte du BFS `findPath` (profondeur d'expansion 3, chemin ≤ 4). VALIDÉ par lecture du referee ; 6 parties sans coup illégal.
- Fin de partie évaluée après chaque action du joueur qui vient de jouer. VALIDÉ (`Referee.gameTurn`).
- Mur sur un mur existant ou sur le bord = illégal → on ne génère que les côtés libres. VALIDÉ.
- Budget : 28 ms au pire sur CG. VALIDÉ.

### v1 — αβ + Voronoi, soumission `41796695`
- Constat : jeu jamais soumis, aucun bot public trouvé.
- Hypothèse : fin exacte + Voronoi suffisent pour le milieu de tableau.
- Sources : referee Mortis66666 (lu), boss Python aléatoire.
- Action : 4 langages, fuzz 200/200.
- Résultat : 5/6 (2/3 rang 43, 3/3 rang 25).
- Leçon : le Voronoi ne voit pas les fermetures en 2 coups ; il faut une notion de zone fermable.
