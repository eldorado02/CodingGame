# back-to-the-code — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-26 — Back to the Code — v1 boucles rectangulaires
- Constat : jeu non soumis
- Hypothèse : rectangles sûrs au meilleur ratio gain/périmètre suffisent pour être classé
- Sources : énoncé (règles d'encerclement, déplacement case par case)
- Action : sommes préfixes 2D, rectangles <= 12x12 avec nous sur le bord, rejet si adversaire à < coût/2
- Résultat : fuzz 200/200 ; 1/4 contre rang 500/1000 (scores serrés 264-412) ; soumis (premier envoi)
- Leçon : piste v2 : BACK pour annuler une boucle coupée, rectangles appuyés sur nos cellules existantes
