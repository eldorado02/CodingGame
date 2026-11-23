# bandas — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Ordre de balayage et poussée récursive identiques au referee. VALIDÉ (port ligne à ligne).
- Rognage : lignes puis colonnes, du haut puis du bas, arrêt à la première ligne avec un pion. VALIDÉ.
- 0 pion des deux côtés → joueur 1 gagne (`checkWinner`). VALIDÉ (reproduit).
- Budget : 29 ms au pire sur CG. VALIDÉ.

### v1 — αβ profondeur 7–8, soumission `41796751`
- Constat : jeu jamais soumis.
- Hypothèse : branching 4 → recherche profonde + éval matériel/chute suffit pour le milieu de tableau.
- Sources : referee Oli8/CG-bandas.
- Action : 4 langages, fuzz 200/200.
- Résultat : 6/6 contre les rangs 302 et 130 ; 0/2 contre le rang 44.
- Leçon : 1,8 µs/nœud sans `-O2` (grille en `int`, recomptages) : un moteur bitboard est le levier suivant.
