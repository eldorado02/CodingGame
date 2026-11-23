# chain-reaction-1 — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### v1 (2026-11-23, réécriture : le code de la session OpenCode est perdu)
- Constat : aucune soumission ; ancien code (`/tmp/opencode`) disparu.
- Hypothèse : αβ + éval heuristique classique suffit pour le milieu de tableau.
- Sources : énoncé ; heuristique classique de Chain Reaction (orbes, vulnérabilité aux cases critiques adverses, coins/bords).
- Action : 4 langages, explosions par rondes simultanées (ordre d'index), arrêt quand l'adversaire n'a plus de case ; fuzz 200/200.
- Résultat : à 150k nœuds/tour, TIMEGUARD sur CG presque à chaque tour (CG ≥3× plus lent que local sur ce code) ; 1/3 rang 37, 0/2 rang 19. Budget abaissé à 50k. Soumission `41796092`.
- Leçon : calibrer le budget sur les logs `d= n= ms=` du serveur, pas sur le local ; les explosions coûtent cher (boucle de rondes).
