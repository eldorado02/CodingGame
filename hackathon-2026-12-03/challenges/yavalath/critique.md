# yavalath — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Champs lus dans l'ordre (myId seulement au 1er tour, sinon `9`). VALIDÉ (referee `sendInputs`, `turn <= 1`).
- Vue normalisée 1 = moi. VALIDÉ (referee inverse 1/2 pour le joueur 2).
- Ligne ≥ 4 prioritaire sur 3. VALIDÉ (`CheckNeighbours` renvoie la plus longue ligne).
- Vol : autorisé au tour 1 (2e joueur). VALIDÉ par sonde.

### v0 — sonde protocole
- Constat : énoncé ambigu sur l'ordre des tours et le vol.
- Action : sonde Python qui vole toujours (`probe-*.log`).
- Résultat : l'adversaire (2e joueur) a volé notre 1er coup ; notre sonde a voulu revoler au tour suivant → coup illégal. Le vol n'existe qu'au 1er tour du joueur 2.
- Leçon : condition de vol = `myId == 2` **et** 1er tour, jamais « une seule pierre adverse ».

### v1 — αβ + coups forcés, soumission `41796345`
- Constat : jeu jamais soumis.
- Hypothèse : la tactique (menaces/blocages/suicides) domine ; une recherche avec extension des coups forcés suffit au milieu de tableau.
- Sources : referee MultiStruct/Yavalath, énoncé.
- Action : 4 langages, fuzz 200/200, 14000 nœuds.
- Résultat : 3/3 contre les rangs 133 et 61 ; CG 14000 nœuds = 48 ms au pire.
- Leçon : extension des coups forcés gratuite (branching 1–2) = profondeur effective bien supérieure à l'ID nominal (d=3–4).
