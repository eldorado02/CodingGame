# penguins — critique et journal des tentatives

Format : constat / hypothèse / sources / action / résultat / leçon.

### 2026-11-23 — Penguins — v1→v2 gloutonnes, v2 soumise
- Constat : jeu jamais joué ; grille hexagonale décalée, 2–4 joueurs
- Hypothèse : un glouton poissons + mobilité suffit contre le boss Wood 3
- Sources : énoncé, 2 sondes (géométrie déduite des listes de coups légaux), redblobgames hexagons (cité par l'énoncé)
- Action : v1 destination, v1b + poissons mangés à la source, v2 + mobilité ; ids de pingouins par joueur corrigés ; fuzz 200/200
- Résultat : 0/3, 1/4, 0/4 contre le boss `6797562` ; v2 soumise `41796912` (1re soumission)
- Leçon : le boss Wood 3 cherche : un glouton perd de ~1 poisson ; il faut une recherche + éval de territoire
