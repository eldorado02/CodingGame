# mad-knights — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### v1 (2026-11-23)
- Constat : jeu jamais joué ; 3 joueurs.
- Hypothèse : la recherche paranoïde (coalition adverse) avec mobilité suffit pour le milieu de tableau.
- Sources : énoncé ; Knight Isolation (Udacity, cité par l'énoncé) ; ALGO.md classe P (partition).
- Action : 4 langages, élimination d'un joueur bloqué sans consommer de profondeur, fuzz 200/200.
- Résultat : 3/3 contre rangs 69 + 86 ; soumission `41796137`.
- Leçon : pour un jeu à N joueurs, le paranoïde réutilise tel quel le moteur αβ 2 joueurs.
