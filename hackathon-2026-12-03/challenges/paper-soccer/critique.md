# paper-soccer — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### Checklist plan (PLAYBOOK §3)
- Géométrie et règles copiées du referee (paires des poteaux, bords pré-dessinés). VALIDÉ.
- Fin de tour sur point bloqué = défaite du joueur. VALIDÉ (`getWinner`).
- Byte-identité : pas de flottant, ordre DFS fixe, dédoublonnage par (point, issue). VALIDÉ (fuzz 200/200).
- Budget : un nœud tardif peut coûter > 1 ms (DFS des rebonds) → garde-temps à chaque nœud. VALIDÉ après v1b.

### v1 — αβ tours complets, garde-temps 150 ms vérifié tous les 64 nœuds
- Constat : jeu jamais soumis.
- Hypothèse : αβ sur tours complets + distance BFS suffit pour le milieu de tableau.
- Sources : referee jdermont, Lecorbio `docs/algorithms.md`.
- Action : 4 langages, fuzz 200/200.
- Résultat : 6/6 contre les rangs 97 et 49 ; mais 153 ms en fin de partie (TIMEGUARD déclenché trop tard).
- Leçon : quand le coût d'un nœud varie de 1 à 1000 (énumération de coups composés), vérifier le temps à chaque nœud.

### v1b — garde-temps 80 ms à chaque nœud
- Action : `TIME_GUARD_MS = 80` (40 % de 200), vérification à chaque nœud (C/C++/Rust ; Python sans garde, c'est la référence).
- Résultat : 3/3 contre le rang 33 ; pire tour 81 ms ; soumission `41796586`.
- Leçon : garde-temps par nœud = coût négligeable, sécurité totale.
