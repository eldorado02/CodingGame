# oware-abapa — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Oware Abapa — v1 αβ soumise
- Constat : jeu jamais joué ; ligues Wood 2 → Wood 1
- Hypothèse : αβ prof. ~10 sur règles exactes du referee bat le boss
- Sources : referee MaximeCheramy/cg-oware (lu), marchete/CGZero, joansalasoler/oware (Aalina) via gh_search.sh
- Action : 4 langages, fuzz 200/200 auto-joué, scores reconstitués depuis les graines disparues
- Résultat : 4/4 contre le boss `2469367` (25–7) ; soumission `41796595`
- Leçon : lire le referee donne des règles exactes (grand chelem, obligation de nourrir) qu'un énoncé tronqué omet

### 2026-11-23 — Oware Abapa — v1.1 correctif joueur 1
- Constat : après soumission v1 : score 2,63, rang 45/54 alors que 4/4 contre le boss en test
- Hypothèse : en test nous sommes toujours joueur 0 ; le chemin joueur 1 est faux
- Sources : referee MaximeCheramy/cg-oware Player.getAction : num += 6 pour le joueur 1
- Action : sortie = index relatif 0..5 dans les 4 langages ; fuzz 200/200
- Résultat : re-soumission `41796829` (comportement joueur 0 inchangé, 4/4 conservé)
- Leçon : les test games ne couvrent que le siège 0 : lire le parsing de sortie du referee pour l'autre siège, et surveiller le score juste après soumission
