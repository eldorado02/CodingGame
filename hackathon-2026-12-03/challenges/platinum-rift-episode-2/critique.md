# platinum-rift-episode-2 — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Platinum Rift - Episode 2 — v1 s'étaler + marcher sur le QG
- Constat : jeu non soumis ; bench affichait me=None
- Hypothèse : bench ne lisait que 12 lignes d'en-tête, les options de carte repoussent le bloc agents
- Sources : log de test (bloc options map=...)
- Action : bench.py lit le bloc agents: où qu'il soit ; bot règles BFS QG ennemi
- Résultat : fuzz 200/200 ; 5/7 contre rang 500/1000 ; soumis
- Leçon : outil : ne jamais supposer une position fixe dans l'en-tête du log
