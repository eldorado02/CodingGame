# hex — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Lecture des champs : lignes avec espaces → suppression des blancs avant indexation. VALIDÉ.
- Orientation des voisins : l'énoncé ne la donne pas. VALIDÉ par test game (voir v0).
- Budget : itérations, pas de temps → déterministe ; garde-temps 70 ms. VALIDÉ.
- Byte-identité : flottants sans `log`, mêmes opérations dans le même ordre. VALIDÉ (fuzz 200/200).

### v0 — sondes d'orientation
- Constat : l'énoncé ne dit pas quelle diagonale est voisine.
- Hypothèse : convention HexWiki (`(-1,+1)` et `(+1,-1)` voisins).
- Sources : logs `games/hex/logs/probe*-*.log`.
- Action : bot Python qui joue l'anti-diagonale ; l'adversaire (rang 61) a swappé et gagné avec le chemin `(5,4)→(6,3)`.
- Résultat : le referee accepte `(+1,-1)` comme voisin → convention HexWiki confirmée. Les bots de queue (`[CG]Llama…`, `RomainPicot`) timeoutent ou jouent un coup illégal : inutiles comme sparring.
- Leçon : vérifier la géométrie par la fin d'une partie réelle avant d'écrire le moteur.

### v1 — MCTS + RAVE, soumission `41796234`
- Constat : jeu jamais soumis.
- Hypothèse : MCTS-RAVE simple suffit pour le milieu de tableau.
- Sources : ALGO.md §2.2, énoncé, recherche OpenCode (research.md).
- Action : 4 langages ; vecteurs STL remplacés par tableaux statiques (vector + pragma : 14 µs/itération) ; expansion après 3 visites (sinon ~40 nœuds créés par itération).
- Résultat : fuzz 200/200 ; **3/3** contre le rang 61 ; CG : 3000 itérations = 44 ms → budget 2600.
- Leçon : en MCTS, le coût dominant est la sélection sur ~120 enfants et l'expansion ; mesurer `ms=` dans les logs serveur.
